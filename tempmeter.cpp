/*
 * tempmeter.cpp
 *
 * Created: 22.3.2015 10:52:29
 *  Author: Kremmen
 */

/*
Lämpötilan mittaus- ja säätötaski
Lämpötila mitataan MAX31855KASA -termoparimittareilla.
SPIkanavan kautta luettava data on 32-bittinen sana jonka MSB siirtyy ensin
Bitit on järjestetty seuraavasti:
D31:		termoparin mitta-arvon etumerkki
D30 ... D18	termoparin mitattu lämpö (MSB = 1024 C, LSB = 0,25 C)
D17:		varattu
D16:		diagnostiikka; 1 = fault
D15:		kompensointimittauksen etumerkki
D14 ... D4	kompensointilämpötila (MSB = 64 C, LSB = 0,0625 C)
D3:			varattu
D2:			1 = oikosulku Vcc
D1:			1 = oikosulku GND
D0:			1 = termopari avoin piiri
*/

#include "FreeRTOS_ARM.h"
#include "Arduino.h"
#include <stdio.h>
#include "tempmeter.h"
#include "SMDOvenGPIO.h"
#include "OvenControl.h"
#include "flashconfig.h"
#include "sequence.h"
#include "PID.h"

#define CH_UPPERSENSOR 11
#define CH_LOWERSENSOR 12
#define MISO2 6
#define SCK2 7
#define CLK_ACTIVE LOW


class tempdata {
public:
	void begin(uint8_t ch);
	void update();
	float temp() { return temperature; }
	uint8_t isFault() { return fault; }
	uint8_t getFault();
private:
	uint8_t channel;
	uint32_t raw;
	int16_t hitemp;
	int16_t lotemp;
	float temperature;
	boolean fault;
	uint8_t short_vcc;
	uint8_t short_gnd;
	uint8_t open_circuit;
};
	
void tempdata::begin( uint8_t ch) {
	channel = ch;
	pinMode(ch, OUTPUT);
	digitalWrite(ch, HIGH);
	
}

void tempdata::update() {
uint32_t tmp = 0;
	digitalWrite(channel, LOW);
	
	for (uint8_t cnt = 0; cnt < 32; cnt++ ) {
		digitalWrite(SCK2, CLK_ACTIVE);
		tmp = tmp << 1;
		digitalWrite(SCK2, !CLK_ACTIVE);
		if ( digitalRead(MISO2) ) tmp |= 1;
	}
	digitalWrite(channel, HIGH);
	//päivitetään mittaustiedot
	raw = tmp;
	hitemp = ( raw & 0xfffc0000 ) >> 18;
	lotemp = ( raw & 0x0000fff0 ) >> 4;
	temperature = ( 0.25 * hitemp ); // - ( 0.0625 * lotemp );
	fault =			(( tmp & 0x00010000 ) > 0);
	short_vcc =		( tmp & 0x04 );
	short_gnd =		( tmp & 0x02 );
	open_circuit =	( tmp & 0x01 );
	
}

uint8_t tempdata::getFault() {
	return short_vcc + short_gnd + open_circuit;
}


// säätäjän parametrit, tilamuuttujat ja signaalit
runState currentRunState;
runState prevRunState;
tempdata temperature_upper, temperature_lower;


float setpoint = 0.0; 
float actual = 0.0;
float PID_output = 0.0;
Sequence seq;
PID *controller;

#define MODULATORMAX 100
#define CYCLETIME 1000		// ms

// kernelin jonot ja viestipuskurit
resourceStruct_t *q;
QueueHandle_t controlQueue, responseQueue, loggingQueue;
toCTMessage command;
toLogMessage loggingcommand;
toGUIMessage reply;
char msg0[80];
char msg1[40];
char msg2[40];

// loggeri 
void sendLogMessage() {
	sprintf(msg0,"set;%3.1f;act;%3.1f;out;%3.1f;P;%3.1f;I;%3.1f;D;%3.1f", setpoint, actual, PID_output, controller->getPTerm(), controller->getITerm(), controller->getDTerm() );
	if ( temperature_upper.isFault() ) {
		sprintf(msg1, ";up;fault %d", temperature_upper.getFault());
	}
	else {
		sprintf(msg1, ";up;%3.1f", temperature_upper.temp());
	}
	
	if ( temperature_lower.isFault() ) {
		sprintf(msg2, ";dn;fault %d", temperature_lower.getFault());
	}
	else {
		sprintf(msg2, ";dn;%3.1f", temperature_lower.temp());
	}
	loggingcommand.mtype = log_Start;
	loggingcommand.mvalue.p[0] = msg0;
	loggingcommand.mvalue.p[1] = msg1;
	loggingcommand.mvalue.p[2] = msg2;
	xQueueSendToBack( loggingQueue, &loggingcommand, portMAX_DELAY );
}



void supervisorTask( void *pvParameters ) {
	q = (resourceStruct_t *)pvParameters;
	controlQueue = q->cQ;
	responseQueue = q->qQ;
	loggingQueue = q->lQ;

	for ( ; ; ) {
		xQueueReceive( controlQueue, &command, portMAX_DELAY );
		switch ( command.mtype ) {
			case sequence_Stop: {
				currentRunState = onIdle;
				break;
			}
			case sequence_Start: {
				if ( seq.load( command.mvalue.i[0] ) ) {
					currentRunState = onAuto;
				}
				break;
			}
			case query_RunStatus: {
				reply.mtype = rsp_RunStatus;
				reply.mvalue.i[0] = currentRunState;
				xQueueSendToBack( responseQueue, &reply, portMAX_DELAY );
				break;
			}
			case query_PIDsetpoint: {
				reply.mtype = rsp_Processvalues;				
				break;
			}
			case set_PIDsetpoint: {
				if ( command.mvalue.f[0] > 250.0 ) setpoint = 250.0;
				else setpoint = command.mvalue.f[0];	// setpoint in C
				if ( setpoint > 0.0 ) {
					currentRunState = onManual;
				}
				else {
					currentRunState = onIdle;
				}
				break;
			}
			case query_Kvalues: {
				reply.mtype = rsp_Kvalues;
				reply.mvalue.f[0] = config.pid_P;
				reply.mvalue.f[1] = config.pid_I;
				reply.mvalue.f[2] = config.pid_D;
				xQueueSendToBack( responseQueue, &reply, portMAX_DELAY );
				break;
			}
			case set_Kvalues: {
				config.pid_P = command.mvalue.f[0];
				config.pid_I = command.mvalue.f[1];
				config.pid_D = command.mvalue.f[2];
				config.store();
				controller->SetTunings( config.pid_P, config.pid_I, config.pid_D );
				break;
			}
		}
	}
}

void controllerTask( void *pvParameters ) {
TickType_t xLastWakeTime;
TickType_t mod_Input;

	controller = new PID( &actual, &PID_output, &setpoint, config.pid_P, config.pid_I, config.pid_D, DIRECT );
	pinMode(MISO2, INPUT);
	digitalWrite(SCK2,!CLK_ACTIVE);
	pinMode(SCK2, OUTPUT);

	temperature_upper.begin(CH_UPPERSENSOR);
	temperature_lower.begin(CH_LOWERSENSOR);

	// Alustetaan säätäjä
	controller->SetSampleTime(CYCLETIME);
	controller->SetOutputLimits( 0.0, MODULATORMAX );
	controller->SetTunings(config.pid_P, config.pid_I, config.pid_D);
	setpoint = 0;
	currentRunState = onIdle;
	prevRunState = onIdle;
	xLastWakeTime = xTaskGetTickCount();
	
	for ( ;; ) {
		// mitataan lämmöt
		temperature_upper.update();
		temperature_lower.update();
		actual = temperature_lower.temp();
		
		// ajetaan säätäjä
		switch ( currentRunState ) {
			case onAuto: {
				if ( prevRunState != onAuto ) {
					seq.startSequence( xTaskGetTickCount() );
					controller->SetMode(AUTOMATIC);
					digitalWrite( FAN, HIGH );
				}
				setpoint = seq.interpolate( xTaskGetTickCount() );
				controller->Compute();
				if ( seq.isComplete() )	currentRunState = onIdle;
				break;
			}
			case onManual: {
				if ( prevRunState != onManual ) {
					controller->SetMode(AUTOMATIC);
					digitalWrite( FAN, HIGH );
				}
				controller->Compute();
				break;
			}
			case onIdle: {
				if ( prevRunState != onIdle ) {
					controller->SetMode(MANUAL);
					digitalWrite( FAN, LOW );
				}
				break;
			}
		}
		
		// ajetaan modulaattori
		if ( currentRunState != onIdle ) {
			mod_Input = CYCLETIME / MODULATORMAX * PID_output;
			//modulaattori
			if ( mod_Input > 0 ) {
				digitalWrite( LED1, HIGH );
				digitalWrite( PWR1, HIGH );
				vTaskDelay( mod_Input );
				digitalWrite( LED1, LOW );
				digitalWrite( PWR1, LOW );
			}
			else {
				digitalWrite( LED1, LOW );
				digitalWrite( PWR1, LOW );
			}
			sendLogMessage();
		}
				
		prevRunState = currentRunState;
		vTaskDelayUntil(&xLastWakeTime, CYCLETIME);
	}
}
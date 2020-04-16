/*!	\file SMDOvenControl.ino
	\brief Pizzauunista häksätty SMD-juotosuuni

Author: Kremmen

Tässä projektissa on modattu pizzauunia paremmin jutoskäytöön sopivaksi lisäämällä tehokkaampi lämmitys,
puolijohdereleisiin perustuva tehonohjaus ja tarvittavat apujännite- ja suojapiirit.
Ohjaus on toteutettu käyttäen Arduino Due-lautaa kontrollerina sekä custom hardwarea varsinaisen uunin ohjaukseen.
Yksinkertaisen moniajon mahdollistamiseksi ohjelma on toteutettu  FreeRTOS-skedulerin hallinnoimina taskeina.
*/


#include "FreeRTOS_ARM.h"
#include "utility/timers.h"
#include "list.h"
#include "cartesian.h"
#include "allFonts.h"
#include "DueFlashStorage.h"
#include "flashconfig.h"
#include "sequence.h"


#include "GUI.h"					// käyttäliittymätaski
#include "GUIEntryDialogs.h"
#include "tempmeter.h"				// lämpötilan mitaustaski
#include "logger.h"					//lokitustaski
#include "ILI_SdSpi.h"
#include "ILI_SdFatConfig.h"
#include "ILI9341_due_gText.h"
#include "ILI9341_due.h"
#include "PID.h"
#include "SMDOvenGPIO.h"
#include "OvenControl.h"



resourceStruct_t queues;	//!< Struktuuri johon talletetaan taskien välisten viestijonojen osoitteet

/*! \def setup()
	\brief Arduinon setup()
	
	Setupissa alustetaan GPIO, luodaan taskien viestijonot ja mikäli onnistuu, luodaan varsinaiset taskit.
	Jos kaikki on OK, kernel käynnistetään ja sovellus starttaa.
*/
void setup( void ) {
	//!< Alustetaan GPIO
	digitalWrite( PWR1, LOW );
	digitalWrite( PWR2, LOW );
	digitalWrite( FAN, LOW );
	digitalWrite( LED1, LOW );
	digitalWrite( LED2, LOW );
	pinMode( PWR1, OUTPUT );
	pinMode( PWR2, OUTPUT );
	pinMode( FAN, OUTPUT );
	pinMode( LED1, OUTPUT );
	pinMode( LED2, OUTPUT );

	pinMode(BEEPER, OUTPUT);
	digitalWrite(BEEPER, LOW);
	pinMode(BACKLIGHT, OUTPUT);
	digitalWrite(BACKLIGHT, HIGH);
	pinMode(PUMP, OUTPUT);
	digitalWrite(PUMP, LOW);
	
	
	// odotetaan pari sekuntia jotta ulkoinen 9V jännite ehtii stabiloitua ennen kuin yritetään jatkaa
	// muussa tapauksessa esim. näyttö ei välttämättä lähde käyntiin oikein
	delay(2000);
	
	// ja vielä varmuudeksi uudestaan
	pinMode( PWR1, OUTPUT );
	pinMode( PWR2, OUTPUT );
	pinMode( FAN, OUTPUT );
	pinMode( LED1, OUTPUT );
	pinMode( LED2, OUTPUT );
	digitalWrite( PWR1, LOW );
	digitalWrite( PWR2, LOW );
	digitalWrite( FAN, LOW );
	digitalWrite( LED1, LOW );
	digitalWrite( LED2, LOW );

	pinMode(BEEPER, OUTPUT);
	digitalWrite(BEEPER, LOW);
	pinMode(BACKLIGHT, OUTPUT);
	digitalWrite(BACKLIGHT, HIGH);
	pinMode(PUMP, OUTPUT);
	digitalWrite(PUMP, LOW);

	initFlash();

	Serial.begin(9600);

	//!< Luodaan taskien väliset kommunikointijonot
	queues.cQ = xQueueCreate( 5, sizeof( toCTMessage ) );
	queues.qQ = xQueueCreate( 5, sizeof( toGUIMessage ) );
	queues.lQ = xQueueCreate( 5, sizeof( toLogMessage ) );
	
	//!< jos jonot luotiin OK niin voidaan luoda sovelluksen taskit
	if( ( queues.cQ != NULL ) && ( queues.lQ != NULL ) && ( queues.lQ != NULL ) ) {
		xTaskCreate( GUITask, "GUI", 2048, &queues, 2, NULL );					//!< näytön ohjaus ja käyttöliittymä
		xTaskCreate( loggerTask, "TEMP", 1024, &queues, 2, NULL );				//!< lokitus sarjaporttiin
		xTaskCreate( supervisorTask, "SUPR", 1024, &queues, 3, NULL );			//!< sekvenseri ja lämpötilan säätö
		xTaskCreate( controllerTask, "CTRL", 1024, &queues, 3, NULL );			//!< sekvenseri ja lämpötilan säätö
		vTaskStartScheduler();
	}
	//!< tähän ei pitäisi tulla ikinä
	Serial.println("Queue creation failed!");
	for( ; ; ) {}
}

/*! \def loop()
	\brief Ardun looppia ei tarvita mihinkään joten se jää tyhjäksi.
*/
void loop() {
	for ( ; ; ) {}
}

boolean pumpbit = false;

/*! \def vApplicationTickHook()
	\brief FreeRTOS-kernelin Tick hook proseduuri.
	
	Tämä ajetaan jokaisella skedulerin kellokeskeytyksellä
	Tick hook pulssittaa apukortilla olevan varauspumpun ohjaussignaalia.
	Pumppu enabloi lämmitysvastusten ohjaussignaalit.
	Jos FreeRTOSin kello lakkaa tikittämästä niin enable poistuu hetikohta
	jolloin kaikki ohjaukset menevät pois päältä.
*/
extern "C"{  // FreeRTOS olettaa että käytässä on C-tyylinen linkitys
void vApplicationTickHook( void ) {
	digitalWrite(PUMP, pumpbit);
	pumpbit = !pumpbit;
}

} //extern "C"
/*-----------------------------------------------------------*/


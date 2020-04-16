/*
 * logger.cpp
 *
 * Created: 17.5.2015 11:42:19
 *  Author: martti
 */
 

#include "FreeRTOS_ARM.h"
#include "Arduino.h"
#include "OvenControl.h"


void loggerTask( void *pvParameters ) {
	QueueHandle_t loggingQueue;
	resourceStruct_t *queues;
	toLogMessage message;

	queues = (resourceStruct_t *)pvParameters;
	loggingQueue = queues->lQ;

	for ( ; ; ) {
		xQueueReceive( loggingQueue, &message, portMAX_DELAY );
		Serial.print( xTaskGetTickCount() );
		Serial.print( ";" );
		for (uint8_t i = 0; i < 3; i++ ) {
			if ( message.mvalue.p[i] ) Serial.print( (char *)message.mvalue.p[i] );
		}
		Serial.println();
	}
}

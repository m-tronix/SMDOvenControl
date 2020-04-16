/*
 * tempmeter.h
 *
 * Created: 22.3.2015 10:52:08
 *  Author: Kremmen
 */ 


#ifndef TEMPMETER_H_
#define TEMPMETER_H_


void supervisorTask( void *pvParameters );
void controllerTask( void *pvParameters );


#endif /* TEMPMETER_H_ */
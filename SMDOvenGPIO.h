/*
 * SMDOvenGPIO.h
 *
 * Created: 15.5.2015 15:14:25
 *  Author: martti
 */ 


#ifndef SMDOVENGPIO_H_
#define SMDOVENGPIO_H_

// Lämpötehon ohjaukset
#define PWR1 2
#define PWR2 3
// puhallin
#define FAN 4
// ilmaisinledit
#define LED1 22
#define LED2 23
// piippari
#define BEEPER 24
#define BEEPERONTIME 50			// ms
// näytön taustavalo
#define BACKLIGHT 5
#define BACKLIGHTONTIME 60000	// ms
// varauspumppu
#define PUMP 14

//Arduino Due käyttää näitä DMA-moodissa
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8

//-----------------
// kosketusnäytän pinnit
#define XM A2
#define XP 50
#define YM A3
#define YP 52

//Kosketusnäytän A/D-muunnosarvot
#define MIN_X 191
#define MAX_X 858
#define MIN_Y 108
#define MAX_Y 907



#endif /* SMDOVENGPIO_H_ */
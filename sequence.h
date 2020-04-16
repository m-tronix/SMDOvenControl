/*
 * sequence.h
 *
 * Created: 1.3.2015 9:36:48
 *  Author: Kremmen
 */ 


#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "FreeRTOS_ARM.h"
#include "Arduino.h"

#define MAXSTEPS 10		// maksimimäärä steppejä yhdessä juotossekvenssissä.


typedef struct {			// ohjelmastepit esitetään aika/lämpötilapareina jotka vastaa käyrän kulmapisteitä
	TickType_t timeOffset;	// aika on FreeRTOS:in kernelin tickeinä (normaalisti 1 tick = 1 ms)
	float tempC;
	} step_t;

typedef struct {		// ohjelmasekvenssin struktuuri
	char name[20];
	uint8_t stepCount;
	step_t stepList[MAXSTEPS];
} seq_t;


// Ohjelmasekvenssin luokka
// luokka voidaan ladata ja tallentaa flash-muistiin myöhempää käyttöä varten
class Sequence {		
public:
	Sequence();
	Sequence( char *sname );
	Sequence( uint16_t seqno );
	void addStep( step_t step );
	uint16_t getNumSteps() { return seq.stepCount; }
	boolean getStep( step_t *st, uint16_t index );
	boolean load( uint16_t seqno );
	void store( uint16_t seqno );
	void add() { store( Sequence::numSequences ); }
	char *getCaptionText() { return &seq.name[0]; }
	boolean startSequence( TickType_t stime );
	boolean stopSequence();
	float interpolate( TickType_t ctime );
	boolean isComplete() { return seqComplete; }
	static uint16_t numSequences;
private:
	TickType_t startTime;
	seq_t seq;
	boolean seqComplete;
};

#endif /* SEQUENCE_H_ */
/*
 * sequence.cpp
 *
 * Created: 1.3.2015 9:37:09
 *  Author: Kremmen
 */

#include "sequence.h"
#include "DueFlashStorage.h"
#include "flashconfig.h"


Sequence::Sequence() {
	seq.stepCount =0;
}

Sequence::Sequence( char *sname ) {
char *name_p;
	name_p = &seq.name[0];
	while ( (*name_p++ = *sname++) );
	seq.stepCount =0;
}

Sequence::Sequence( uint16_t seqno ){
	load(seqno);
}

void Sequence::addStep( step_t step ) {
	if ( seq.stepCount < ( MAXSTEPS-1 ) ) {
		seq.stepList[seq.stepCount++] = step;
	}
}

boolean Sequence::getStep( step_t *st, uint16_t index ) {
	if ( index < seq.stepCount ) {
		*st = seq.stepList[index];
		return true;
	}
	else return false;
}

boolean Sequence::load( uint16_t seqno ) {
byte *src;
	if (seqno < numSequences ) {
		src = dueFlashStorage.readAddress( SEQ_OFFSET + seqno * SEQ_T_SIZE );
		memcpy( &seq, src, sizeof( seq ) );
		return true;
	}
	return false;
}

void Sequence::store( uint16_t seqno ) {
uint16_t storeno;
	if ( seqno < numSequences ) storeno = seqno;
	else {
		storeno = numSequences;
		numSequences++;
	}
	dueFlashStorage.write( SEQ_OFFSET + storeno * SEQ_T_SIZE, (byte *)&seq, sizeof( seq ) );
}

boolean Sequence::startSequence( TickType_t stime) {
//Serial.println(">.StartSequence");
	if ( startTime == 0 ) {
		startTime = stime;
		seqComplete = false;
		return true;
	}
	else return false;
}

boolean Sequence::stopSequence() {
	if ( startTime ) {
		startTime = 0;
		seqComplete = true;
		return true;
	}
	else return false;
}

float Sequence::interpolate( TickType_t ctime ) {
TickType_t rtime;
uint8_t cnt;
float num, den, R;
	rtime = ctime - startTime;
	for ( cnt = seq.stepCount - 1; cnt > 0; cnt-- ) {
		if ( rtime > seq.stepList[cnt].timeOffset * configTICK_RATE_HZ ) break;
	}
	if ( cnt == seq.stepCount - 1 ) {
		seqComplete = true;
		return 0.0;
	}
	num = ( rtime - seq.stepList[cnt].timeOffset * configTICK_RATE_HZ );
	den = (( seq.stepList[cnt+1].timeOffset - seq.stepList[cnt].timeOffset ) * configTICK_RATE_HZ );
	R =  num / den;
	return seq.stepList[cnt].tempC + R * ( seq.stepList[cnt+1].tempC - seq.stepList[cnt].tempC );
}







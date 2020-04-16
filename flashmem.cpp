/*
 * flashmem.cpp
 *
 * Created: 13.4.2015 7:42:01
 *  Author: martti
 */ 

#include "flashconfig.h"
#include "sequence.h"

Config config;

DueFlashStorage dueFlashStorage;

#define MAGIC 0xAA559669	// taikaluku josta tiedet‰‰n onko flash alustettu

uint16_t Sequence::numSequences;

// Kaksi perussekvenssi‰ jotka ovat aina k‰ytett‰viss‰. Alustetan flash-muistiin mik‰li ne eiv‰t jo ole siell‰
seq_t initseq1 = {
	"Std Leaded", 7, {{0,20}, {60,150}, {180,175}, {220,220}, {230,220}, {260,100}, {270,0}, {0,0}, {0,0}, {0,0}}
};
seq_t initseq2 = {
	"Std RoHS", 7, {{0,20}, {30,150}, {120,175}, {170,240}, {180,240}, {210,110}, {220,0}, {0,0}, {0,0}, {0,0}}
};

// Uunin konfiguraation oletusarvot
#define KEYBEEPON true
#define BACKLIGHTONTIME 60000
#define KP 5.0
#define KI 20.0
#define KD 50

// Flash-muistin alustus
void initFlash() {
byte *src;
uint16_t sequences;
	// haetaan konfiguraatio flash-muistista
	src = dueFlashStorage.readAddress( (uint32_t)CONF_OFFSET );
	memcpy( &config, src, sizeof( config ) );
	// tarkistetaan onko taikaluku mukana. jos ei ole, flash on alustamaton jolloin asetetaan oletusarvot
	if ( config.magic != MAGIC ) {
		config.magic = MAGIC;
		config.keyBeepOn = KEYBEEPON;
		config.pid_P = KP;
		config.pid_I = KI;
		config.pid_D = KD;
		config.backLightOnTime = BACKLIGHTONTIME;
		dueFlashStorage.write( (uint32_t)CONF_OFFSET, (byte *)&config, sizeof( config ) );
		sequences = 2;
		dueFlashStorage.write( (uint32_t)SEQNUM_OFFSET, (byte *)&sequences, sizeof( sequences ) );
		dueFlashStorage.write( (uint32_t)SEQ_OFFSET, (byte *)&initseq1, sizeof( seq_t ) );
		dueFlashStorage.write( (uint32_t)(SEQ_OFFSET + SEQ_T_SIZE), (byte *)&initseq2, sizeof( seq_t ) );
	}
	src = dueFlashStorage.readAddress( SEQNUM_OFFSET );
	memcpy( &Sequence::numSequences, src, sizeof( Sequence::numSequences ) );
}

void Config::store() {
	dueFlashStorage.write( (uint32_t)CONF_OFFSET, (byte *)&config, sizeof( config ) );	
}


/*
 * flashconfig.h
 *
 * Created: 13.4.2015 7:38:49
 *  Author: Kremmen
 */ 


#ifndef FLASHCONFIG_H_
#define FLASHCONFIG_H_

#include "Arduino.h"
#include "DueFlashStorage.h"

// Uunin kohfiguraatio (tallennetan flash-muistiin tarvittaessa)
struct Config {
	uint32_t magic;
	boolean keyBeepOn;
	uint32_t backLightOnTime;
	float pid_P;
	float pid_I;
	float pid_D;
	void store();
};

extern Config config;

extern DueFlashStorage dueFlashStorage;

// M‰‰ritell‰‰n flash-muistin tietorakenteiden offsetit niin, ett‰ ne ovat 4 tavun rajoilla
// koska se on edellytys flashiin kirjoittamisessa
#define FLASH_START 4
#define CONF_OFFSET ( FLASH_START )
#define SEQNUM_OFFSET ( ( ( (uint32_t)CONF_OFFSET + sizeof( Config ) ) + 3 ) & 0xfffffffc )
#define SEQ_OFFSET ( ( ( (uint32_t)SEQNUM_OFFSET + sizeof( uint16_t ) ) + 3 ) & 0xfffffffc )
#define SEQ_T_SIZE ( ( sizeof( seq_t ) + 3 ) & 0xfffffffc )


void initFlash();		// Flash-muistin alustuksen tarkistus ja suoritus tarvittaessa


#endif /* FLASHCONFIG_H_ */
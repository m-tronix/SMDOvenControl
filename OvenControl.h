/*
 * OvenControl.h
 *
 * Created: 16.5.2015 14:19:12
 *  Author: martti
 */ 


#ifndef OVENCONTROL_H_
#define OVENCONTROL_H_

//Käyttisresurssien välitysstruktuuri. Tarvitaan taskeja ja niiden välistä kommunikointia perustettaessa


typedef struct {
	QueueHandle_t cQ;	// command Queue GUI -> controlTask
	QueueHandle_t qQ;	// Query queue controlTask -> GUI (status, PID params etc.)
	QueueHandle_t lQ;	// command Queue controlTask -> logger task
} resourceStruct_t;


typedef union {
	double f[3];
	int32_t i[3];
	void *p[3];
} mvalues;

// uunin ajotila
typedef enum { onIdle, onManual, onAuto } runState;

// viestit  ->controlTask (komentoja, kyselyjä)
typedef enum { sequence_Stop, sequence_Start, query_RunStatus, set_PIDsetpoint, query_PIDsetpoint, set_Kvalues, query_Kvalues } toCTmsg_t;
typedef struct {
	toCTmsg_t mtype;
	mvalues mvalue;
} toCTMessage;

// viestit  ->GUI (vastauksia)
typedef enum { rsp_RunStatus, rsp_Kvalues, rsp_Processvalues } toGUImsg_t;
typedef struct toGUIMessage {
	toGUImsg_t mtype;
	mvalues mvalue;
} toGUIMessage;

// viestit  ->logger (lokituskomentoja)
typedef enum { log_Start, log_Stop, log_Message } toLogmsg_t;
typedef struct {
	toLogmsg_t mtype;
	mvalues mvalue;
} toLogMessage;

#endif /* OVENCONTROL_H_ */
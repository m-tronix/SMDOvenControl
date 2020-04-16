/*
 * GUIEntryDialogs.h
 *
 * Created: 16.5.2015 10:22:31
 *  Author: martti
 */ 


#ifndef GUIENTRYDIALOGS
#define GUIENTRYDIALOGS

boolean strDialog(char *rtnTxt, char *capTxt, char *initTxt);
boolean intDialog( int32_t *rtnInt, char *capTxt, int32_t initInt );
boolean floatDialog( double *rtnFloat, char *capTxt, double initFloat );

boolean listDialog( uint8_t *rtnIndex, LinkedList *slist );

#endif /* GUIENTRYDIALOGS */
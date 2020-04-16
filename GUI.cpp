/*! \file GUI.cpp
	\brief LCD-kosketusnäytön ohjaustaski

	GUI.cpp
	Created: 3.3.2015 8:22:27
	Author: Kremmen
	GUITask ohjaa näyttöä lukemalla käyttäjän kosketusohjauksen ja muodostamalla yksinkrtaisen GUI-tyylisen näyttöhierarkian.
	Hierarkia on jaettu näkymiin joissa olevat widgetit on kuvattu näkymäkohtaisiin taulukoihin.
	Näkymät ovat View-luokan instansseja jotka perustavat kunkin näkymän ja vastaanottavat kosketuskoordinaatteja sekä välittävät ne
	näkymään kuuluville widgeteille käsiteltäviksi. Kukin widgetti ja haluttaessa koko näkymä on koukutettu käsittelijöihin joita
	kutsutaan jos ne on määritelty ja/tai kun havaitaan kosketuksen osuvan widgetin alueen sisään.
 */ 

#include "FreeRTOS_ARM.h"			// FreeRTOS-reaaliaikakäyttis
#include "utility/timers.h"			// FreeRTOS ajastimet
#include "utility/queue.h"

#include "ILI_SdSpi.h"				// LCD-näytön SPI-kanavan hallinta
#include "ILI9341_due_gText.h"		// gText-näyttöwidgetit
#include "ILI9341_due.h"			// perusnäyttöelementtejä


#include "cartesian.h"				// gTextin päälle rakennettu näyttöwidget-luokasto + koordinaattipisteet
#include "list.h"					// tuplasti linkitetty geneerinen lista
#include "allFonts.h"				// näytön fontit

#include "flashconfig.h"			// kestomuistin käsittely
#include "sequence.h"				// juotossekvenssin ohjaus
#include "tempmeter.h"				// lämpötilan mittaus ja säätö
#include "SMDOvenGPIO.h"			// GPIO-pinnien määrittelyjä
#include "OvenControl.h"			// taskien välisen kommunikoinnin muuttujia
#include "GUIEntryDialogs.h"		// GUI:n popup-dialogit

PGM_P introteksti[] = {
	"SMD Oven v 1.0.3",
	"Hacked by Kremmen",
	"powered by Arduino Due",
	"",
	"Last compilation",
	__DATE__,
	__TIME__,
	NULL
};


// staattinen LCD-luokan instanssi
ILI9341_due tft = ILI9341_due( TFT_CS, TFT_DC, TFT_RST );

#define BGCOLOR1 ILI9341_CYAN




QueueHandle_t cmdQueue;			// ohjaus- ja kyselysanomat säätötaskille
QueueHandle_t replyQueue;		// vastaussanomat GUItaskille

uint8_t currentProfile = 0;		// valitun profiilin numero

/*! \brief näyttöjen määrittelyt

	Kukin näyttö on esitetty taulukkona jossa on lueteltu näytössä esitettävät widgetit.
	Taulukko sisältää widgetin tyypin enumina jonka perusteella näkymän konstruktori osaa luoda oikean widgetin
	Lisäksi taulukossa on widgetin koordinaatit, mahdollinen näkyvä teksti sekä tapahtumankäsittelijöiden osoitteet
	Näkymä aktivoidaan luomalla uusi View-luokan instanssi jolle annetaan argumenttina näyttötaulukko.
*/

/*! \bried Juotossekvenssinäyttö

	Tämä näkymä on aktiivinen kun uunin varsinainen juotossekvenssi on käynnissä.
	Näkymä piirtää näytölle koordinaatiston jolla esitetään lämpötilan ohjearvoprofiili sekä ajan edetessä
	myös toteutunut lämpötilaprofiili.
	TODO:
	* käyrien päivitys määrävälein
	* käyrän piirtoalgoritmin päättäminen ja toteutus
*/

//! HOLD-painikkeen käsittelijä
/*!
	HOLD-painike pysäyttää sekvenssin etenemisen ja pitää lämpötilan ohjearvon senhetkisessä arvossaan.
	Lämpötilan säätäjä on aktiivinen ja HOLD-tila purkaantuu ja sekvenssi jatkuu joko käyttäjän painaessa
	HOLD-painiketta uudelleen tai kun tauon maksimiaika ylittyy.
	HUOM: ei vielä toteutettu!
*/
void seq_HoldButtonHandler(View *currentView, gText *hitObject, event_t ev) {
	View::exitView = true;
}

//! Sekvenssin keskeytyspainikkeen käsittelijä
/*!
	ABORT-nappi keskeyttää juotossekvenssin välittömästi, lukitsee lämpötilasäätäjän ja nollaa kaikki ohjearvot
	Uuni on tämän jälkeen neutraalitilassa ja valmis uuteen sekvenssiin
*/
void seq_AbortButtonHandler(View *currentView, gText *hitObject, event_t ev) {
	View::exitView = true;
}

//! Koordinaatiston tapahtumankäsittelijä
void seq_CartesianHandler(View *currentView, gText *hitObject, event_t ev) {
}

/*! Sekvenssinäytön widget-taulukko
	Taulukkoon on määritelty napit mutta ei itse koordinaatistoa joka luodaan erikseen näkymän alustuksessa
	X- ja Y-koordinaatit on määritelty headerissa "coordinates.h"
*/
#define SEQ_NUMVIEWELEMENTS 2
gTextParamStruct seq_ViewElements[SEQ_NUMVIEWELEMENTS] = {
/*	e_Caption, X211, Y841, X212, Y842, ILI9341_WHITE, ILI9341_BLACK, "S", NULL,		// Setpoint
	e_Caption, X221, Y841, X222, Y842, ILI9341_WHITE, ILI9341_BLACK, "A", NULL,		// Actual
	e_Caption, X311, Y851, X312, Y852, ILI9341_WHITE, ILI9341_BLACK, "P", NULL,		// P
	e_Caption, X321, Y851, X322, Y852, ILI9341_WHITE, ILI9341_BLACK, "I", NULL,		// I
	e_Caption, X331, Y851, X332, Y852, ILI9341_WHITE, ILI9341_BLACK, "D", NULL,		// D
*/
	e_Button, X211, Y441, X212, Y442, ILI9341_YELLOW, ILI9341_BLACK, "HOLD", seq_HoldButtonHandler,
	e_Button, X221, Y441, X222, Y442, ILI9341_RED, ILI9341_BLACK, "ABORT", seq_AbortButtonHandler
};

//!< Sekvenssinäytön näkymän esialustus
/*!	preInit-proseduuria kutsutaan View-luokan konstruktorissa ennenkuin näkymän widgetit luodaan taulukosta.
	Sekvenssinäkymän alustuksessa luodaan uusi koordinaatisto-instanssi ja lisätään se näkymään.
	Samalla koordinaatistolle luodaan ja kiinnitetään X- ja Y-akselit sopivasti skaalattuina.
	Koordinaatistoa ei voi luoda listalta, koska se vaatii aivan erilaiset konstruktioargumentit kuin "vakio"widgetit.
	Koordinaatiston tarkempi kuvaus tiedostoissa "cartesian.h/cpp"
*/
void seq_preInit( View *view ) {
cartesian *c;

	c = new cartesian( 0, 0, DISP_W, B_BOTTOM_Y1 - BMARGIN/2, ILI9341_WHITE, seq_CartesianHandler );
	view->add( c );
}

//!< Sekvenssinäytön näkymän jälkialustus
/*!	postInit-proseduuria kutsutaan View-luokan konstruktorissa sen jälkeen kun näkymän widgetit on luotu taulukosta.
	Asetetaan näytön taustaväri ja näytetään koordinaatisto (sitä ei näytetä automaattisesti koska ei ole widget-listalla).
 
*/
void seq_postInit( View *view ) {
cartesian *c;
Axis *ax;
Sequence seq;
step_t st;
uint8_t numSteps;
Plot *pl;

	tft.fillScreen(ILI9341_CYAN);
	c = (cartesian *)view->getItem(0);
	seq.load( currentProfile );
	numSteps = seq.getNumSteps();
	ax = new LeftAxis( ILI9341_WHITE, ILI9341_BLACK, 0.0, 300.0, 50.0 );
	c->setAxis( AXPOS_LEFT, ax );
	ax = new BottomAxis( ILI9341_WHITE, ILI9341_BLACK, 0.0, 300.0, 50.0 );
	c->setAxis( AXPOS_BOTTOM, ax );
	pl = new Plot(MAXSTEPS);
	for (uint8_t cnt = 0; cnt < numSteps; cnt++ ) {
		if ( seq.getStep( &st, cnt) ) {
			pl->addPoint( st );
		}
	}
	c->addPlot(pl);
}


// valintalista (tällä listalla on konfigurointiparametreja)
LinkedList *selectionList;


// Konfiguraatioarvon valintanäyttö
//----------------------------------------------------------
void conf_preInit( View *view ) {
	selectionList = new LinkedList();
}


void conf_postInit( View *view ) {
	tft.fillScreen(ILI9341_BLUE);
}


void conf_finalize( View *view ) {
	delete selectionList;
}

// Raporttinäyttö (toistaiseksi testikoodia!)
//----------------------------------------------------------

//apuproseduuri jolla haetaan kontrolleritaskilta PID-parametrit näytettäväksi ja muokattavaksi
boolean getPIDParams(double *Kp, double *Ki, double *Kd) {
toCTMessage query;
toGUIMessage reply;
	query.mtype = query_Kvalues;
	xQueueSendToBack( cmdQueue, &query, portMAX_DELAY );
	if ( xQueueReceive( replyQueue, &reply, portMAX_DELAY ) ) {
		*Kp = reply.mvalue.f[0];
		*Ki = reply.mvalue.f[1];
		*Kd = reply.mvalue.f[2];
	}	
	return true;
}

void putPIDParams( double Kp, double Ki, double Kd ) {
toCTMessage command;
	command.mtype = set_Kvalues;
	command.mvalue.f[0] = Kp;
	command.mvalue.f[1] = Ki;
	command.mvalue.f[2] = Kd;
	xQueueSendToBack( cmdQueue, &command, 0 );
}

void rpt_preInit( View *view ) {
}


void rpt_postInit( View *view ) {
double KpVal, KiVal, KdVal;
Caption *tmpCap;
	getPIDParams( &KpVal, &KiVal, &KdVal );
	tmpCap = (Caption *)(view->getItem( 9 ));
	tmpCap->setFloat( "%3.2f", KpVal );
	tmpCap = (Caption *)(view->getItem( 10 ));
	tmpCap->setFloat( "%3.2f", KiVal );
	tmpCap = (Caption *)(view->getItem( 11 ));
	tmpCap->setFloat( "%3.2f", KdVal );
}


void rpt_finalize( View *view ) {
}

void rpt_ManButtonHandler(View *currentView, gText *hitObject, event_t ev) {
toCTMessage msg;
toGUIMessage reply;
double setp;
Caption *cap;
	msg.mtype = set_PIDsetpoint;
	if ( floatDialog( &setp, "Temp?", 100.0) ) {
		msg.mvalue.f[0] = setp;
		xQueueSendToBack( cmdQueue, &msg, 0 );
	}
	msg.mtype = query_RunStatus;
	xQueueSendToBack( cmdQueue, &msg, 0 );
	if ( xQueueReceive( replyQueue, &reply, portMAX_DELAY ) ) {
		cap = (Caption *)currentView->getItem( 0 );
		if ( reply.mvalue.i[0] == onManual ) cap->setText("Manual");
		else cap->setText("FAILURE!");
		cap->show();
	}
}

void rpt_SeqButtonHandler(View *currentView, gText *hitObject, event_t ev) {
toCTMessage msg;
toGUIMessage reply;
double setp;
Caption *cap;
	msg.mtype = sequence_Start;
	msg.mvalue.i[0] = 0;
	xQueueSendToBack( cmdQueue, &msg, 0 );
	msg.mtype = query_RunStatus;
	xQueueSendToBack( cmdQueue, &msg, 0 );
	if ( xQueueReceive( replyQueue, &reply, portMAX_DELAY ) ) {
		cap = (Caption *)currentView->getItem( 0 );
		if ( reply.mvalue.i[0] == onAuto ) cap->setText("Automatic");
		else cap->setText("FAILURE!");
		cap->show();
	}
}

void rpt_StopButtonHandler(View *currentView, gText *hitObject, event_t ev) {
toCTMessage msg;
toGUIMessage reply;
Caption *cap;
	msg.mtype = sequence_Stop;
	xQueueSendToBack( cmdQueue, &msg, 0 );
	msg.mtype = query_RunStatus;
	xQueueSendToBack( cmdQueue, &msg, 0 );
	if ( xQueueReceive( replyQueue, &reply, portMAX_DELAY ) ) {
		cap = (Caption *)currentView->getItem( 0 );
		if ( reply.mvalue.i[0] == onIdle ) cap->setText("Stopped");
		else cap->setText("FAILURE!");
		cap->show();
	}
}

void rpt_SetKButtonHandler(View *currentView, gText *hitObject, event_t ev) {
double KpVal, KiVal, KdVal;
double *dp;
double selected_K;
Button *tmpBtn;
uint8_t captionIndex;
Caption *tmpCap;
	KpVal = KiVal = KdVal = 0.0;
	getPIDParams( &KpVal, &KiVal, &KdVal );
	tmpBtn = (Button *)hitObject;
	switch ( currentView->getItemIndex( hitObject ) ) {
		case 6: {
			dp = &KpVal;
			selected_K = KpVal;
			captionIndex = 9;
			break;
		}
		case 7: {
			dp = &KiVal;
			selected_K = KiVal;
			captionIndex = 10;
			break;
		}
		case 8: {
			dp = &KdVal;
			selected_K = KdVal;
			captionIndex = 11;
			break;
		}
	}
	if ( floatDialog( dp, tmpBtn->getText(), selected_K ) ) {
		tmpCap = (Caption *)(currentView->getItem( captionIndex ));
		tmpCap->setFloat( "%3.2f", *dp );
		tmpCap->show();
		putPIDParams( KpVal, KiVal, KdVal );
	}
}

void rpt_BackButtonHandler(View *currentView, gText *hitObject, event_t ev) {
toCTMessage msg;
	msg.mtype = sequence_Stop;
	msg.mvalue.f[0] = 0.0;
	xQueueSendToBack( cmdQueue, &msg, portMAX_DELAY );
	View::exitView = true;
}

void rpt_evHandler( View *vi, event_t ev ) {
	if ( ev.eventID == eventTimer ) {
	}
	else {
		
	}
}


#define RPT_NUMVIEWELEMENTS 13
gTextParamStruct rpt_ViewElements[RPT_NUMVIEWELEMENTS] = {
	e_Caption, X211, Y811, X222, Y812, ILI9341_GRAY, ILI9341_BLACK, "", NULL,
	e_Button, X311, Y821, X312, Y822, ILI9341_GREEN, ILI9341_BLACK, "Manual", rpt_ManButtonHandler,
	e_Button, X311, Y831, X312, Y832, ILI9341_GREEN, ILI9341_BLACK, "Seq", rpt_SeqButtonHandler,
	e_Button, X311, Y841, X312, Y842, ILI9341_GREEN, ILI9341_BLACK, "Stop", rpt_StopButtonHandler,
	e_Button, X311, Y851, X312, Y852, ILI9341_GREEN, ILI9341_BLACK, "60C", NULL,
	e_Button, X311, Y861, X312, Y862, ILI9341_GREEN, ILI9341_BLACK, "70C", NULL,
	
	e_Button, X321, Y821, X322, Y822, ILI9341_GRAY, ILI9341_BLACK, "Kp", rpt_SetKButtonHandler,
	e_Button, X321, Y831, X322, Y832, ILI9341_GRAY, ILI9341_BLACK, "Ki", rpt_SetKButtonHandler,
	e_Button, X321, Y841, X322, Y842, ILI9341_GRAY, ILI9341_BLACK, "Kd", rpt_SetKButtonHandler,

	e_Caption, X331, Y821, X332, Y822, ILI9341_WHITE, ILI9341_BLACK, "", NULL,
	e_Caption, X331, Y831, X332, Y832, ILI9341_WHITE, ILI9341_BLACK, "", NULL,
	e_Caption, X331, Y841, X332, Y842, ILI9341_WHITE, ILI9341_BLACK, "", NULL,

	e_Button, X321, Y881, X332, Y882, ILI9341_YELLOW, ILI9341_BLACK, "Back", rpt_BackButtonHandler
};
void rpt_preShow( View *view ) {
	tft.fillScreen(ILI9341_BLUE);
}

// Aloitusnäyttö
//----------------------------------------------------------
void top_ButtonHandler(View *currentView, gText *hitObject, event_t ev) {
uint8_t itm;
Sequence *seq;
LinkedList *textList;
Caption * cap;
	itm = currentView->getItemIndex( hitObject );
	switch ( itm ) {
		case 2: {	// START
			currentView = new View(SEQ_NUMVIEWELEMENTS, seq_ViewElements, seq_preInit, seq_postInit, NULL, NULL, NULL );
	currentView->show();
	delete currentView;
			break;
		}
		case 3: {	// CONFIGURE
			textList = new LinkedList();
			for ( uint8_t cnt = 0; cnt < Sequence::numSequences; cnt ++ ) {
				seq->load(cnt);
				char *txt = new char[20];
				strcpy(txt, seq->getCaptionText() );
				textList->insertLast(txt);
			}
			if ( listDialog( &currentProfile, textList ) ) {
				cap = (Caption *)currentView->getItem( 1 );
				seq->load( currentProfile );
				cap->setText( seq->getCaptionText() );
				cap->show();
			}
			break;
		}
		case 4: {	// REPORT (now TEST)
			currentView = new View(RPT_NUMVIEWELEMENTS, rpt_ViewElements, rpt_preInit, rpt_postInit, rpt_preShow, rpt_finalize, rpt_evHandler );
	currentView->show();
	delete currentView;
			break;
		}
	}
}

void top_SelectProfileHandler(View *currentView, gText *hitObject, event_t ev) {
Sequence *seq;
LinkedList *textList;
Caption * cap;

	seq = new Sequence();
	textList = new LinkedList();
	for ( uint8_t cnt = 0; cnt < Sequence::numSequences; cnt ++ ) {
		seq->load(cnt);
		char *txt = new char[20];
		strcpy(txt, seq->getCaptionText() );
		textList->insertLast(txt);
	}
	if ( listDialog( &currentProfile, textList ) ) {
		cap = (Caption *)currentView->getItem( 1 );
		seq->load( currentProfile );
		cap->setText( seq->getCaptionText() );
		cap->show();
	}
	delete seq;
	delete textList;
}

#define TOP_NUMVIEWELEMENTS 5
gTextParamStruct top_ViewElements[TOP_NUMVIEWELEMENTS] = {
	e_PGMTextArea, X211, Y211, X222, Y212, ILI9341_YELLOW, ILI9341_BLACK, (PGM_P)introteksti, NULL,
	e_Caption, X211, Y851, X222, Y852, ILI9341_WHITE, ILI9341_BLACK, "", top_SelectProfileHandler,
	e_Button, X211, Y861, X212, Y872, ILI9341_PANTHER, ILI9341_BLACK, "START", top_ButtonHandler,
	e_Button, X221, Y861, X222, Y872, ILI9341_GREEN, ILI9341_BLACK, "CONFIGURE", top_ButtonHandler,
	e_Button, X221, Y881, X222, Y882, ILI9341_BLUE, ILI9341_WHITE, "TEST", top_ButtonHandler
};

/*! \brief Haetaan aktiivisen profiilin näyttöön oletusprofiili 0 ( Std Leaded )
*/
void top_postInit( View *view ) {
Caption *cap;
Sequence *seq;
	seq = new Sequence();
	seq->load( 0 );
	cap = (Caption *)view->getItem( 1 );
	cap->setText( seq->getCaptionText() );
	delete seq;
	currentProfile = 0;
}

void top_preShow( View *view ) {
	tft.fillScreen(ILI9341_BLACK);
}


//============================================================================
// näyttöelementtien kantaluokan staattinen muuttuja
ILI9341_due *ILI9341_due_gText::_ili;
TimerHandle_t View::backLightTimer;
boolean View::exitView;

#define BACKLIGHT 5			// Näytön taustavalo
#define BEEPER 24			// Piipparin GPIO-pinni


// ohjelman yleisiä ajastimia

boolean backLight;

void LCDBackLightTimer( TimerHandle_t xTimer) {
	digitalWrite(BACKLIGHT, LOW);
	backLight = false;
}


/**************************************************************/
void GUITask( void *pvParameters ) {
resourceStruct_t *queues;
const TickType_t xBackLightOnTime = BACKLIGHTONTIME;
TimerHandle_t bLightTimer;
View *topView;

	delay(50);
	queues = (resourceStruct_t *)pvParameters;
	cmdQueue = queues->cQ;
	replyQueue = queues->qQ;
	
	// alustetaan LCD-näyttö ja kierretään kuva oikeaan asentoon
	bLightTimer = xTimerCreate( "LCD", xBackLightOnTime, pdFALSE, NULL, LCDBackLightTimer );
	digitalWrite(BACKLIGHT, HIGH);
	tft.begin();
	tft.setRotation( (iliRotation)0 );
	// käynnistetään näytön taustavalon ajastin
	xTimerStart( bLightTimer, xBackLightOnTime );
	
	//näyttöluokkien staattisten muuttujien linkitys yllä luotuihin olioihin
	ILI9341_due_gText::_ili = &tft;
	View::backLightTimer = bLightTimer;
	
	// Aloitusnäyttö
	topView = new View( TOP_NUMVIEWELEMENTS, top_ViewElements, NULL, top_postInit, top_preShow, NULL, NULL);
	topView->show();

	// tänne ei tulla koskaan vaan suoritus pyörii juurinäytön käsittelijässä ikuisesti
}

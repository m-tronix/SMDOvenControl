/*
 * GUIEntryDialogs.cpp
 *
 * Created: 16.5.2015 10:18:56
 *  Author: Kremmen
 */ 
// Numerosyöttönäyttö
//----------------------
#include "ILI9341_due_gText.h"
#include "cartesian.h"
#include "allFonts.h"
#include "SMDOvenGPIO.h"
#include "coordinates.h"

#define MAXCAPTIONLENGTH 20
#define MAXENTRYLENGTH 20

//char caption_value[MAXCAPTIONLENGTH] = "caption";
//uint8_t caption_cnt = 0;
//static char entry_value[MAXENTRYLENGTH];
//uint8_t entry_cnt = 0;
static boolean entry_valid;

void entry_digitButtonHandler(View *currentView, gText *hitObject, event_t ev) {
Button *item;
Caption *entryfield;
	item = (Button *)hitObject;
	entryfield = (Caption *)currentView->getItem(1);
//	entryfield->appendChar(item->getText()[0]);
	entryfield->appendText(item->getText());
	entryfield->show();
}

void entry_backButtonHandler(View *currentView, gText *hitObject, event_t ev) {
Caption *entryfield;
	entryfield = (Caption *)currentView->getItem(1);
	entryfield->backSpace();
	entryfield->show();
}

void entry_decimalButtonHandler(View *currentView, gText *hitObject, event_t ev) {
char *txt;
Caption *entryfield;
	entryfield = (Caption *)currentView->getItem(1);
	if ( entryfield->findChar('.') ) {
		currentView->keyErrorBeep();		
	}
	else {
		entryfield->appendChar('.');
		entryfield->show();
	}
}
void entry_cancelButtonHandler(View *currentView, gText *hitObject, event_t ev) {
	View::exitView = true;
	entry_valid = false;
}
void entry_okButtonHandler(View *currentView, gText *hitObject, event_t ev) {
	View::exitView = true;
	entry_valid = true;
}



// näyttöelementit
#define ENTRY_NUMVIEWELEMENTS 16
gTextParamStruct entry_ViewElements[ENTRY_NUMVIEWELEMENTS] = {
	e_Caption, X211, Y811, X222, Y812, ILI9341_YELLOW, ILI9341_BLACK, "", NULL,
	e_Caption, X211, Y821, X222, Y822, ILI9341_WHITE, ILI9341_BLACK, "", NULL,
// *** rivijärjestys säilytettävä *** "0" oltava listan rivillä! Muuten GUI menee sekaisin
	e_Button, X321, Y871, X322, Y872, ILI9341_BLUE, ILI9341_YELLOW, "0", entry_digitButtonHandler,
	e_Button, X311, Y861, X312, Y862, ILI9341_BLUE, ILI9341_YELLOW, "1", entry_digitButtonHandler,
	e_Button, X321, Y861, X322, Y862, ILI9341_BLUE, ILI9341_YELLOW, "2", entry_digitButtonHandler,
	e_Button, X331, Y861, X332, Y862, ILI9341_BLUE, ILI9341_YELLOW, "3", entry_digitButtonHandler,
	e_Button, X311, Y851, X312, Y852, ILI9341_BLUE, ILI9341_YELLOW, "4", entry_digitButtonHandler,
	e_Button, X321, Y851, X322, Y852, ILI9341_BLUE, ILI9341_YELLOW, "5", entry_digitButtonHandler,
	e_Button, X331, Y851, X332, Y852, ILI9341_BLUE, ILI9341_YELLOW, "6", entry_digitButtonHandler,
	e_Button, X311, Y841, X312, Y842, ILI9341_BLUE, ILI9341_YELLOW, "7", entry_digitButtonHandler,
	e_Button, X321, Y841, X322, Y842, ILI9341_BLUE, ILI9341_YELLOW, "8", entry_digitButtonHandler,
	e_Button, X331, Y841, X332, Y842, ILI9341_BLUE, ILI9341_YELLOW, "9", entry_digitButtonHandler,
	e_Button, X331, Y871, X332, Y872, ILI9341_BLUE, ILI9341_YELLOW, ".", entry_decimalButtonHandler,
// *** tähän asti ***	
	e_Button, X311, Y871, X312, Y872, ILI9341_CYAN, ILI9341_BLACK, "back", entry_backButtonHandler,
	e_Button, X211, Y881, X212, Y882, ILI9341_RED, ILI9341_YELLOW, "CANCEL", entry_cancelButtonHandler,
	e_Button, X221, Y881, X222, Y882, ILI9341_GREEN, ILI9341_BLACK, "OK", entry_okButtonHandler
};


void entry_preInit (View *view) {
	ILI9341_due_gText::_ili->fillScreen(ILI9341_BLACK);
}

void entry_postInit (View *view) {
//gText *item;
Caption *cap;
/*	for (uint8_t cnt = 1; cnt < 13; cnt++ ) {
		item = (gText *)view->getItem(cnt);
		item->selectFont(fixednums8x16); // fontti asetetaan itemeille 1 - 12 = entry, numerot ja desimaalipilkku
	}
*/
	cap = (Caption *)view->getItem(1);
	cap->alignText(gTextAlignMiddleRight);
}

void entry_eventHandler (View *view, event_t event) {
}

//Yksinkertainen popup-dialogi jolla napataan lukuarvo ja palautetaan se

boolean strDialog(char **rtnTxt, char *capTxt, char *initTxt) {
View popup(ENTRY_NUMVIEWELEMENTS, entry_ViewElements, entry_preInit, entry_postInit, NULL, NULL, NULL );
Caption *cap;
	cap = (Caption *)popup.getItem(0);
	cap->setText( capTxt );
	cap = (Caption *)popup.getItem(1);
	cap->setText( initTxt );
	popup.show();
	if (entry_valid) {
		*rtnTxt = cap->getText();
	}
	else {
		*rtnTxt = NULL;
	}
	return entry_valid;
}

boolean intDialog( int32_t *rtnInt, char *capTxt, int32_t initInt ) {
char initTxt[20];
char *rtnTxt;
	sprintf( initTxt, "%i", initInt );
	if ( strDialog( &rtnTxt, capTxt, initTxt ) ) {
		*rtnInt = atoi( rtnTxt );
		return true;
	}
	else return false;
}

boolean floatDialog( double *rtnFloat, char *capTxt, double initFloat ) {
char initTxt[20];
char *rtnTxt;
	sprintf( initTxt, "%3.2f", initFloat );
	if ( strDialog( &rtnTxt, capTxt, initTxt ) ) {
		*rtnFloat = atof( rtnTxt );
		return true;
	}
	else return false;
}


/*! \brief Listaitemin valintanäyttö

	Tällä näytöllä valitaan itemi käyttäjälle näytettävältä listalta
	Näytöllä on 5 riviä Captioneita joilla näytetään kulloisenkin listan itemejä. Itemi valitaan sen listaindeksin perusteella. Kosketus listan riville fokusoi k.o. itemin.
	UP ja DOWN-Buttonit joilla em. listaa voi vierittää jos profiileja on enemmän kuin näyttöön mahtuu.
	OK ja Cancel-buttonit jotka tekee juuri mitä kuvittelisikin. OK buttonin painallus päivittää valitun listaindeksin jatkokäsittelyä varten.
*/
#define BGCOLOR3 ILI9341_PANTHER

LinkedList *list;			//!< osoitin listaan jolla valittavat elementit (stringit)
gText *hit;					//!< apuosoitin klikattuun listariviin
uint8_t focusedItem;		//!< listalla valittuna oleva itemi
uint16_t firstItemIndex;	//!< listalla ensimmäisenä näkyvän itemin indeksi (koko listasta)
boolean scrollDownEnabled = false;
boolean scrollUpEnabled = false; 

//! Listaitemin valintakäsittelijä
/*!
	Tämä tapahtumankäsittelijä asettaa apumuuttujan "hit" osoittamaan viimeksi klikattuun listariviin.
	Kun/jos näkymä kuitataan OK:ksi niin apumuuttujan avulla voidaan käydä lukemassa mitä rivillä sanottiin.
*/
void list_SelectHandler( View *currentView, gText *hitObject, event_t ev ) {
	hit = hitObject; 
}

//! Listan vieritys alas/eteenpäin
/*
	Jos alas vieritys on sallittu (itemejä on vielä listalla) niin listalle haetaan seuraava setti itemejä ja mahdollisesti
	vajaaksi jäävä osa tyhjennetään.
	TODO: toimintoa ei ole vielä tehty.
*/
void list_DownButtonHandler( View *currentView, gText *hitObject, event_t ev ) {
	if ( scrollDownEnabled ) {
		
	}
}

//! Listan vieritys ylös/taaksepäin
/*
	Jos ylös vieritys on sallittu (itemejä on listalla alussapäin) niin listalle haetaan edellinen setti itemejä.
	TODO: toimintoa ei ole vielä tehty.
*/
void list_UpButtonHandler( View *currentView, gText *hitObject, event_t ev ) {
	
}

//! Näkymän kuittaus
/*
	Tällä lähdetään ulos näkymästä. 
	Tunnistetaan oliko painettu OK vai Cancel-nappia ja toimitaan sen mukaan.
*/
void list_OKCancelButtonHandler(View *currentView, gText *hitObject, event_t ev) {
	if ( hitObject == currentView->getItem(8) ) {	// OK?
		entry_valid = true;
	} else {
		entry_valid = false;
	}
	View::exitView = true;
}


#define LIST_NUMVIEWELEMENTS 9
gTextParamStruct list_ViewElements[LIST_NUMVIEWELEMENTS] = {
	e_Caption, X311, Y811, X332, Y812, ILI9341_GRAY, ILI9341_BLACK, "", list_SelectHandler,
	e_Caption, X311, Y821, X332, Y822, ILI9341_GRAY, ILI9341_BLACK, "", list_SelectHandler,
	e_Caption, X311, Y831, X332, Y832, ILI9341_GRAY, ILI9341_BLACK, "", list_SelectHandler,
	e_Caption, X311, Y841, X332, Y842, ILI9341_GRAY, ILI9341_BLACK, "", list_SelectHandler,
	e_Caption, X311, Y851, X332, Y852, ILI9341_GRAY, ILI9341_BLACK, "", list_SelectHandler,
// --> aseta koordinaatit kohdalleen!
	e_Button, X211, Y871, X212, Y872, ILI9341_GRAY, ILI9341_BLACK, "Next", list_DownButtonHandler,
	e_Button, X221, Y871, X222, Y872, ILI9341_GRAY, ILI9341_BLACK, "Prev", list_UpButtonHandler,
// <--
	e_Button, X211, Y881, X212, Y882, ILI9341_RED, ILI9341_BLACK, "Cancel", list_OKCancelButtonHandler,
	e_Button, X221, Y881, X222, Y882, ILI9341_GREEN, ILI9341_BLACK, "OK", list_OKCancelButtonHandler
};


/*! \brief Profiilin valintanäyttö

	Näkymä hyödyntää listaitemin valintanäyttöä populoimalla listalle kestomuistiin tallennettujen juotosprofiilien nimet.
	Kaksi ensimmäistä profiilia ovat kiinteitä "StdLead" ja "StdRoHS", ja pyrkivät suunnilleen mallintamaan teollisuudessa käytetyt perusprofiilit.
	
*/


//!< Valintalistan jälkialustus
/*!	Populoidaan näyttölista
	Jos muistissa on enemmän kuin listalle mahtuu, enabloidaan skrollaus ja sitten fokusoidaan ekaan itemiin
*/
void list_postInit( View *view ) {
Caption *cap;
char *txt;
	txt = (char *)list->getFirst();
	for ( uint8_t cnt = 0; (txt != NULL) && ( cnt < 5 ); cnt++ ) {
		cap = (Caption *)view->getItem(cnt);
		cap->appendText( txt );
		txt = (char *)list->getNext();
	}
	cap =  (Caption *)view->getItem(0);
	cap->setFontColor( ILI9341_BLACK, ILI9341_YELLOW );
	focusedItem = 0;
	firstItemIndex = 0;
	// jos sekvenssejä on muistissa yli 5 kpl, enabloidaan alas skrollausnappi
	if ( list->length() > 5 ) {
		cap = (Caption *)view->getItem(5);
		cap->setFontColor( ILI9341_BLACK, ILI9341_YELLOW );
		scrollDownEnabled = true;		
	}
	ILI9341_due_gText::_ili->fillScreen(ILI9341_BLACK);
}


//!< Sekvenssinäytön näkymän käsittelijä
/*!	Käsittelijä käy listan läpi ja vaihtaa fokusvärin näpättyyn listaitemiin sekä vaihtaa muut ei-fokusväriin.
*/
void list_evHandler( View *vi, event_t ev ) {
Caption *cap;
	for ( uint8_t cnt = 0; cnt < 5 ; cnt++ ) {
		cap = (Caption *)vi->getItem(cnt);
		if ( ( cap == hit ) && ( cap->getText() != NULL ) ) {
			cap->setFontColor( ILI9341_BLACK, ILI9341_YELLOW );
			focusedItem = firstItemIndex + cnt;
		}
		else cap->setFontColor( ILI9341_BLACK, ILI9341_GRAY );
		cap->show();
	}
}


boolean listDialog( uint8_t *rtnIndex, LinkedList *slist ) {
View *popup;
void *citem;
	list = slist;
	popup = new View(LIST_NUMVIEWELEMENTS, list_ViewElements, NULL, list_postInit, NULL, NULL, list_evHandler );
	popup->show();
	if ( entry_valid ) {
		*rtnIndex = focusedItem;
		delete popup;
		return true;
	}
	delete popup;
	return false;
}


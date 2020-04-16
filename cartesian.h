/*
 * cartesian.h
 *
 * Created: 26.1.2015 11:39:33
 *  Author: Kremmen
 
 * A small class for rendering cartesian x-y coordinate backgrounds and plotting arbitrary diagrams on same.
 * originally designed to display the temperature sequences of Hacklab's SMD soldering oven mod.
 
 */ 


#ifndef CARTESIAN_H_
#define CARTESIAN_H_

#include "FreeRTOS_ARM.h"
#include "utility/timers.h"
#include "ILI9341_due.h"
#include "ILI9341_due_gText.h"
#include "allFonts.h"
#include "list.h"
#include "sequence.h"
#include "coordinates.h"



#define MAJORTICKWIDTH 5

#define MAXPLOTS 20
#define CAPTIONLENGTH 20
#define PLOTAREA_TOPMARGIN 10
#define PLOTAREA_RIGHTMARGIN 10

// näytänohjauksen tapahtumarakenteet

enum eventID_t {eventNone, eventTouch, eventTimer};	//!< GUI-tapahtuman tyyppi

typedef struct {
	eventID_t eventID;
	union {
		uint32_t time;
		struct {
			uint16_t x;
			uint16_t y;
		} pos;
		uint8_t byte[4];
	} event_data;
} event_t;											//!< struktuuri jossa tiedot GUI-tapahtumasta

class gText;
class View;
typedef void (*gEvHandler)(View *vi, gText *hitObject, event_t ev);	//!< GUI-tapahtumankäsittelijä
typedef void (*gPrepare)( View *vi );								//!< GUI-näkymän alustus
typedef void (*gViewHandler)( View *vi, event_t ev );				//!< GUI-näkymän käsittelijä

enum elemtype {e_PGMTextArea, e_TextArea, e_Caption, e_Button, e_cartesian};	//!< eri GUI-widgettien tyypit kosruktiofactorya varten

//!< GUI-näkymän widgetlistan tyypitys
typedef struct {
	elemtype etype;
	int16_t x1,y1,x2,y2;
	uint16_t color_bg, color_fg;
	const char * txt;
	gEvHandler hnd;
} gTextParamStruct;

/*!	\brief GUI-näkymäluokka joka luo ja ohjaa näkymiä

	Jokainen käyttöliittymän näyttö toteutetaan omana View-luokan instanssina.
	View ylläpitää listaa näytöllä näkyvista widgeteistä, jotka luodaan staattisen widgetlistan avulla
	Uuden näkymän alustuksessa voidaan lisäksi luoda spesiaali-widgettejä ja liittää niitä näkymään tarpeen mukaan.
	Näkymä sieppaa käyttäjän inputin kosketuskalvolta ja kutsuu koordinaattien mukaista käsittelijää.
	Lisäksi näkymä voi tarvittaessa käsitellä tapahtumat näkymätasolla vielä erikseen.
*/
class View {
	public:
	static TimerHandle_t backLightTimer;	//!< Näytön taustavalon ajastin (luokkamuuttuja)
	static boolean exitView;				//!< Näkymän sulkulippu (luokkamuuttuja): Kun true, näkymä tuhotaan ja palataan edelliseen
	View();
	View(uint8_t elems, gTextParamStruct elemparams[], gPrepare preinit, gPrepare postinit, gPrepare preshow, gPrepare final, gViewHandler vh); //!< Factory-konstruktori. Luo widgetlistan mukaiset elemntit ja koukuttaa käsittelijät
	~View();
	void add( void *element );				//!< Näyttöwidgetin lisäys näkymään
	void show();							//!< Näkymän päivitys näytölle
	int8_t getItemIndex( void *item );		//!< Osoitetun widgetin listaindeksin nouto
	void *getItem( uint8_t index );			//!< Osoitin indeksin mukaiseen widgettiin
	void keyErrorBeep();					//!< virhepiippaus väärästä näpäyksestä
	private:
	static boolean screenIsDirty;			//!< näytön päivityslippu
	LinkedList elementList;					//!< Näkymään kuuluvien widgettien lista
	uint16_t viewBgColor;					//!< Näkymän taustaväri
	gPrepare preInit, postInit, preShow, finalize;	//!< Osoittimet alustus- ja lopetusproseduureihin
	gViewHandler viewHandler;				//!< Osoitin näkymän näkymäkohtaiseen käsittelijään
	void render();							//!< Näkymän renderöinti LCD-näytön pintaan
	void handleView();						//!< Näkymän oletuskäsittelijä
};

/*!	\brief Näyttöwidgetien sovitusluokka

	gText liittää 3. osapuolen kirjoittaman ILI9341_due_gText-luokan cartesian-kirjaston näkymänkäsittelyyn.
*/
class gText: public ILI9341_due_gText {
public:
	gText(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, gEvHandler handler);	//!< Konstruktori joka liittää mukaan tapahtumankäsittelijän osoitteen
	void handleEvent( View *vi, event_t ev );	//!< Widgetin tapahtumankäsittelijä
	virtual void show();						//!< Widgetin päivitys näytölle
protected:
	gEvHandler gTextEventHandler;				//!< Widgetkohtainen koukku tapahtumankäsittelijään
	

};

/*!	\brief Tekstikenttä jonka teksti flash-muistissa

	Teksti on flash-muistiin talletettu C-tyylinen string array joka terminoituu \0\0 eli viimeinen string on tyhjä.
	Käytetään lähinnä pitkien vakiotekstien näyttöön ruudulla.
*/
class PGM_TextArea: public gText{
public:
	PGM_TextArea(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t but_color, uint16_t txt_color, PGM_P txt[], gEvHandler handler );
	void show();
	char *getText() { return NULL; }	// tämä ei toimi kuten muut - jätetään sivuun
private:
	PGM_P *text;
};

/*!	\brief Tekstikenttä jonka teksti on RAM-muistissa

	Teksti on RAM-muistiin talletettu C-tyylinen string array joka terminoituu \0\0 eli viimeinen string on tyhjä.
	Käytetään pitkien muutuvien tekstien esittämiseen.
*/
class TextArea: public gText{
	public:
	TextArea(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t but_color, uint16_t txt_color, char *txt[], gEvHandler handler );
	void show();
	char *getText() { return *text; }
	private:
	char **text;
};

/*!	\brief Otsikkokenttä vakio- tai muuttuvalla tekstillä

	Käytetään sekä staattisena otsikkona että syöttökenttänä luettaessa käyttäjän inputtia sisään.
	Tyypillinen käyttötapa on listata Captionit näkymäkohtaisiin widget-listoihin.
	View-luokan instanssi luo näkymän listan mukaan.
*/
class Caption: public gText{
public:
	Caption( int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t but_color, uint16_t txt_color, const char *txt, gEvHandler handler );
	void show();
	void appendText( char *txt );
	void appendChar( char ch );
	void setFloat( char *fmt, double num );
	void setInteger( int32_t num );
	void backSpace();
	void clear();
	void setText( char *txt ) { clear(); appendText( txt ); }
	char *getText() { return text; }
	void alignText(gTextAlign align) { textAlignment = align; }
	char *findChar( char ch );
private:
	char text[CAPTIONLENGTH];
	gTextAlign textAlignment;
};

/*!	\brief Painikenappi toimintokäskyjen vastaanottamiseksi käyttäjältä

	Button suorittaa käyttäjän toimenpiteitä siihen liitetyn tapahtumankäsittelijän avulla.
	Sovelluskohtainen tapahtumankäsittelijä aktivoituu joka kerta kun nappia kosketetaan.
	Tyypillinen käyttötapa on listata Buttonit näkymäkohtaisiin widget-listoihin ja indikoida käsittelijät kyseisessä listassa.
	View-luokan instanssi luo näkymän listan mukaan ja linkkaa käsittelijät Buttoniin ajon aikana.
*/
class Button:public gText{
public:
	Button( int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t but_color, uint16_t txt_color, const char *cap, gEvHandler handler);
	void show();
	char *getText() { return caption; }
	
private:
	char caption[CAPTIONLENGTH];
};
	
/*!	\brief Karteesisen koordinaatiston koordinaattiakselin kantaluokka

	Tästä kantaluokasta johdetaan akselit X- ja Y-suuntaan erikseen
*/
class Axis: public ILI9341_due_gText {
public:
	Axis( uint16_t but_color, uint16_t txt_color, float sMin, float sMax, float sTick);
	virtual void show();
	int16_t getWidth() { return _area.x2 - _area.x1; }
	int16_t getHeight() { return _area.y2 - _area.y1; }
	float getMin() { return scaleMin; }
	float getMax() { return scaleMax; }
	int16_t getTextWidth();
	int16_t getTextHeight() { return fontHeight(); }
protected:
	float scaleMin;
	float scaleMax;
	float scaleTick;
	boolean ticksOn;
	boolean gridOn;
};

/*!	\brief Karteesisen koordinaatiston vasen (Y-) akseli

	Kantaluokasta johdettu akseli jonka kordinaatit on sovitettu koordinatistowidgetin vasempaan laitaan.
	Luokan päivitys renderöi akseliviivan ja siihen asteikon.
*/
class LeftAxis: public Axis {
public:
	LeftAxis( uint16_t but_color, uint16_t txt_color, float sMin, float sMax, float sTick);
	void show();
};

/*!	\brief Karteesisen koordinaatiston ala (X-) akseli

	Kantaluokasta johdettu akseli jonka kordinaatit on sovitettu koordinatistowidgetin alalaitaan.
	Luokan päivitys renderöi akseliviivan ja siihen asteikon.

*/
class BottomAxis: public Axis {
public:
	BottomAxis( uint16_t but_color, uint16_t txt_color, float sMin, float sMax, float sTick);
	void show();
};

/*!	\brief Karteesisen koordinaatiston oikea (Y-) akseli

	Kantaluokasta johdettu akseli jonka kordinaatit on sovitettu koordinatistowidgetin oikeaan laitaan.
	Luokan päivitys renderöi akseliviivan ja siihen asteikon
*/
class RightAxis: public Axis {
	RightAxis( uint16_t but_color, uint16_t txt_color, float sMin, float sMax, float sTick);
	public:
	void show();
};

/*!	\brief Karteesisen koordinaatiston datan kuvaajaluokka

	Datasetti muodostaa koordinaatistossa näytettävän kuvaajan.
	\todo	Kirjoita luokan sisältö!
*/
class cartesian;
class Plot {
public:
	Plot(uint16_t numDataPoints);
	~Plot();
	boolean addPoint( step_t pt );
	boolean getPoint( step_t *pt, uint16_t index );
	void show( cartesian *c );
private:
	uint16_t maxPoints = 0;
	uint16_t insertIndex = 0;
	step_t *point;
};

enum axisPosition {AXPOS_LEFT, AXPOS_BOTTOM, AXPOS_RIGHT };
	
/*!	\brief Karteesinen koordinaatisto

	Luokka luo X-Y -koordinaatiston ja liittää siihen koordinaattiakselit sekä yhden tai useampia kuvaajia 
	Kuvaajat on assosioitu koordinaatiston akseleihin siten, että niitä vastaavat käyrät piirretään akselistojen
	skaalauksen mukaisesti.
	\todo	varmista, että koordinaattiakselit on linkattu ennen renderöintiä!
*/
class cartesian: public gText {
	friend class plot;
public:
	cartesian( uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color, gEvHandler handler );	//!< Luo koordinaatistolle widgetin ja linkkaa tapahtumankäsittelijän
	void setAxis( axisPosition pos, Axis *ax );					//!< Linkkaa koordinaatistoon uuden akselin. Uusi korvaa mahdollisen edellisen
	void addPlot( Plot *pl );									//!< Linkkaa koordinatistoon uuden kuvaajan. Uusi kuvaaja lisätään jo olevien lisäksi
	void show();												//!< koko koordinaatiston päivitys näyttöön
	void drawLine(float x0, float y0, float x1, float y1);		//!< Skaalatun viivan piirto koordinatistoon
private:
	int16_t getPXPos(uint8_t index, float val);					//!< Palauttaa pikseliposition reaalikoordinatista
	uint16_t p_X1, p_Y1, p_X2, p_Y2; // kuvaajien piirtoala		//!< koordinaatiston ala
	LinkedList plotList;										//!< Kuvaajien lista
	LeftAxis *leftAxis;											//!< osoitin vasempaan (Y-) akseliin
	BottomAxis *bottomAxis;										//!< osoitin alempaan (X-) akseliin
	RightAxis *rightAxis;										//!< osoitin oikeaan (Y-) akseliin
};

#endif /* CARTESIAN_H_ */

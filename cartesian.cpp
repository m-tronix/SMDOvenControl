/*
 * cartesian.cpp
 *
 * Created: 26.1.2015 11:39:48
 *  Author: Kremmen
 */
#include "cartesian.h"
#include "Arduino.h"
#include "SMDOvenGPIO.h"

gText::gText(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, gEvHandler handler):ILI9341_due_gText(X1,Y1,X2,Y2){
	gTextEventHandler = handler;
}

void gText::show() {
	;
}

void gText::handleEvent( View *vi, event_t ev ) {
	if ( gTextEventHandler ) gTextEventHandler( vi, this, ev ); 
}


PGM_TextArea::PGM_TextArea(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t but_color, uint16_t txt_color, PGM_P txt[], gEvHandler handler):gText(X1,Y1,X2,Y2,handler) {
	text = txt;
	_fontBgColor = but_color;
	_fontColor = txt_color;
	_fontMode = gTextFontMode_Solid;
}


void PGM_TextArea::show() {
uint16_t cnt,t_X,t_Y,fH;
	clearArea(_fontBgColor);
	selectFont( calibri );
	fH = fontHeight();
	setFontColor( _fontColor, _fontBgColor);
	cnt = 0;
	t_X = fH/2; t_Y = fH/2;
	while (text[cnt]) {
		drawString_P( text[cnt], t_X, t_Y);
		t_Y += fH;
		cnt++;	
	}
}

TextArea::TextArea(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t but_color, uint16_t txt_color, char *txt[], gEvHandler handler):gText(X1,Y1,X2,Y2,handler) {
	text = txt;
	_fontBgColor = but_color;
	_fontColor = txt_color;
	_fontMode = gTextFontMode_Solid;
}


void TextArea::show() {
uint16_t cnt,t_X,t_Y,fH;
	clearArea(_fontBgColor);
	selectFont( calibri );
	fH = fontHeight();
	setFontColor( _fontColor, _fontBgColor);
	cnt = 0;
	t_X = fH/2; t_Y = fH/2;
	while (text[cnt]) {
		drawString( text[cnt], t_X, t_Y);
		t_Y += fH;
		cnt++;
	}
}
Caption::Caption( int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t but_color, uint16_t txt_color, const char *txt, gEvHandler handler ):gText(X1,Y1,X2,Y2,handler) {
char *textp;
uint8_t cnt;
	_fontBgColor = but_color;
	_fontColor = txt_color;
	_fontMode = gTextFontMode_Solid;
	textAlignment = gTextAlignMiddleCenter;
	textp = &text[0];
	for ( cnt = 0; cnt < CAPTIONLENGTH; cnt++ ) {
		if ( !( *textp++ = *txt++ ) ) break;
	}
	selectFont( calibri );
	setFontColor( _fontColor, _fontBgColor);
}

void Caption::show() {
	clearArea(_fontBgColor);
	_ili->drawRect(_area.x1, _area.y1, _area.x2-_area.x1+1, _area.y2-_area.y1+1, ILI9341_BLACK);
	drawString( text, textAlignment);
}

void Caption::appendText( char *txt ) {
	strncat(text, txt, CAPTIONLENGTH);
}

void Caption::appendChar( char ch ) {
	char *textp;
	uint8_t cnt =0;
	textp = &text[0];
	while ( *textp ) {
		cnt++;
		textp++;
	}
	if ( cnt < CAPTIONLENGTH-1 ) {
		*textp++ = ch;
		*textp = 0x00;
	}
}

void Caption::setFloat( char *fmt, double num ) {
	sprintf( text, fmt, num );
	textAlignment = gTextAlignMiddleRight;
}

void Caption::setInteger( int32_t num ) {
	sprintf( text, "%i", num );
	textAlignment = gTextAlignMiddleRight;
}

char *Caption::findChar( char ch ) {
	return strchr( text, ch );
}

void Caption::backSpace() {
	char *textp;
	uint8_t cnt =0;
	textp = &text[0];
	while ( *textp ) {
		cnt++;
		textp++;
	}
	if ( cnt > 0 ) {
		textp--;
		*textp = 0x00;
	}
}

void Caption::clear() {
	for ( uint8_t cnt = 0; cnt < CAPTIONLENGTH; cnt++ ) text[cnt] = 0x00;	
}

/*! \brief Button-luokan peruskonstruktori

	Konstruktori ottaa argumentteina widgetin koordinaatit, esitett‰v‰n tekstin v‰rit ja stringin sek‰ osoittimen tapahtumank‰sittelij‰‰n
	\param X1, Y1, X2, Y2 koordinaatit
	\param but_color napin pohjav‰ri
	\param txt_color tekstin v‰ri
	\param cap otsikkoteksti
	\param gEvHandler osoitin tapahtumank‰sittelij‰‰n
*/
Button::Button( int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t but_color, uint16_t txt_color, const char *cap, gEvHandler handler):gText(X1,Y1,X2,Y2,handler) {
	strcpy(caption,cap);
	selectFont( calibri );
	_fontBgColor = but_color;
	_fontColor = txt_color;
	_fontMode = gTextFontMode_Solid;
	setFontColor( _fontColor, _fontBgColor);
}

/*! \brief Button-luokan p‰ivitys n‰kym‰‰n

	Metodi siivoaa Buttonin koordinaatiien alan taustav‰riseksi, piirt‰‰ yksinkertaisen "varjostetun" laatikon
	sek‰ tulostaa Buttonin tekstin laatikkoon alustetulla v‰rill‰
*/
void Button::show() {
	clearArea(_fontBgColor);
	_ili->drawFastHLine(_area.x1, _area.y1, _area.x2-_area.x1, ILI9341_WHITE);
	_ili->drawFastHLine(_area.x1, _area.y2, _area.x2-_area.x1, ILI9341_BLACK);
	_ili->drawFastVLine(_area.x1, _area.y1, _area.y2-_area.y1, ILI9341_WHITE);
	_ili->drawFastVLine(_area.x2, _area.y1, _area.y2-_area.y1, ILI9341_BLACK);
	drawString( caption, gTextAlignMiddleCenter);
}

Plot::Plot(uint16_t numDataPoints) {
	point = (step_t *)malloc( numDataPoints * sizeof( step_t) );
	if ( point ) { 
		maxPoints = numDataPoints;
	}
	else maxPoints = 0;
	insertIndex = 0;
}

Plot::~Plot() {
	free( point );
}

boolean Plot::addPoint( step_t pt ) {
	if ( insertIndex < maxPoints-1 ) {
		point[insertIndex++] = pt;
		return true;
	}
	else return false;
}

boolean Plot::getPoint( step_t *pt, uint16_t index ) {
	if ( index < insertIndex ) {
		*pt = point[index];
		return true;
	}
	else return false;
}

void Plot::show(cartesian *c ) {
uint16_t cnt;
	if ( insertIndex > 0 ) {
		for (cnt = 0; cnt < insertIndex-1; cnt ++ ) {
			c->drawLine( point[cnt].timeOffset,point[cnt].tempC,point[cnt+1].timeOffset,point[cnt+1].tempC );
		}
	}
}


/*! \brief Axis-kantaluokan peruskonstruktori

	Konstruktori ottaa argumentteina esitett‰v‰n tekstin v‰rit ja stringin sek‰ osoittimen tapahtumank‰sittelij‰‰n.
	Huom: kantaluokan konstruktoria saa kutsua vain johdetun luokan konstruktorista koska suoraan kutsuminen ei ole mielek‰st‰.
	\param but_color "napin" pohjav‰ri joka m‰‰r‰‰ koordinaatiston taustav‰rin
	\param txt_color tekstin v‰ri
	\param Min, Max koordinaatin raja-arvot skaalattuina mittasignaalin arvoalueeseen
	\param sTick Akselin rasterimerkintˆjen v‰li skaalattuna mittasignaaliin
*/
Axis::Axis( uint16_t but_color, uint16_t txt_color, float sMin, float sMax, float sTick ):ILI9341_due_gText(0,0,1,1) {
	_font = calibri;
	_fontBgColor = but_color;
	_fontColor = txt_color;
	scaleMin = sMin;
	scaleMax = sMax;
	scaleTick = sTick;
	ticksOn = true;
	gridOn = true;
}

/*! \brief Axis-kantaluokan tekstilabelin leveys pikselein‰

	Funktio palauttaa akselin suurinta skaalalukemaa vastaavan stringin leveyden pikselein‰.
	K‰ytet‰‰n kohdistettaessa akseleita koordinaatiston n‰yttˆalaan. 
*/
int16_t Axis::getTextWidth() {
char s[10];
char *sptr;
	sptr = &s[0];
	itoa( ( int16_t )scaleMax, sptr, 10);
	return stringWidth( sptr );
}

/*! \brief Axis-kantaluokan p‰ivitys n‰yttˆˆn

	Metodi siivoaa akselin piirtoalan t‰ytt‰m‰ll‰ sen taustav‰rill‰.
	Metodia kutsutaan johdetun akseliluokan n‰ytˆnp‰ivityksest‰. 
*/
void Axis::show() {
	_ili->fillRect(_area.x1,_area.y1,_area.x2-_area.x1,_area.y2-_area.y1,_fontBgColor);
}

/*! \brief Vasemman laidan akseliluokan konstruktori

	Konstruktori ottaa argumentteina esitett‰v‰n tekstin v‰rit ja stringin sek‰ osoittimen tapahtumank‰sittelij‰‰n.
	Konstruktorina k‰ytet‰‰n kantaluokan konstruktoria suoraan.
	\param but_color "napin" pohjav‰ri joka m‰‰r‰‰ koordinaatiston taustav‰rin
	\param txt_color tekstin v‰ri
	\param Min, Max koordinaatin raja-arvot skaalattuina mittasignaalin arvoalueeseen
	\param sTick Akselin rasterimerkintˆjen v‰li skaalattuna mittasignaaliin
*/
LeftAxis::LeftAxis( uint16_t but_color, uint16_t txt_color, float sMin, float sMax, float sTick ):Axis(but_color,txt_color,sMin,sMax,sTick) {
}

/*! \brief Vasemman laidan akselin p‰ivitys n‰yttˆˆn

	P‰ivitys piirt‰‰ akselin koordinatiston vasempaan laitaan.
	\todo	asemoi akseliviiva niin ett‰ skaalanumeron leveys huomioidaan
*/
void LeftAxis::show() {
float tickValue, tickValToPX;
int32_t tickIndex;
uint16_t t_X, t_Y;
	Axis::show();
	_ili->drawFastVLine( _area.x2, _y ,_area.y2-_area.y1, _fontColor );
	tickValToPX = ( _area.y2-_area.y1 ) / ( scaleMax - scaleMin );	// muunnoskerroin pikseli‰ / yksikkˆ jos pystyakseli
	tickIndex = ( uint32_t ) ( ( scaleMin / scaleTick ) + 1 );
	tickValue = tickIndex * scaleTick;
	while ( tickValue <= scaleMax ) {
		t_X = _area.x2 - MAJORTICKWIDTH;
		t_Y = _area.y2 - ( uint16_t ) ( tickValToPX * ( tickValue - scaleMin ) );
		_ili->drawFastHLine( t_X, t_Y, MAJORTICKWIDTH, _fontColor );
		t_X = _x;
		t_Y -= 3;
		_ili->setCursor( t_X, t_Y );
		_ili->setTextColor( _fontColor );
		_ili->println( ( int16_t ) tickValue );
		tickIndex += 1;
		tickValue = tickIndex * scaleTick;
	}
}

/*! \brief Alalaidan akseliluokan konstruktori

	Konstruktori ottaa argumentteina esitett‰v‰n tekstin v‰rit ja stringin sek‰ osoittimen tapahtumank‰sittelij‰‰n.
	Konstruktorina k‰ytet‰‰n kantaluokan konstruktoria suoraan.
	\param but_color "napin" pohjav‰ri joka m‰‰r‰‰ koordinaatiston taustav‰rin
	\param txt_color tekstin v‰ri
	\param Min, Max koordinaatin raja-arvot skaalattuina mittasignaalin arvoalueeseen
	\param sTick Akselin rasterimerkintˆjen v‰li skaalattuna mittasignaaliin
*/
BottomAxis::BottomAxis( uint16_t but_color, uint16_t txt_color, float sMin, float sMax, float sTick ):Axis(but_color,txt_color,sMin,sMax,sTick) {
}

/*! \brief Alalaidan akselin p‰ivitys n‰yttˆˆn

	P‰ivitys piirt‰‰ akselin koordinatiston alalaitaan.
	\todo	asemoi akseliviiva niin ett‰ skaalanumeron korkeus huomioidaan
*/
void BottomAxis::show() {
float tickValue, tickValToPX;
int32_t tickIndex;
uint16_t t_X, t_Y;
	Axis::show();
	_ili->drawFastHLine( _x, _y,_area.x2 - _area.x1, _fontColor );
	tickValToPX = ( _area.x2 - _area.x1 ) / (scaleMax - scaleMin);	// muunnoskerroin pikseli‰ / yksikkˆ jos vaaka-akseli
	tickIndex = ( uint32_t) ( ( scaleMin / scaleTick ) + 1 );
	tickValue = tickIndex * scaleTick;
	while ( tickValue <= scaleMax ) {
		t_Y = _y;
		t_X = _x  + ( uint16_t ) ( tickValToPX * ( tickValue - scaleMin ) );
		_ili->drawFastVLine( t_X, t_Y, MAJORTICKWIDTH, _fontColor );
		t_Y = _y + 6;
		t_X -= 10;
		_ili->setCursor( t_X, t_Y );
		_ili->setTextColor( _fontColor );
		_ili->println( (int16_t) tickValue );
		tickIndex += 1;
		tickValue = tickIndex * scaleTick;
	}
}

RightAxis::RightAxis( uint16_t but_color, uint16_t txt_color, float sMin, float sMax, float sTick ):Axis(but_color,txt_color,sMin,sMax,sTick) {
}

void RightAxis::show() {
float tickValue, tickValToPX;
int32_t tickIndex;
uint16_t t_X, t_Y;
	Axis::show();
	_ili->drawFastVLine( _x, _y,( _area.y2 - _area.y1 ), _fontColor );
	tickValToPX = ( _area.y2 - _area.y1 ) / (scaleMax - scaleMin);	// muunnoskerroin pikseli‰ / yksikkˆ jos pystyakseli
	tickIndex = ( uint32_t) ( ( scaleMin / scaleTick ) + 1 );
	tickValue = tickIndex * scaleTick;
	while ( tickValue <= scaleMax ) {
		t_X = _x;
		t_Y = _area.y2 - ( uint16_t ) ( tickValToPX * ( tickValue - scaleMin ) );
		_ili->drawFastHLine( t_X, t_Y, MAJORTICKWIDTH, _fontColor );
		t_X += 5;
		t_Y -= 3;
		_ili->setCursor( t_X, t_Y );
		_ili->setTextColor( _fontColor );
		_ili->println( (int16_t) tickValue );
		tickIndex += 1;
		tickValue = tickIndex * scaleTick;
	}
}

/*! \brief Koordinaatiston konstruktori

	Koordinatistolle m‰‰ritell‰‰n konstruktorissa piirtoala ja tapahtumank‰sittelij‰ joka kaappaa koordinaatiston alueelle osuvat kosketukset.
	\param X1,Y1,X2;Y2 koordinaatit
	\param color koordinaatiston pohjav‰ri
	\param gEvHandler widgetin tapahtumank‰sittelij‰n osoite
*/
cartesian::cartesian( uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color, gEvHandler handler ):gText(X1,Y1,X2,Y2,handler)  {
	p_X1 = X1; p_Y1 = Y1 + PLOTAREA_TOPMARGIN; p_X2 = X2 - PLOTAREA_RIGHTMARGIN; p_Y2 = Y2;
	_fontBgColor = color;
	leftAxis = NULL;
	bottomAxis = NULL;
	rightAxis = NULL;
}

/*! \brief Akselin assosiointi koordinaatistoon

	Metodi lis‰‰ koordinaatistoon akselin parametrilla indikoituun positioon. Positiossa mahdollisesti aiemmin ollut akseli korvataan uudella.
	Mik‰li on tarvetta, setAxis sovittaa akseleiden risteyskohdat koordinaatiston nurkissa siten, ett‰ ne eiv‰t mene p‰‰llekk‰in.
	\param pos indikaattori haluttuun akselipositioon (enum axisPosition)
	\param *ax osoitin assosioitavaan akseliluokkaan
*/
void cartesian::setAxis(  axisPosition pos, Axis *ax  ) {
//Serial.println(">cartesian.setAxis");
	switch ( pos ) {
		case AXPOS_LEFT: {
			if ( leftAxis ) delete leftAxis;
			leftAxis = (LeftAxis *)ax;
			p_X1 = _area.x1 + ax->getTextWidth();
			break;
		}
		case AXPOS_BOTTOM: {
			if ( bottomAxis ) delete bottomAxis;
			bottomAxis = (BottomAxis *)ax;
			p_Y2 = _area.y2 - ax->getTextHeight();
			break;
		}
		case AXPOS_RIGHT: {
			if ( rightAxis ) delete rightAxis;
			rightAxis = (RightAxis *)ax;
			p_X2 = _area.x2 - ax->getTextWidth();
			break;
		}
		default: {
			
		}
	}
	if (leftAxis ) leftAxis->defineArea( 0, p_Y1, p_X1, p_Y2 );
	if (bottomAxis ) bottomAxis->defineArea( p_X1, p_Y2, p_X2, _area.y2 );
	if (rightAxis ) rightAxis->defineArea( p_X2, p_Y1, _area.x2, p_Y2 );
}

void cartesian::addPlot( Plot *pl ) {
	plotList.insertLast( pl );
}

/*! \brief Koordinaatiston p‰ivitys n‰yttˆˆn

	Metodi siivoaa koordinaatiston piirtoalan t‰ytt‰m‰ll‰ sen taustav‰rill‰.
	Sen j‰lkeen p‰ivitet‰‰n vuorollaan kaikki assosioidut akselit ja lopuksi iteroidaan kuvaajien lista l‰pi ja p‰ivitet‰‰n jokainen kuvaaja.
	\todo aktivoi kuvaajan show() -metodi jahka se on toteutettu
*/
void cartesian::show() {
Plot *plot;
	_ili->fillRect(_area.x1,_area.y1,_area.x2-_area.x1,_area.y2-_area.y1,_fontBgColor);
	if ( leftAxis ) leftAxis->show();
	if ( bottomAxis ) bottomAxis->show();
	if ( rightAxis ) rightAxis->show();
	plot = (Plot *)plotList.getFirst();
	while ( plot ) {
		plot->show( this );
		plot = (Plot *)plotList.getNext();
	}
}


 int16_t cartesian::getPXPos(uint8_t index, float val) {
 	if ( (index & 0x01) == 0x00 ){
		return p_Y2 - ( val - leftAxis->getMin() ) * ( ( p_Y2 - p_Y1 ) / ( leftAxis->getMax() - leftAxis->getMin() ) );
	} else {
		return p_X1 + val * ( ( p_X2 - p_X1 ) / ( bottomAxis->getMax() - bottomAxis->getMin() ) );
	}
}

/*! \brief Skaalatun viivan piirto koordinatistoon

	Metodi piirt‰‰ viivan suoraan piirtoalalle lis‰‰m‰tt‰ sit‰ mihink‰‰n kuvaajaan.
	Viivan x- ja y-koordinaatit ovat reaalimaailman skaalassa, ei pikseleit‰.
	\param x0,y0,x1,y1 viivan p‰‰tepisteiden koordinaatit
	\todo lis‰‰ argumentit joilla viiva assosioidaaan X- ja Y-akseleihin
	\todo toteuta metodi! (mutta vain jos oikeasti tarvitaan)
*/
void cartesian::drawLine(float x0, float y0, float x1, float y1) {
	_ili->drawLine(getPXPos(1, x0), getPXPos(0, y0), getPXPos(1, x1), getPXPos(0, y1), 0x0000 );
}

//-------------------------------------------------------

boolean View::screenIsDirty;

/*! \brief View-luokan oletuskonstruktori. Ei juuri hyˆdyllinen koska muut tarvittavat metodit puuttuu
*/
View::View() {
	preInit = NULL;
	postInit = NULL;
	finalize = NULL;
	viewHandler = NULL;
}

/*! \brief View-luokan factory-konstruktori.

	Kaikki n‰ytˆll‰ n‰kyv‰t jutut kuuluvat johonmkin kerralla esitett‰v‰‰n n‰kym‰‰n joka on toteutettu t‰m‰n View-luokan instanssina.
	N‰kym‰ss‰ olevat widgetit synnytet‰‰n kahdella tavalla:
	1:	Suoraan konstruktorissa joka toimii widgetfactoryna. Widgetit on listattu staattiseen struktuuriin joka annetaan argumenttina konstruktorille
		Konstruktori k‰y listan l‰pi ja luo halutun tyyppisen widgetin annettujen parametrien mukaisesti.
	2:	Alustuken yhteydess‰ tai myˆhemminkin kutsumalla Viewin add-metodia jolla n‰kym‰‰n lis‰t‰‰n itse "k‰sin" luotuja widgettej‰. T‰ss‰ tapauksessa pit‰‰
		kiinnitt‰‰ huomiota luontij‰rjestykseen koska se m‰‰r‰‰ widgettien indeksin listalla. Lista on t‰rke‰ tyˆkalu widgettej‰ ja niiden sis‰ltˆ‰ manipuloitaessa.
		View::add() lis‰‰ widgetin aina listan viimeiseksi jolloin se saa suurimman senhetkisen indeksin. Indeksit alkaa nollasta.
	\param elems luontistruktuurin elemnttien (widgettien) lukum‰‰r‰
	\param elemparams[] widgettien luontistruktuuri
	\param preinit osoitin esialustusproseduuriin joka ajetaan ennen widgettien konstruoimista. T‰st‰ ei saa viitata widget-listaan koska ei ole widgettej‰ viel‰!
	\param postinit osoitin j‰lkialustusproseduuriin joka ajetaan kun kaikki listalla olevat widgetit on luotu ja olemassa.
	\param preshow osoitin p‰ivityksen valmisteluproseduuriin joka ajetaan aina ennen View::show() -metodia.
	\param final osoitin n‰kym‰n lopetusproseduuriin joka ajetaan juuri ennen n‰kym‰n destruktoria. Jos n‰kym‰‰n on k‰sin allokoitu resursseja niin t‰m‰ on viimeinen mahdollisuus vapauttaa ne.
	\param gViewHandler osoitin n‰kym‰n k‰sittelij‰‰n. Jokainen kosketus poikii kutsun sek‰ osuman saaneen widgetin k‰sittelij‰‰n ett‰ t‰h‰n. Widgetit hoidetaan ensin, lopuksi n‰kym‰.
	Mik‰li joitakin linkatuista proseduureista ei tarvita, on vastaavan argumentin arvon oltava NULL jolloin se j‰tet‰‰n k‰ytt‰m‰tt‰.
*/
View::View(uint8_t elems, gTextParamStruct elemparams[], gPrepare preinit, gPrepare postinit, gPrepare preshow, gPrepare final, gViewHandler vh) {
	preInit = preinit;
	postInit = postinit;
	preShow = preshow;
	finalize = final;
	viewHandler = vh;
	viewBgColor = ILI9341_GRAY;
	if ( preInit ) {
		preInit( this );
	}
	for ( uint8_t cnt = 0; cnt < elems; cnt++ ) {
		switch ( elemparams[cnt].etype ) {
			case e_PGMTextArea: {
				add( new PGM_TextArea(elemparams[cnt].x1, elemparams[cnt].y1, elemparams[cnt].x2, elemparams[cnt].y2, elemparams[cnt].color_bg, elemparams[cnt].color_fg, (PGM_P *)elemparams[cnt].txt, elemparams[cnt].hnd ));
				break;
			}
			case e_TextArea: {
				add ( new TextArea(elemparams[cnt].x1, elemparams[cnt].y1, elemparams[cnt].x2, elemparams[cnt].y2, elemparams[cnt].color_bg, elemparams[cnt].color_fg, (char **)elemparams[cnt].txt, elemparams[cnt].hnd ));
				break;
			}
			case e_Caption: {
				add ( new Caption(elemparams[cnt].x1, elemparams[cnt].y1, elemparams[cnt].x2, elemparams[cnt].y2, elemparams[cnt].color_bg, elemparams[cnt].color_fg, elemparams[cnt].txt, elemparams[cnt].hnd ));
				break;
			}
			case e_Button: {
				add ( new Button(elemparams[cnt].x1, elemparams[cnt].y1, elemparams[cnt].x2, elemparams[cnt].y2, elemparams[cnt].color_bg, elemparams[cnt].color_fg, elemparams[cnt].txt, elemparams[cnt].hnd ));
				break;
			}
			case e_cartesian: {
				break;
			}
		}
	}
	if ( postInit ) {
		postInit( this );
	}
}

/*! \brief View-luokan destruktori

	Juuri ennen luokan tuhoamista kutsutaan finalize() -lopetusk‰sittelij‰‰ jos sellainen on m‰‰ritelty.
	Sen j‰lkeen kaikki n‰kym‰‰n kuuluvat widgetit tuhotaan ja merkataan n‰yttˆ likaiseksi s.o. ett‰ se pit‰‰ p‰ivitt‰‰.
	N‰kym‰n tuhoamisen j‰lkeisen p‰ivityksen hoitaa se n‰kym‰ josta t‰h‰n tultiin, eli sen k‰sitelij‰ tsekkaa muuttujan screenIsDirty ja toimii vastaavasti.
	Juurin‰kym‰‰ ei siis tietenk‰‰n passaa tuhota koska silloin ei ole en‰‰ ket‰‰n joka hoitaa hommat...
*/
View::~View() {
	if ( finalize ) finalize( this );
	elementList.deleteAll();
	screenIsDirty = true;
}

/*! \brief Uuden widgetin lis‰‰minen n‰kym‰‰n listan viimeiseksi

	\param *element osoitin lis‰tt‰v‰‰n widgettiin. K‰ytt‰j‰n vastuulla on lis‰t‰ vain aitoja widgettej‰!
*/
void View::add(void *element) {
	elementList.insertLast( element );
}

/*! \brief N‰kym‰n p‰ivitys

	N‰kym‰ piirret‰‰n alusta alkaen uudelleen ja k‰sitell‰‰n mahdolliset eventit pois
*/
void View::show() {
	render();
	handleView();
}

/*! \brief Widgetin listaindeksin nouto osoittimen perusteella

	Metodi palauttaa widgetin indeksin n‰kym‰n widgetlistalla.
	Jollei osoitettu widget ole listalla, metodi palauttaa -1.
	\param *item osoitin widgettiin jonka indeksi halutaan tiet‰‰
*/
int8_t View::getItemIndex( void *item) {
gText *elem;
int8_t cnt;

	cnt = 0;
	elem = (gText *)elementList.getFirst();
	while ( elem ) {
		if ( elem == item ) {
			return cnt;
		}
		cnt++;
		elem = (gText *)elementList.getNext();
	}
	return -1;	// ei lˆytynyt
}

/*! \brief Widgetin osoittimen nouto listaindeksin perusteella

	Metodi palauttaa osoittimen widgetiin n‰kym‰n widgetlistan indeksin perusteella.
	Jollei listalta lˆydy annettua indeksi, metodi palauttaa NULL pointerin.
	Huom: listan ensimm‰inen indeksi on 0.
	\param index halutun widgetin j‰rjestysnumero listalla 
*/
void *View::getItem(uint8_t index) {
void *elem;
int8_t cnt;

	cnt = 0;
	elem = (gText *)elementList.getFirst();
	while ( elem ) {
		if ( index == cnt ) return elem;
		else {
			elem = (gText *)elementList.getNext();
			cnt++;
		}
	}
	return NULL;
}

/*! \brief Kolme nopeaa piippausta (n‰pp‰ilyvirheet yms)
*/
void View::keyErrorBeep() {
	for ( uint8_t i = 0; i < 3; i++ ) {
		vTaskDelay(50);
		digitalWrite(BEEPER, HIGH);
		vTaskDelay(50);
		digitalWrite(BEEPER, LOW);
	}
}

/*! \brief N‰kym‰n piirto n‰ytˆlle

	Metodi valmistelee n‰ytˆn piirron kutsumalla proseduuria preShow mik‰li se oli konstruktorissa annettu.
	Sen j‰lkeen piirtoala siivotaan taustav‰rill‰ t‰ytt‰en ja n‰ytet‰‰n widget-lista alusta loppuun.
	Lopuksi merkataan n‰yttˆ puhtaaksi eli ajantasaiseksi.
*/
void View::render() {
	gText *elem;
	if ( preShow ) preShow( this );
	else ILI9341_due_gText::_ili->fillScreen(viewBgColor);
	elem = (gText *)elementList.getFirst();
	while ( elem ) {
		elem->show();
		elem = (gText *)elementList.getNext();
	}
	screenIsDirty = false;
}

// kosketuksen tunnistus
uint8_t touching() {
	if (digitalRead( YM ) == LOW) return 1;
	else return 0;
}

//yksinkertainen j‰rjestetty lista
static void insert_sort(uint16_t array[], uint8_t size) {
	uint16_t j;
	uint16_t save;
	
	for (uint16_t i = 1; i < size; i++) {
		save = array[i];
		for (j = i; j >= 1 && save < array[j - 1]; j--)
		array[j] = array[j - 1];
		array[j] = save;
	}
}

#define NUMSAMPLES 5

uint16_t x, y, z;

// kosketuskalvon inputtien viritys kosketuksen tunnistusta varten
void resetTouchPins() {
	pinMode( XP, OUTPUT );
	digitalWrite( XP, LOW );
	
	pinMode( XM, INPUT );
	digitalWrite( XM, LOW );
	
	pinMode( YP, INPUT );
	digitalWrite( YM, LOW );

	pinMode( YM, INPUT_PULLUP );
	digitalWrite( YM, HIGH );
}

// kosketuskalvon inputtien viritys X-akselin position tunnistusta varten
void setTouchPinsForX() {
	pinMode(YP, INPUT);
	pinMode(YM, INPUT);
	digitalWrite(YP, LOW);
	digitalWrite(YM, LOW);
	
	pinMode(XP, OUTPUT);
	pinMode(XM, OUTPUT);
	digitalWrite(XP, HIGH);
	digitalWrite(XM, LOW);
}

// kosketuskalvon inputtien viritys Y-akselin position tunnistusta varten
void setTouchPinsForY() {
	pinMode(XP, INPUT);
	pinMode(XM, INPUT);
	digitalWrite(XP, LOW);
	digitalWrite(XM, LOW);
	
	pinMode(YP, OUTPUT);
	pinMode(YM, OUTPUT);
	digitalWrite(YP, HIGH);
	digitalWrite(YM, LOW);
}

// kosketuskoordinaatin mittaus
void getPoint(uint16_t *x, uint16_t *y) {
	uint16_t samples[NUMSAMPLES];
	uint8_t i;
	uint16_t _x, _y;

	setTouchPinsForX();
	for (i=0; i<NUMSAMPLES; i++) samples[i] = analogRead(YP);
	#if NUMSAMPLES > 2
	insert_sort(samples, NUMSAMPLES);
	#endif
	#if NUMSAMPLES == 2
	if (samples[0] != samples[1]) { valid = 0; }
	#endif
	_x = (samples[NUMSAMPLES/2]);

	setTouchPinsForY();
	for (i=0; i<NUMSAMPLES; i++) samples[i] = analogRead(XM);
	#if NUMSAMPLES > 2
	insert_sort(samples, NUMSAMPLES);
	#endif
	#if NUMSAMPLES == 2
	if (samples[0] != samples[1]) { valid = 0; }
	#endif
	_y = (samples[NUMSAMPLES/2]);

	resetTouchPins();
	*x = map(_x,MIN_X,MAX_X, 0, DISP_W);
	*y = map(_y,MIN_Y,MAX_Y, 0, DISP_H);
}
//-----------------

extern boolean backLight;

/*! \brief N‰kym‰n tapahtumank‰sittelij‰

	Tapahtumank‰sittelij‰ havainnoi kosketuksia ja sellaisen tapahtuessa tekee seuraavaa:
	1.	tuorestetaan n‰ytˆn taustavalo mik‰li se olisi sattunut sammumaan timeouttiin.
	2.	Jos taustavalo oli p‰‰t‰‰ (eli n‰yttˆ oli "aktiivinen") niin
		-	annetaan kosketuksen merkki‰‰ni,
		-	mitataan kosketuksen koordinaatit
		-	k‰yd‰‰n l‰pi n‰kym‰n widget-lista lopusta alkuun (p‰‰llimm‰isest‰ widgetist‰ pohjimmaiseen) ja tsekataan osuuko kosketus widgettiin
		-	kutsutaan osuman saaneen widgetin k‰sittelij‰‰
		-	kutsutaan n‰kym‰n k‰sittelij‰‰ jos m‰‰ritelty
		-	tarvittaessa tuorestetaan n‰yttˆ jos joku k‰sittelijˆiden k‰ynnist‰m‰ popup sotki sen
		-	odotellaan ett‰ kosketus p‰‰ttyy jottei toisteta samaa kosketusta holtittomasti.
	Huom: Widgetin tai n‰kym‰n tapahtumank‰sittelij‰ voi vapaasti luoda uuden View-luokan instanssin n‰ytt‰‰kseen jonkin popupin tai muun n‰kym‰n.
	T‰llˆin n‰kym‰t pinoutuvat rekursiivisesti ja k‰sittelij‰st‰ tullaan ulos vasta kun alemman tason n‰yttˆ on k‰sitelty loppuun asti.
	T‰m‰n takia jokaisen handlerin j‰lkeen on tsekattava viel‰kˆ n‰yttˆ on siin‰ asussa kuin kuvitellaan vai sotkiko joku alemman tason n‰yttˆ sen
	jolloin tarvitaan uudelleen renderˆinti.
*/
void View::handleView() {
TickType_t xStartDelayTime;
const TickType_t xTouchBeepTime = BEEPERONTIME;
const TickType_t xBackLightOnTime = BACKLIGHTONTIME;
uint16_t x, y;
event_t event;
gText *elem;

	exitView = false;
	resetTouchPins();
	for ( ;; ) {
		if ( touching() ) {											// odotellaan kosketusta
			xTimerReset( backLightTimer, xBackLightOnTime );		// n‰ytˆn taustavalolle lis‰‰ aikaa
			if ( backLight ) {										// jos taustavalo on p‰‰ll‰ (eli n‰yttˆ on aktiivinen)
				xStartDelayTime = xTaskGetTickCount();				// otetaan aikamerkki piippausajastinta varten
				digitalWrite(BEEPER, HIGH);							// k‰ynnistet‰‰n piippaus
				getPoint(&x, &y);									// mitataan kosketuksen koordinaatit
				event.eventID = eventTouch;							// rakennetaan eventti
				event.event_data.pos.x = x;
				event.event_data.pos.y = y;
				vTaskDelayUntil(&xStartDelayTime, xTouchBeepTime);	// ajastetaan piippaus loppuun alkumerkin mukaan (50 ms)
				digitalWrite(BEEPER, LOW);							// ja mykistet‰‰n piippari
				elem = (gText *)elementList.getLast();				// k‰yd‰‰n l‰pi kaikki n‰ytˆll‰ n‰kyv‰t elementit aloittaen p‰‰llimm‰isest‰
				while ( elem ) {
					if ( elem->isInside( x, y ) ) {
						elem->handleEvent( this, event );			// ...ja jos kosketus osuu niin k‰sitell‰‰n
						break;
					}
					elem = (gText *)elementList.getPrev();
				}
				if ( viewHandler ) viewHandler( this, event );		// lopuksi yleisk‰sittely jos on m‰‰ritelty
				if ( !exitView && screenIsDirty ) render();			// jos handleri n‰ytti popup-ikkunaa niin joudutaan ehk‰ regeneroimaan n‰yttˆ
			}
			else {
				digitalWrite( BACKLIGHT, HIGH );					// jos taustavalo oli pime‰n‰, se sytytet‰‰n kosketuksesta
				backLight = true;									// mutta ei tehd‰ muuta
			}
			while ( touching() ) {									// odotellaan ett‰ kosketus p‰‰ttyy
				taskYIELD();
			}
			vTaskDelay(100);											// ja viel‰ 100 ms ettei tule bounceja
			if ( exitView ) {
				exitView = false;
				return;									// jos joku laittoi lopetusbitin pystyyn niin h‰ivyt‰‰n ( ja toivotaan ett‰ takana on yh‰ edellinen ikkuna :) )
			}
		}
		else taskYIELD();											// jos ei kosketusta niin annetaan muille peliaikaa
	}
}

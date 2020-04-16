/*! \brief GUI-näytön yhtenäistetty koordinaatisto

	Created: 17.5.2015 8:56:39
	Author: Kremmen
	Tässä headerissa on listattu LCD-näytön X- ja Y-koordinatteja niin, että niillä on helppoa ja yhdenmukaista
	luoda otsikko- ja syöttökenttiä sekä nappeja eri näkymiin ja saada kaikki yhtenäisen kokoisiksi ja samoihin paikkoihin.
	X-suunnassa löytyy kordinaatit 2, 3 ja 4 sarakeelle; Y- suunnassa 2, 4 ja 8 riville.
	Alussa on listattu joitakin aiempia deprekoituja koordinaatteja jotka mahdollisesti poistetaan jatkossa.
	Harmonisoidut koordinaatit ovat muotoa Sanm missä S on suunta (X tai Y), a=rivien/sarakkeiden kokonaismäärä, n=rivi (2,3,4 tai 8) ja m=alku/loppukoordinaatti (1 tai 2) 
 */ 


#ifndef COORDINATES_H_
#define COORDINATES_H_

// ruudun koko
#define DISP_W 240
#define DISP_H 320

// Buttonrasterin mittoja näytöllä
#define X_MARGIN 10
#define Y_MARGIN 6

#define B_LEFT_X1 X_MARGIN
#define B_LEFT_X2 ((DISP_W/2)-(X_MARGIN/2))

#define B_RIGHT_X1 ((DISP_W/2)+(X_MARGIN/2))
#define B_RIGHT_X2 (DISP_W-X_MARGIN)

#define B_TOP_Y1 ((DISP_H/2)+Y_MARGIN)
#define B_TOP_Y2 ((DISP_H*3/4)-(Y_MARGIN/2))

#define B_BOTTOM_Y1 ((DISP_H*3/4)+(Y_MARGIN/2))
#define B_BOTTOM_Y2 (DISP_H-Y_MARGIN)

// Lisää nappien jne koordinaatteja näytöllä
#define BMARGIN 6
#define BTOP 80
#define BBOTTOM (DISP_H-BMARGIN/2)
#define BHEIGHT ((BBOTTOM-BTOP)/5)
#define BCOLUMNS3 3

#define X1_1 (BMARGIN/2)
#define X1_2 (DISP_W/BCOLUMNS3-(BMARGIN/2))
#define X2_1 (DISP_W/BCOLUMNS3+(BMARGIN/2))
#define X2_2 (2*DISP_W/BCOLUMNS3-(BMARGIN/2))
#define X3_1 (2*DISP_W/BCOLUMNS3+(BMARGIN/2))
#define X3_2 (DISP_W-BMARGIN/2)

#define Y1_1 (BTOP+BMARGIN/2)
#define Y1_2 (BTOP+BHEIGHT-BMARGIN/2)
#define Y2_1 (BTOP+BHEIGHT+BMARGIN/2)
#define Y2_2 (BTOP+2*BHEIGHT-BMARGIN/2)
#define Y3_1 (BTOP+2*BHEIGHT+BMARGIN/2)
#define Y3_2 (BTOP+3*BHEIGHT-BMARGIN/2)
#define Y4_1 (BTOP+3*BHEIGHT+BMARGIN/2)
#define Y4_2 (BTOP+4*BHEIGHT-BMARGIN/2)
#define Y5_1 (BTOP+4*BHEIGHT+BMARGIN/2)
#define Y5_2 BBOTTOM

#define NUMROWS 7
#define LISTROWH (DISP_H/NUMROWS)
#define LY1_1 (BMARGIN/2)
#define LY1_2 (LISTROWH-BMARGIN/2)
#define LY2_1 (LISTROWH+BMARGIN/2)
#define LY2_2 (2*LISTROWH-BMARGIN/2)
#define LY3_1 (2*LISTROWH+BMARGIN/2)
#define LY3_2 (3*LISTROWH-BMARGIN/2)
#define LY4_1 (3*LISTROWH+BMARGIN/2)
#define LY4_2 (4*LISTROWH-BMARGIN/2)
#define LY5_1 (4*LISTROWH+BMARGIN/2)
#define LY5_2 (5*LISTROWH-BMARGIN/2)
#define LY6_1 (5*LISTROWH+BMARGIN/2)
#define LY6_2 (6*LISTROWH-BMARGIN/2)
#define LY7_1 (6*LISTROWH+BMARGIN/2)
#define LY7_2 (7*LISTROWH-BMARGIN/2)

/* Marginaalit */

#define YMARGIN 6
#define XMARGIN 6

/* 2 tasan jakautunutta riviä */
#define Y2ROWS 2
#define Y2ROWH DISP_H/Y2ROWS
// Y211: Y2=2riviä; 1=eka rivi jne; 1=ylälaita 2=alalaita
#define Y211 YMARGIN
#define Y212 (1*Y2ROWH-YMARGIN)
#define Y221 (1*Y2ROWH+YMARGIN)
#define Y222 (2*Y2ROWH-YMARGIN)

/* 4 tasan jakautunutta riviä */
#define Y4ROWS 4
#define Y4ROWH DISP_H/Y4ROWS
// Y411: Y4=4riviä; 1=eka rivi jne; 1=ylälaita 2=alalaita
#define Y411 YMARGIN
#define Y412 (1*Y4ROWH-YMARGIN)
#define Y421 (1*Y4ROWH+YMARGIN)
#define Y422 (2*Y4ROWH-YMARGIN)
#define Y431 (2*Y4ROWH+YMARGIN)
#define Y432 (3*Y4ROWH-YMARGIN)
#define Y441 (3*Y4ROWH+YMARGIN)
#define Y442 (4*Y4ROWH-YMARGIN)

/* 8 tasan jakautunutta riviä */
#define Y8ROWS 8
#define Y8ROWH DISP_H/Y8ROWS
// Y811: Y8=8riviä; 1=eka rivi jne; 1=ylälaita 2=alalaita
#define Y811 YMARGIN
#define Y812 (1*Y8ROWH-YMARGIN)
#define Y821 (1*Y8ROWH+YMARGIN)
#define Y822 (2*Y8ROWH-YMARGIN)
#define Y831 (2*Y8ROWH+YMARGIN)
#define Y832 (3*Y8ROWH-YMARGIN)
#define Y841 (3*Y8ROWH+YMARGIN)
#define Y842 (4*Y8ROWH-YMARGIN)
#define Y851 (4*Y8ROWH+YMARGIN)
#define Y852 (5*Y8ROWH-YMARGIN)
#define Y861 (5*Y8ROWH+YMARGIN)
#define Y862 (6*Y8ROWH-YMARGIN)
#define Y871 (6*Y8ROWH+YMARGIN)
#define Y872 (7*Y8ROWH-YMARGIN)
#define Y881 (7*Y8ROWH+YMARGIN)
#define Y882 (8*Y8ROWH-YMARGIN)

/* 2 tasan jakautunutta saraketta */
#define X2COLS 2
#define X2COLW DISP_W/X2COLS
// X211: X2=2saraketta; 1=eka sarake jne; 1=vasenlaita 2=oikealaita
#define X211 XMARGIN
#define X212 (1*X2COLW-XMARGIN)
#define X221 (1*X2COLW+XMARGIN)
#define X222 (2*X2COLW-XMARGIN)

/* 3 tasan jakautunutta saraketta */
#define X3COLS 3
#define X3COLW DISP_W/X3COLS
// X311: X3=2saraketta; 1=eka sarake jne; 1=vasenlaita 2=oikealaita
#define X311 XMARGIN
#define X312 (1*X3COLW-XMARGIN)
#define X321 (1*X3COLW+XMARGIN)
#define X322 (2*X3COLW-XMARGIN)
#define X331 (2*X3COLW+XMARGIN)
#define X332 (3*X3COLW-XMARGIN)

/* 4 tasan jakautunutta saraketta */
#define X4COLS 4
#define X4COLW DISP_W/X4COLS
// X411: X4=2saraketta; 1=eka sarake jne; 1=vasenlaita 2=oikealaita
#define X411 XMARGIN
#define X412 (1*X4COLW-XMARGIN)
#define X421 (1*X4COLW+XMARGIN)
#define X422 (2*X4COLW-XMARGIN)
#define X431 (2*X4COLW+XMARGIN)
#define X432 (3*X4COLW-XMARGIN)
#define X441 (3*X4COLW+XMARGIN)
#define X442 (4*X4COLW-XMARGIN)

#endif /* COORDINATES_H_ */
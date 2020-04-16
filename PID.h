/*! \brief PID-s‰‰t‰j‰luokka sovitettuna l‰mmitysuunin ohjaukseen.

	Alkuper‰inen PID-koodi on nyk‰isty Arduinon esimerkkikoodeista ja raskaasti modattu sopimaan paremmin ep‰lineaarisen l‰mmityssysteemin ohjaukseen.
	
*/
#ifndef PID_v1_h
#define PID_v1_h
#define LIBRARY_VERSION	2.0.0
#define AUTOMATIC	1
#define MANUAL	0
#define DIRECT  0
#define REVERSE  1

#define DSAMPLES 8

/*! \brief PID-s‰‰t‰j‰

	Luokka toteuttaa PID-s‰‰tˆalgoritmin laskettuna skaalaamattomilla liukuluvuilla (==float).
	K‰ytt‰j‰n vastuulla on kutsua luokan Compute() -metodia halutulla laskentaperiodilla - luokka itse ei ota mit‰‰n kantaa vaan olettaa aikav‰lin annetuksi.
*/
class PID {
public:
	PID(float* Input, float* Output, float* Setpoint, float Kp, float Ki, float Kd, int ControllerDirection);	// * constructor.  links the PID to the Input, Output, and Setpoint.  Initial tuning parameters are also set here
	void SetMode(int Mode);						//!< sets PID to either Manual (0) or Auto (non-0)
	bool Compute();								//!< performs the PID calculation.
	void SetOutputLimits(float Min, float Max);	//!< rajaa s‰‰t‰j‰n l‰hdˆn annetylle arvoalueelle

	void SetTunings(float Kp, float Ki, float Kd);	//!< s‰‰t‰j‰n viritysarvojen muuttaminen
 	void SetControllerDirection(int);				//!< s‰‰t‰j‰n toimintasuunnan asetus
	void SetSampleTime(int NewSampleTime);              // * sets the frequency, in Milliseconds, with which 

	float GetKp();						//!< P-arvon kysely
	float GetKi();						//!< I-arvon kysely
	float GetKd();						//!< D-arvon kysely
	int GetMode();						//!< s‰‰t‰j‰n toimintatilan kysely
	int GetDirection();					//!< s‰‰t‰j‰n toimisuunnan kysely
	float getPTerm() { return PTerm; }	//!< hetkellisen P-arvon kysely
	float getITerm() { return ITerm; }	//!< hetkellisen I-arvon kysely
	float getDTerm() { return DTerm; }	//!< hetkellisen D-arvon kysely
		
  private:
	void Initialize();
	
	float dispKp;				//!< k‰ytt‰j‰n syˆtt‰m‰ P-arvo 
	float dispKi;				//!< k‰ytt‰j‰n syˆtt‰m‰ I-arvo 
	float dispKd;				//!< k‰ytt‰j‰n syˆtt‰m‰ D-arvo 
    
	float kp;					//!< P-kerroin 
	float ki;					//!< I-aikavakio sekunteja
	float kd;					//!< D-aikavakio sekunteja
	int controllerDirection;	//!< s‰‰t‰j‰n toimisuunta

	float *myInput;				//!< osoitin olarvomuuttujaan
	float *myOutput;			//!< osoitin s‰‰t‰j‰n l‰htˆmuuttujaan
	float *mySetpoint;			//!< osoitin ohjearvomuuttujaan

	float PTerm;				//!< laskettu P-termi
	float ITerm;				//!< laskettu I-termi
	float DTerm;				//!< laskettu D-termi
	
	float Dbuffer[DSAMPLES];	//!< D-termin keskiarvoistamiseen k‰ytetty puskuri
	uint8_t bufindex = 0;		//!< D-termin keskiarvopuskurin kirjoitusindeksi
			  
	unsigned long lastTime;		//!< edellisen ajokerran aikaleima (ei k‰ytˆss‰)
	float lastError;			//!< edellisen ajokerran eroarvo

	uint32_t SampleTime;		//!< s‰‰t‰j‰n laskennallinen n‰ytev‰li
	float outMin, outMax;		//!< l‰hdˆn raja-arvot
	bool inAuto;				//!< ajotilan indikaattori
};
#endif


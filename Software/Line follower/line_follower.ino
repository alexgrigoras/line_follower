/*
 *  Robot Line Follower with Arduino
 *  
 *  Autor: Grigoras Alexandru
 */

/* --------------INCEPUT PROGRAM------------- */

#include <QTRSensors.h>

#define STANDBY           8   /* Pin-ul de STANDBY al driverului de motoare */
  
#define MOTORLEFT_DIR_A  12   /* Pin-ul 1 de directie al motorului stang */  
#define MOTORLEFT_DIR_B  13   /* Pin-ul 2 de directie al motorului stang */ 
#define MOTORLEFT_PWM    11   /* Pin-ul PWM al motorului stang */ 

#define MOTORRIGHT_DIR_A  3   /* Pin-ul 1 de directie al motorului drept */  
#define MOTORRIGHT_DIR_B  4   /* Pin-ul 1 de directie al motorului drept */  
#define MOTORRIGHT_PWM    5   /* Pin-ul PWM al motorului drept */ 

#define NUM_SENSORS       8   /* Numarul de senzori utilizati */

/*
 * Cantitatea de citiri analogice pentru senzori
 */ 
#define TIMEOUT 2500
#define EMITTER_PIN   9   /* Pin-ul emitator al senzorului QTR */

/* ------------------------------------------ */

/*
 * Structura senzorilor
 * Senzorii sunt conectati in pinii analogici A0-A7 sau pinii digitali 14-21
 */
QTRSensorsRC qtrrc((unsigned char[]) {  14, 15, 16, 17, 18, 19, 20, 21} , NUM_SENSORS, TIMEOUT, EMITTER_PIN); 

/*
 * Stocarea valorilor senzorilor
 */
unsigned int sensorValues[NUM_SENSORS];

/* ------------------------------------------ */

  /*
   * Functie pentru motorul stang
   */
  void setMotorLeft(int value)
  {
    if ( value >= 0 )
    {
      /* Daca valoarea este pozitiva merge inainte */
      digitalWrite(MOTORRIGHT_DIR_A,HIGH);
      digitalWrite(MOTORRIGHT_DIR_B,LOW);
    }
    else
    {
      /* Daca valoarea este pozitiva inversam directia */
      digitalWrite(MOTORRIGHT_DIR_A,LOW);
      digitalWrite(MOTORRIGHT_DIR_B,HIGH);
      value *= -1;
    }
    /* Setare viteza */
    analogWrite(MOTORRIGHT_PWM,value);
  }

  /*
   * Functie pentru motorul drept
   */
  void setMotorRight(int value)
  {  
    if ( value >= 0 )
    {
      /* Daca valoarea este pozitiva merge inainte */
      digitalWrite(MOTORLEFT_DIR_A,HIGH);
      digitalWrite(MOTORLEFT_DIR_B,LOW);
    }
    else
    {
      /* Daca valoarea este pozitiva inversam directia */
      digitalWrite(MOTORLEFT_DIR_A,LOW);
      digitalWrite(MOTORLEFT_DIR_B,HIGH);
      value *= -1;
    }    
    /*  Setare viteza  */
    analogWrite(MOTORLEFT_PWM,value);
  }

  /*
   * Functie pentru viteza motoarelor
   */
  void setMotors(int left, int right)
  {
    digitalWrite(STANDBY,HIGH);
    setMotorLeft(left);
    setMotorRight(right);
  }

  /*
   * Functie pentru franarea motoarelor
   */
  void setBrake(boolean left, boolean right, int value)
  {
    /* pin STANDBY */
    digitalWrite(STANDBY,HIGH);
  
    if ( left )
    {
      /* Motorul stang => crestere viteza motor drept */
      digitalWrite(MOTORRIGHT_DIR_A,HIGH);
      digitalWrite(MOTORRIGHT_DIR_B,HIGH);
      analogWrite (MOTORRIGHT_PWM, value);
    }
  
    if ( right )
    {
      /* Motorul drept => crestere viteza motor stang */
      digitalWrite(MOTORLEFT_DIR_A,HIGH);
      digitalWrite(MOTORLEFT_DIR_B,HIGH);
      analogWrite (MOTORLEFT_PWM, value);
    }
  }

  /*
   * Functie pentru setari motor si calibrare senzori
   */
  void setup()
  {
    /* Initializare pini de iesire */
    pinMode(STANDBY, OUTPUT);
    pinMode(MOTORRIGHT_DIR_A, OUTPUT);
    pinMode(MOTORRIGHT_DIR_B, OUTPUT);
    pinMode(MOTORRIGHT_PWM, OUTPUT);
    pinMode(MOTORLEFT_DIR_A, OUTPUT);
    pinMode(MOTORLEFT_DIR_B, OUTPUT);
    pinMode(MOTORLEFT_PWM, OUTPUT);

    /* calibrare sensori qtr */
    for ( int i=0; i<100; i++)
    {
      qtrrc.calibrate();
    }
    
    /* Asteptare 5 secunde */
    delay(5000);

    /* Miscare robot inainte pentru 0.3 secunde */
    setMotors(90, 90);
    delay(300);
  }

/* ------------------------------------------ */

  /*
   * Variabile utilizate
   */
  unsigned int position = 0; /* Pozitia curenta a senzorilor */
  int derivative = 0;        /* Derivat */
  int proportional = 0;      /* Proportional */
  int power_difference = 0;  /* Diferenta de viteza */
  int max = 250;             /* Viteza maxima */
  int last_proportional;     /* Proportionala anterioara */
  float KP = 0.2;           /* Constanta proportionala */
  float KD = 2;              /* Constanta derivata */
  
  /* Contanta pentru intervalul de franare */
  #define RANGEBRAKE 2500

  /*
   * Bucla principala a programului
   */
  void loop()
  {   
    /*
     * Obține poziția liniei
     * Nu ne interesează valorile individuale ale fiecărui senzor
     */
    position = qtrrc.readLine(sensorValues);
  
    /* Termenul proporțional trebuie să fie 0 atunci când suntem pe linie */
    proportional = ((int)position) - 2500;
  
    /* Dacă se încadrează în intervalul de frânare, se aplica în direcţia curbei */
    if ( proportional <= -RANGEBRAKE )
    {
      setMotorRight(0);
      setBrake(true,false,255);
      delay(1);
    }
    else if ( proportional >= RANGEBRAKE )
    {
      setMotorLeft(0);
      setBrake(false,true,255);
      delay(1);
    }
  
    /* Calculare pozitie termen derivat si modificare */
    derivative = proportional - last_proportional;
  
    /* Memoreaza ultima pozitie */
    last_proportional = proportional;

    /*
     * Calculează diferența dintre puterea celor doua motoare [m1 - m2]
     * Dacă este un număr pozitiv, robotul se roteşte [dreapta]
     * Dacă este un număr pozitiv, robotul se roteşte [stanga]
     * Mărimea numărului determină unghiul de rotație
     */
    int power_difference = ( proportional * KP ) + ( derivative * KD );
  
    /*
     * Daca diferentiala viteza este mai mare decât cea posibilă atât pozitiv şi negativ, 
     * atribuie valoarea maximă permisă
     */
    if ( power_difference > max ) 
    {
      power_difference = max; 
    }
    else if ( power_difference < -max ) 
    {
      power_difference = -max;
    }
  
    /* Atribuiți viteză calculată în puterea diferențială a motorului */
    if( power_difference < 0 )
    {
      setMotors(max+power_difference, max);
    }
    else 
    {
      setMotors(max, max-power_difference);
    }
  }

/* --------------SFARSIT PROGRAM------------- */

  

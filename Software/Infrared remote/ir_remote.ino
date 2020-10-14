
#include <IRremote.h>

//motor A connected between A01 and A02
//motor B connected between B01 and B02

int STBY = 8; //standby

//Motor A
int AIN1 = 3; //Direction
int AIN2 = 4; //Direction
int PWMA = 5; //Speed control 

//Motor B
int BIN1 = 12; //Direction
int BIN2 = 13; //Direction
int PWMB = 11; //Speed control

  #define LEDPIN     10                // número del pin de test
  
  int RECV_PIN = 2;   //pin 2 of arduino to data pin of ir receiver
  IRrecv irrecv(RECV_PIN);
  decode_results results;
  
void setup(){
  pinMode(STBY, OUTPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}
boolean ok=false;
void loop()
{
  if (irrecv.decode(&results))
    {
      Serial.println(results.value, DEC);
      irrecv.resume(); // Receive the next value
      ok=true;
    }
  //Inainte
  if((results.value==924466310) && (ok==true))
    {
      move(1, 255, 1); //motor 1, full speed, left
      move(0, 255, 1); //motor 2, full speed, left
      delay(150); //go for 1 second
      stop(); //stop
      ok=false;
    }
  //Inapoi
  if((results.value==970202566) && (ok==true))
    {
      move(1, 255, 0); //motor 1, full speed, right
      move(0, 255, 0); //motor 2, full speed, right
      delay(150); //go for 1 second
      stop(); //stop
      ok=false;            
    }
  //Stanga  
  if((results.value==3768077238) && (ok==true))
    {
      move(1, 255, 0); //motor 1, full speed, left
      move(0, 255, 1); //motor 2, full speed, right  
      delay(10); //go for 1 second
      stop();
      ok=false;
    }
  //Dreapta
  if((results.value==2737486129) && (ok==true))
    {
      move(1, 255, 1); //motor 1, full speed, right
      move(0, 255, 0); //motor 2, full speed, left
      delay(10); //go for 1 second
      stop();
      ok=false;    
    }
  //STOP
  if(results.value==338831067)
    {
      stop();      
    }    
  
}


void move(int motor, int speed, int direction){
//Move specific motor at speed and direction
//motor: 0 for B 
//       1 for A
//speed: 0 is off, 
//       and 255 is full speed
//direction: 0 clockwise, 
//           1 counter-clockwise

  digitalWrite(STBY, HIGH); //disable standby

  boolean inPin1 = LOW;
  boolean inPin2 = HIGH;

  if(direction == 1){
    inPin1 = HIGH;
    inPin2 = LOW;
  }

  if(motor == 1){
    digitalWrite(AIN1, inPin1);
    digitalWrite(AIN2, inPin2);
    analogWrite(PWMA, speed);
  }else{
    digitalWrite(BIN1, inPin1);
    digitalWrite(BIN2, inPin2);
    analogWrite(PWMB, speed);
  }
}

void stop(){
//enable standby  
  digitalWrite(STBY, LOW); 
}

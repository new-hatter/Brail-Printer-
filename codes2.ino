#include <Servo.h>
#include <Stepper.h>
#include <SD.h>        
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define STEP_PINY         9
#define DIR_PINY          8
#define STEP_PINX         6
#define DIR_PINX          7


#define move_forward_time 3000    
#define move_back_time 3000       
#define pause_time 4000           
#define frequency 2250
#define move_delay 500



int x = 0, y = 0;

Servo myservo;
Stepper sty(200, 7, 6);
Stepper stx(200, 8, 9);

#define motorInterfaceType 1
int wrd[3][16] {
  {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
  {1,1,0,1,1,0,0,1,0,0,1,0,1,0,0,0},
  {0,0,0,1,0,0,0,0,1,1,1,0,0,0,0,1}
  };

void setup() {
  //digitalWrite(STEP_PINX, HIGH);
  ///digitalWrite(STEP_PINY, HIGH);
  //digitalWrite(DIR_PINX, LOW);
  //digitalWrite(DIR_PINY, LOW);

  Serial.begin(9600);

  //pinMode(STEP_PINX   , OUTPUT);     

  //pinMode(DIR_PINX    , OUTPUT);
              
  //pinMode(STEP_PINY   , OUTPUT);     

  //pinMode(DIR_PINY    , OUTPUT);

  myservo.attach(1);
  stx.setSpeed(1000);
  sty.setSpeed(1000);
}

void loop() {
  int m = 0;
  int l = 0;
  int x = 0;
  int y = 0;
  for (int i=0 ; i<3; i++) {
    for (int j=0; j<16; j++) {
      if (wrd[i][j]) {
        setPoint(i, j);
        Serial.print("Point at ");
        Serial.print(i);
        Serial.print(", ");
        Serial.println(j);
        delay(300);
       
       
        myservo.write(60);
        delay(500);
        myservo.write(0);
        delay(500);  
        // for(int k = 0; k < (j-l)*5; k++) {
        //     digitalWrite(STEP_PINX, HIGH);
        //     delay(move_delay);
        //     digitalWrite(STEP_PINX, LOW);
        //     delay(move_delay);
        //   }
          
        m = i;
        l = j;
        Serial.println(m,l);
      }  
    }
    /*for(int n = 0; n < 5; n++){
      digitalWrite(STEP_PINY, HIGH);
      delay(move_delay);
      digitalWrite(STEP_PINY, LOW);
      delay(move_delay);

    }*/
  }
}

void setPoint(int xtg, int ytg) {
  int dx = x + xtg, dy = y + ytg;
  x = x + dx;
  y = y + dy;
  stx.step(dx*25);
  sty.step(dy*25);
  

}

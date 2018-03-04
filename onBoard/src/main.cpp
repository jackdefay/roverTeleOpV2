#include <Arduino.h>

#define JOYSTICK_RANGE 1023

//motor direction pins
#define rfpos 11
#define rfneg 10
#define rbpos 9
#define rbneg 6
#define lfpos 5
#define lfneg A5
#define lbpos A2
#define lbneg A1

//motor speed pins
#define pwmrf 13
#define pwmrb 12
#define pwmlf A4
#define pwmlb A3

void setDirection(char motor, bool direction);
void setSpeed(int pwmr, int pwml);
int clip(int num);

void setup() {
  Serial.begin(115200);

  Serial.println("starting");

  //motor pins
  pinMode(rfpos, OUTPUT);
  pinMode(rfneg, OUTPUT);
  pinMode(rbpos, OUTPUT);
  pinMode(rbneg, OUTPUT);
  pinMode(lfpos, OUTPUT);
  pinMode(lfneg, OUTPUT);
  pinMode(lbpos, OUTPUT);
  pinMode(lbneg, OUTPUT);
  pinMode(pwmrf, OUTPUT);
  pinMode(pwmrb, OUTPUT);
  pinMode(pwmlf, OUTPUT);
  pinMode(pwmlb, OUTPUT);

  //set motors to forwards
  setDirection('r', true);
  setDirection('l', true);
}

void loop(){
  setSpeed(255, 255);
}

void setDirection(char motor, bool direction){  //1 = forwards, 0 = backwards
  Serial.print("setting direction to "); Serial.println(direction);
  if(motor == 'r'){
    digitalWrite(rfpos, (int) direction);
    digitalWrite(rfneg, (int) !direction);
    digitalWrite(rbpos, (int) direction);
    digitalWrite(rbneg, (int) !direction);
  }
  else if(motor == 'l'){                        //the left wheels are reversed
    digitalWrite(lfpos, (int) !direction);
    digitalWrite(lfneg, (int) direction);
    digitalWrite(lbpos, (int) !direction);
    digitalWrite(lbneg, (int) direction);
  }
}

void setSpeed(int pwmr, int pwml){
  if(pwmr>=0) setDirection('r', true);
  else setDirection('r', false);

  if(pwml>=0) setDirection('l', true);
  else setDirection('l', false);

  pwmr = clip(pwmr);
  pwml = clip(pwml);

  Serial.print("the values being written to the motors are: "); Serial.print(pwmr); Serial.print(", "); Serial.println(pwml);

  analogWrite(pwmrf, pwmr);
  analogWrite(pwmrb, pwmr);
  analogWrite(pwmlf, pwml);
  analogWrite(pwmlb, pwml);
}

int clip(int num){
  if(num>=-100 && num<=100) return 0;
  if(num>=0 && num<=255) return num;
  else if(num>=-255 && num<0) return -num;
  else if(num>255 || num<-255) return 255;
  else return 0;
}

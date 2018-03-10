#include <Arduino.h>
#include <SPI.h>
#include <RH_RF69.h>
#include <Wire.h>

#define JOYSTICK_RANGE 1023

#define RF69_FREQ 900.0

//radio pins
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
#define LED           13

RH_RF69 rf69(RFM69_CS, RFM69_INT);

// ultrasonic pins
#define trigPin 5  //output
#define echoPin 6  //input

int getUltrasonicDistance();
void sendLevel(int level);
void writeSpeed(int rightSpeed, int leftSpeed);

void setup() {
    Serial.begin(115200);

    Wire.begin(); // join i2c bus (address optional for master)

    pinMode(LED, OUTPUT);
    pinMode(RFM69_RST, OUTPUT);
    digitalWrite(RFM69_RST, LOW);

      // manual reset
    digitalWrite(RFM69_RST, HIGH);
    delay(10);
    digitalWrite(RFM69_RST, LOW);
    delay(10);

    if (!rf69.init()) {
      //Serial.println("RFM69 radio init failed");
      while (1);
    }
    Serial.println("RFM69 radio init OK!");
    if (!rf69.setFrequency(RF69_FREQ)) {
      //Serial.println("setFrequency failed");
    }
    rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
    uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    rf69.setEncryptionKey(key);

    pinMode(LED, OUTPUT);

    //ultrasonic pins
    // pinMode(trigPin, OUTPUT);
    // pinMode(echoPin, INPUT);
}

void loop() {
  String word, xcoord, ycoord;
  char temp[20];
  int i = 0, xcoordint, ycoordint;//, pwmr = 0, pwml = 0;
  static unsigned long previousMillis = 0, currentMillis = 0;
  int level;

  if (rf69.available()) {
      uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      if (rf69.recv(buf, &len)) {
        if (!len) return;
        buf[len] = 0;
        Serial.println((char*)buf);
        for(int i = 0; i<20; i++){
          temp[i] = (char) buf[i];
        }
        word = temp;
        i = 0;
        while(word.charAt(i) != '*'){
          if((word.charAt(i) >= '0' && word.charAt(i) <= '9') || word.charAt(i) == '-') xcoord += word.charAt(i);
          i++;
        }
        i++;
        while(word.charAt(i) != '*'){
          if((word.charAt(i) >= '0' && word.charAt(i) <= '9') || word.charAt(i) == '-') ycoord += word.charAt(i);
          i++;
        }

        xcoordint = (int) xcoord.toInt();
        ycoordint = (int) ycoord.toInt();

        //Serial.print(xcoordint); Serial.print(", "); Serial.println(ycoordint);

        writeSpeed(xcoordint, ycoordint);
        Serial.print(xcoordint); Serial.print(", "); Serial.println(ycoordint);

        //after setting the speed of the motors, sends ultrasonic data back for haptics
        level = getUltrasonicDistance();
        sendLevel(level);

        Serial.println(level);

        previousMillis = millis();  //reset timer

        digitalWrite(13, HIGH);
      }

      else {
        Serial.println("Receive failed");
      }
   }
   else {
     digitalWrite(13, LOW);
     Serial.println("no signal");
   }

   currentMillis = millis();
   if(currentMillis - previousMillis > 100) writeSpeed(0, 0);
   delay(10);                                                  //change this after done testing
}

void writeSpeed(int rightSpeed, int leftSpeed){
  Wire.beginTransmission(8);  // transmit to device #8
  Wire.write('*');            // send indicator
  if(rightSpeed>=0) Wire.write(1);
  else{
    Wire.write(0);
    rightSpeed *= -1;
  }
  Wire.write((byte) rightSpeed);     // sends rightSpeed
  if(leftSpeed>=0) Wire.write(1);
  else{
    Wire.write(0);
    leftSpeed *= -1;
  }
  Wire.write((byte) leftSpeed);      // sends LeftSpeed
  Wire.endTransmission();     // stop transmitting
}

int getUltrasonicDistance(){
  long duration;
  double distanceCm;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  distanceCm = (double) duration * 0.01715;

  if(distanceCm < 20) return 5;
  else if(distanceCm < 40) return 4;
  else if(distanceCm < 60) return 3;
  else if(distanceCm < 80) return 2;
  else if(distanceCm < 100) return 1;
  else return 0;
}

void sendLevel(int level){
  char radiopacket[20];
  char temp[5];
  String tempWord = "####";

  //Serial.print(x); Serial.print(", "); Serial.println(y);
  itoa((int) level, temp, 10);
  tempWord = temp;
  tempWord += "* ";
  tempWord.toCharArray(radiopacket, 20);
  //Serial.println(radiopacket);
  rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
  rf69.waitPacketSent();
}

/*
   vive510
     class for tracking multiple diodes under a vive basestation (1.0)

   use timer interrupt to measure pulse width and between pulses,
   updates vive.xCoord and vive.yCoord return x,y coordinates that are updated asynchronously 60 times per sec
   range X (up/down): 1400 to 7000 may vary unit to unit - roughly 120 deg

   range Y (left/right): 1000 to 6700 may vary unit to unit - roughly 120 deg
   distance reliable to 10'
*/



#include "vive510.h"
#include "motorControl.h"  // contains string "body" html code
#include "html510.h"




#define ledcPinFrontRight1 23
#define ledcPinFrontRight2 19
#define ledcPinRearRight1 21
#define ledcPinRearRight2 22
#define ledcPinFrontLeft1 33
#define ledcPinFrontLeft2 32
#define ledcPinRearLeft1 26
#define ledcPinRearLeft2 25
#define ledcChFrontRight1 0
#define ledcChFrontRight2 1
#define ledcChFrontLeft1  2
#define ledcChFrontLeft2  3
#define ledcChRearRight1  4
#define ledcChRearRight2  5
#define ledcChRearLeft1   6
#define ledcChRearLeft2   7
#define ledcResolutionBit 15// 
#define ledcResolution ((1<<ledcResolutionBit)-1)
#define frequency 5
#define SIGNALPIN1 27 // pin receiving signal from Vive circuit

uint16_t dutyCycle = 1 / 2 *  ledcResolution;
const uint16_t dutyCycle0 = 0 * ledcResolution;
const uint16_t dutyCycle100 = 1 * ledcResolution;
int16_t xcoordTarget;
int16_t ycoordTarget;


//***************************************************************************Wifi and UDP Receiver*********************************************************************************
IPAddress myIPaddress(192, 168, 1, 169); // change to your IP
const char* ssid     = "TP-Link_05AF";
const char* password = "47543454";

WiFiUDP canUDPServer;
WiFiUDP robotUDPServer;
WiFiUDP UDPTestServer;
unsigned int UDPPort = 2510; // port for cans is 1510, port for robots is 2510
IPAddress ipTarget(192, 168, 1, 255); // 255 is a broadcast address to everyone at 192.168.1.xxx
IPAddress ipLocal(192, 168, 1, 169);  // replace with your IP address

void fncUdpSend(char *datastr, int len)
{
  UDPTestServer.beginPacket(ipTarget, UDPPort);
  UDPTestServer.write((uint8_t *)datastr, len);
  UDPTestServer.endPacket();
}
//***************************************************************************Wifi and UDP Receiver*********************************************************************************




//***************************************************************************Set Robot ID*********************************************************************************
#define idPin1 37
#define idPin2 38
#define idPin3 34
#define idPin4 35
int robotID;
void setRobotID() {
  if (digitalRead(idPin1) == HIGH && digitalRead(idPin2) == LOW && digitalRead(idPin3) == LOW && digitalRead(idPin4) == LOW) {
    robotID = 1;
  }
  else if (digitalRead(idPin1) == LOW && digitalRead(idPin2) == HIGH && digitalRead(idPin3) == LOW && digitalRead(idPin4) == LOW) {
    robotID = 2;
  }
  else if (digitalRead(idPin1) == LOW && digitalRead(idPin2) == LOW && digitalRead(idPin3) == HIGH && digitalRead(idPin4) == LOW) {
    robotID = 3;
  }
  else if (digitalRead(idPin1) == LOW && digitalRead(idPin2) == LOW && digitalRead(idPin3) == LOW && digitalRead(idPin4) == HIGH) {
    robotID = 4;
  }
  else {
    robotID = 1;
  }
}
//***************************************************************************Set Robot ID*********************************************************************************


//***************************************************************************Wall Following*********************************************************************************
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
long duration1;
float distanceCm1;
long duration2;
float distanceCm2;
const int trigPin1 = 9;
const int echoPin1 = 10;
const int trigPin2 = 5;
const int echoPin2 = 18;

int wallFollowingFlag = 1;
long right_sensor()
{
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distanceCm1 = duration1 * SOUND_SPEED / 2;
  Serial.print("Distance right(cm): ");
  Serial.println(distanceCm1);
  return distanceCm1;
}
long front_sensor()
{
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distanceCm2 = duration2 * SOUND_SPEED / 2;
  Serial.print("Distance front(cm): ");
  Serial.println(distanceCm2);
  return distanceCm2;
}
void handleWallFollowing() {
  int counter = 0;
  int corner_counter = 0;
  while (corner_counter < 5) {
    Serial.println("followwwwww");
    if (counter == 0)
    {
      ledcWrite(ledcChFrontRight1, dutyCycle0); //rught
      ledcWrite(ledcChFrontRight2, ledcResolution);
      ledcWrite(ledcChRearLeft1, dutyCycle0);
      ledcWrite(ledcChRearLeft2, ledcResolution);
      ledcWrite(ledcChFrontLeft1, ledcResolution);
      ledcWrite(ledcChFrontLeft2, dutyCycle0);
      ledcWrite(ledcChRearRight1, ledcResolution);
      ledcWrite(ledcChRearRight2, dutyCycle0);
      delay(800);
      ledcWrite(ledcChFrontRight1, ledcResolution); //left
      ledcWrite(ledcChFrontRight2, dutyCycle0);
      ledcWrite(ledcChRearLeft1, ledcResolution);
      ledcWrite(ledcChRearLeft2, dutyCycle0);
      ledcWrite(ledcChFrontLeft1, dutyCycle0);
      ledcWrite(ledcChFrontLeft2, ledcResolution);
      ledcWrite(ledcChRearRight1, dutyCycle0);
      ledcWrite(ledcChRearRight2, ledcResolution);
      delay(800);
      counter = 1;
    }
    if (right_sensor() <= 15)
    {
      dutyCycle = ledcResolution;
      ledcWrite(ledcChFrontRight1, dutyCycle);
      ledcWrite(ledcChFrontRight2, dutyCycle0);
      ledcWrite(ledcChRearLeft1, dutyCycle);
      ledcWrite(ledcChRearLeft2, dutyCycle0);
      ledcWrite(ledcChFrontLeft1, dutyCycle0);
      ledcWrite(ledcChFrontLeft2, dutyCycle);
      ledcWrite(ledcChRearRight1, dutyCycle0);
      ledcWrite(ledcChRearRight2, dutyCycle);
    }
    if (right_sensor() >= 23)
    {
      dutyCycle = ledcResolution;
      ledcWrite(ledcChFrontRight1, dutyCycle0);
      ledcWrite(ledcChFrontRight2, dutyCycle);
      ledcWrite(ledcChRearLeft1, dutyCycle0);
      ledcWrite(ledcChRearLeft2, dutyCycle);
      ledcWrite(ledcChFrontLeft1, dutyCycle);
      ledcWrite(ledcChFrontLeft2, dutyCycle0);
      ledcWrite(ledcChRearRight1, dutyCycle);
      ledcWrite(ledcChRearRight2, dutyCycle0);
    }
    if (right_sensor() >= 15 && right_sensor() <= 23)
    {
      ledcWrite(ledcChFrontRight1, ledcResolution); //forward
      ledcWrite(ledcChFrontRight2, dutyCycle0);
      ledcWrite(ledcChRearLeft1, ledcResolution);
      ledcWrite(ledcChRearLeft2, dutyCycle0);
      ledcWrite(ledcChFrontLeft1, ledcResolution);
      ledcWrite(ledcChFrontLeft2, dutyCycle0);
      ledcWrite(ledcChRearRight1, ledcResolution);
      ledcWrite(ledcChRearRight2, dutyCycle0);
    }

    if (front_sensor() <= 10)
    {
      ledcWrite(ledcChFrontRight1, ledcResolution);
      ledcWrite(ledcChFrontRight2, dutyCycle0);
      ledcWrite(ledcChRearLeft1, dutyCycle0);
      ledcWrite(ledcChRearLeft2, ledcResolution);
      ledcWrite(ledcChFrontLeft1, dutyCycle0);
      ledcWrite(ledcChFrontLeft2, ledcResolution);
      ledcWrite(ledcChRearRight1, ledcResolution);
      ledcWrite(ledcChRearRight2, dutyCycle0); //left turn
      delay(800);
      ledcWrite(ledcChFrontRight1, dutyCycle0); //rught
      ledcWrite(ledcChFrontRight2, ledcResolution);
      ledcWrite(ledcChRearLeft1, dutyCycle0);
      ledcWrite(ledcChRearLeft2, ledcResolution);
      ledcWrite(ledcChFrontLeft1, ledcResolution);
      ledcWrite(ledcChFrontLeft2, dutyCycle0);
      ledcWrite(ledcChRearRight1, ledcResolution);
      ledcWrite(ledcChRearRight2, dutyCycle0);
      delay(1000);
      ledcWrite(ledcChFrontRight1, ledcResolution); //left
      ledcWrite(ledcChFrontRight2, dutyCycle0);
      ledcWrite(ledcChRearLeft1, ledcResolution);
      ledcWrite(ledcChRearLeft2, dutyCycle0);
      ledcWrite(ledcChFrontLeft1, dutyCycle0);
      ledcWrite(ledcChFrontLeft2, ledcResolution);
      ledcWrite(ledcChRearRight1, dutyCycle0);
      ledcWrite(ledcChRearRight2, ledcResolution);
      delay(800);
      corner_counter++;
      Serial.println(corner_counter);
    }
  }
  handleMouseUp();
}
//***************************************************************************Wall Following*********************************************************************************



//***************************************************************************Beacon Tracking*********************************************************************************
#define leftPin 13
#define rightPin 4
//#define timeout_ms 5000
//unsigned long search_time_start = 0;
uint32_t leftFrequency;
uint32_t rightFrequency;
volatile uint32_t leftFallingEdgeTime;
volatile uint32_t oldLeftFallingEdgeTime;
volatile uint32_t rightFallingEdgeTime;
volatile uint32_t oldRightFallingEdgeTime;

void IRAM_ATTR handleLeftLEDInterrupt() {
  leftFallingEdgeTime = micros();
  if (leftFallingEdgeTime != oldLeftFallingEdgeTime) {
    leftFrequency = 1000000 / (leftFallingEdgeTime - oldLeftFallingEdgeTime); //converting to seconds and then to Hz
    oldLeftFallingEdgeTime = leftFallingEdgeTime;
  }
}

void IRAM_ATTR handleRightLEDInterrupt() {
  rightFallingEdgeTime = micros();
  if (rightFallingEdgeTime != oldRightFallingEdgeTime) {
    rightFrequency = 1000000 / (rightFallingEdgeTime - oldRightFallingEdgeTime); //converting to seconds and then to Hz
    oldRightFallingEdgeTime = rightFallingEdgeTime;
  }
}
void handleBeacon23() {
  pinMode(leftPin, INPUT_PULLUP); //no use of pullup or pulldown resistor
  pinMode(rightPin, INPUT_PULLUP); //no use of pullup or pulldown resistor

  attachInterrupt(digitalPinToInterrupt(leftPin), handleLeftLEDInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(rightPin), handleRightLEDInterrupt, FALLING);

  int beaconFlag = 1;
  while (beaconFlag) {
    //    Serial.printf("The left frequency is %d\n", leftFrequency);
    //    Serial.printf("The right frequency is %d\n", rightFrequency);
    //    delay(500);
    if (leftFrequency < 25 && leftFrequency > 20) {
      dutyCycle = ledcResolution;
      ledcWrite(ledcChFrontRight1, dutyCycle);
      ledcWrite(ledcChFrontRight2, dutyCycle0);
      ledcWrite(ledcChRearLeft1, dutyCycle);
      ledcWrite(ledcChRearLeft2, dutyCycle0);
      ledcWrite(ledcChFrontLeft1, dutyCycle);
      ledcWrite(ledcChFrontLeft2, dutyCycle0);
      ledcWrite(ledcChRearRight1, dutyCycle);
      ledcWrite(ledcChRearRight2, dutyCycle0);
      delay(1500);
      Serial.printf("GOOOOOOOOOOOOOOOOOOOOO");
    }
    else {
      dutyCycle = 0.30 * ledcResolution;
      ledcWrite(ledcChFrontRight1, dutyCycle0);
      ledcWrite(ledcChFrontRight2, dutyCycle);
      ledcWrite(ledcChRearLeft1, dutyCycle);
      ledcWrite(ledcChRearLeft2, dutyCycle0);
      ledcWrite(ledcChFrontLeft1, dutyCycle);
      ledcWrite(ledcChFrontLeft2, dutyCycle0);
      ledcWrite(ledcChRearRight1, dutyCycle0);
      ledcWrite(ledcChRearRight2, dutyCycle);

      Serial.println("turnnnnnnnnnnnnnnnnnnnnnnnnn");
    }
  }
}
//***************************************************************************Beacon Tracking*********************************************************************************


Vive510 vive1(SIGNALPIN1);
HTML510Server h(80);

/*****************/
/* web handler   */
void handleRoot() {
  h.sendhtml(body);
}


void handleMouseDownForward() {
  ledcWrite(ledcChFrontRight1, dutyCycle);
  ledcWrite(ledcChFrontRight2, dutyCycle0);
  ledcWrite(ledcChRearLeft1, dutyCycle);
  ledcWrite(ledcChRearLeft2, dutyCycle0);
  ledcWrite(ledcChFrontLeft1, dutyCycle);
  ledcWrite(ledcChFrontLeft2, dutyCycle0);
  ledcWrite(ledcChRearRight1, dutyCycle);
  ledcWrite(ledcChRearRight2, dutyCycle0);
  h.sendplain(""); // acknowledge         
}
void handleMouseDownBackward() {
  ledcWrite(ledcChFrontRight1, dutyCycle0);
  ledcWrite(ledcChFrontRight2, dutyCycle);
  ledcWrite(ledcChRearLeft1, dutyCycle0);
  ledcWrite(ledcChRearLeft2, dutyCycle);
  ledcWrite(ledcChFrontLeft1, dutyCycle0);
  ledcWrite(ledcChFrontLeft2, dutyCycle);
  ledcWrite(ledcChRearRight1, dutyCycle0);
  ledcWrite(ledcChRearRight2, dutyCycle);
  h.sendplain(""); // acknowledge         
}

void handleMouseDownLeft() {
  ledcWrite(ledcChFrontRight1, dutyCycle);
  ledcWrite(ledcChFrontRight2, dutyCycle0);
  ledcWrite(ledcChRearLeft1, dutyCycle);
  ledcWrite(ledcChRearLeft2, dutyCycle0);
  ledcWrite(ledcChFrontLeft1, dutyCycle0);
  ledcWrite(ledcChFrontLeft2, dutyCycle);
  ledcWrite(ledcChRearRight1, dutyCycle0);
  ledcWrite(ledcChRearRight2, dutyCycle);
  h.sendplain(""); // acknowledge         
}

void handleMouseDownRight() {
  ledcWrite(ledcChFrontRight1, dutyCycle0);
  ledcWrite(ledcChFrontRight2, dutyCycle);
  ledcWrite(ledcChRearLeft1, dutyCycle0);
  ledcWrite(ledcChRearLeft2, dutyCycle);
  ledcWrite(ledcChFrontLeft1, dutyCycle);
  ledcWrite(ledcChFrontLeft2, dutyCycle0);
  ledcWrite(ledcChRearRight1, dutyCycle);
  ledcWrite(ledcChRearRight2, dutyCycle0);
  h.sendplain(""); // acknowledge         
}

void handleMouseDownLeftTurn() {
  ledcWrite(ledcChFrontRight1, dutyCycle);
  ledcWrite(ledcChFrontRight2, dutyCycle0);
  ledcWrite(ledcChRearLeft1, dutyCycle0);
  ledcWrite(ledcChRearLeft2, dutyCycle);
  ledcWrite(ledcChFrontLeft1, dutyCycle0);
  ledcWrite(ledcChFrontLeft2, dutyCycle);
  ledcWrite(ledcChRearRight1, dutyCycle);
  ledcWrite(ledcChRearRight2, dutyCycle0);
  h.sendplain(""); // acknowledge         
}

void handleMouseDownRightTurn() {
  ledcWrite(ledcChFrontRight1, dutyCycle0);
  ledcWrite(ledcChFrontRight2, dutyCycle);
  ledcWrite(ledcChRearLeft1, dutyCycle);
  ledcWrite(ledcChRearLeft2, dutyCycle0);
  ledcWrite(ledcChFrontLeft1, dutyCycle);
  ledcWrite(ledcChFrontLeft2, dutyCycle0);
  ledcWrite(ledcChRearRight1, dutyCycle0);
  ledcWrite(ledcChRearRight2, dutyCycle);
  h.sendplain(""); // acknowledge         
}
void handleMouseUp() {
  ledcWrite(ledcChFrontRight1, dutyCycle0);
  ledcWrite(ledcChFrontRight2, dutyCycle0);
  ledcWrite(ledcChRearLeft1, dutyCycle0);
  ledcWrite(ledcChRearLeft2, dutyCycle0);
  ledcWrite(ledcChFrontLeft1, dutyCycle0);
  ledcWrite(ledcChFrontLeft2, dutyCycle0);
  ledcWrite(ledcChRearRight1, dutyCycle0);
  ledcWrite(ledcChRearRight2, dutyCycle0);
  h.sendplain(""); // acknowledge         
}

void handleXcoord() {
  xcoordTarget =  h.getVal();
  String s = "X-Coord: ";
  s = s + xcoordTarget;
  h.sendplain(s);
}

void handleYcoord() {
  ycoordTarget = h.getVal();
  String s = "Y-Coord: ";
  s = s + ycoordTarget;
  h.sendplain(s);
}

void handleSliderSpeed() {
  String s = "Duty Cycle: ";
  int value = h.getVal();
  dutyCycle = ledcResolution * value / 100.0;
  s = s + value + "%";
  h.sendplain(s);
}

void handleStartNavigation() {
  String s = "Autonomous Navigation Starts";
  uint16_t xcoordFlag = 0;
  uint16_t ycoordFlag = 0;
  do {
    if ((vive1.yCoord() - ycoordTarget) > 100) {
      dutyCycle = 0.60 * ledcResolution;
      handleMouseDownLeft();
    }
    else if ((vive1.yCoord() - ycoordTarget) < -100) {
      dutyCycle = 0.60 * ledcResolution;
      handleMouseDownRight();
    }
    else {
      handleMouseUp();
      ycoordFlag = 1;
    }
  } while (!ycoordFlag);
  handleCalibrate();
  do {
    if ( xcoordTarget - (vive1.xCoord()) > 100) {
      dutyCycle = ledcResolution;
      handleMouseDownForward();
    }
    else {
      handleMouseUp();
      xcoordFlag = 1;
    }
  } while (!xcoordFlag);
}

//void handleStartNavigation() {
//  String s = "Autonomous Navigation Starts";
//  uint16_t xcoordFlag = 0;
//  uint16_t ycoordFlag = 0;
//  do {
//    if ((vive1.xCoord() - xcoordTarget) > 50) {
//      dutyCycle = 0.60 * ledcResolution;
//      handleMouseDownLeft();
//    }
//    else if ((vive1.xCoord() - xcoordTarget) < -50) {
//      dutyCycle = 0.60 * ledcResolution;
//      handleMouseDownRight();
//    }
//    else {
//      handleMouseUp();
//      xcoordFlag = 1;
//    }
//  } while (!xcoordFlag);
////  handleCalibrate();
//  do {
//    if ( ycoordTarget - (vive1.yCoord()) > 50) {
//      dutyCycle = ledcResolution;
//      handleMouseDownForward();
//    }
//    else {
//      handleMouseUp();
//      ycoordFlag = 1;
//    }
//  } while (!ycoordFlag);
//}

void handleCalibrate() {
  int facingForward = 0;
  do {
    int16_t ycoord_prev = vive1.yCoord();
    dutyCycle = ledcResolution;
    handleMouseDownForward();
    delay(500);
    handleMouseUp();
    int16_t ycoord = vive1.yCoord();
    int16_t ycoord_diff = ycoord - ycoord_prev;
//    Serial.println(xcoord_diff);
    if (ycoord_diff > 35) {
      dutyCycle = 15.0 / 100.0 * ledcResolution;
      handleMouseDownLeftTurn();
      delay(250);
      handleMouseUp();
    }
    else if (ycoord_diff < -35) {
      dutyCycle = 15.0 / 100.0 * ledcResolution;
      handleMouseDownRightTurn();
      delay(250);
      handleMouseUp();
    }
    else {
      facingForward = 1;
    }
  } while (!facingForward);
}

void handleUpdateX() {
  h.sendplain(String(vive1.xCoord()));
}

void handleUpdateY() {
  h.sendplain(String(vive1.yCoord()));
}



void setup() {
  Serial.begin(115200);
  Serial.println("Vive trackers started");

  pinMode(idPin1, INPUT_PULLUP);
  pinMode(idPin2, INPUT_PULLUP);
  pinMode(idPin3, INPUT_PULLUP);
  pinMode(idPin4, INPUT_PULLUP);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  WiFi.config(ipLocal, IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  WiFi.begin(ssid, password);
  UDPTestServer.begin(UDPPort);
  canUDPServer.begin(1510); // can port 1510
  robotUDPServer.begin(2510); // robot port 2510

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  vive1.begin();

  ledcSetup(ledcChFrontRight1, frequency, ledcResolutionBit);
  ledcSetup(ledcChFrontRight2, frequency, ledcResolutionBit);
  ledcSetup(ledcChFrontLeft1, frequency, ledcResolutionBit);
  ledcSetup(ledcChFrontLeft2, frequency, ledcResolutionBit);
  ledcSetup(ledcChRearRight1, frequency, ledcResolutionBit);
  ledcSetup(ledcChRearRight2, frequency, ledcResolutionBit);
  ledcSetup(ledcChRearLeft1, frequency, ledcResolutionBit);
  ledcSetup(ledcChRearLeft2, frequency, ledcResolutionBit);
  ledcAttachPin(ledcPinFrontRight1, ledcChFrontRight1);
  ledcAttachPin(ledcPinFrontRight2, ledcChFrontRight2);
  ledcAttachPin(ledcPinFrontLeft1, ledcChFrontLeft1);
  ledcAttachPin(ledcPinFrontLeft2, ledcChFrontLeft2);
  ledcAttachPin(ledcPinRearRight1, ledcChRearRight1);
  ledcAttachPin(ledcPinRearRight2, ledcChRearRight2);
  ledcAttachPin(ledcPinRearLeft1, ledcChRearLeft1);
  ledcAttachPin(ledcPinRearLeft2, ledcChRearLeft2);

  h.begin();
  h.attachHandler("/ ", handleRoot);
  h.attachHandler("/mouseDownForward", handleMouseDownForward);
  h.attachHandler("/mouseDownBackward", handleMouseDownBackward);
  h.attachHandler("/mouseDownRight", handleMouseDownRight);
  h.attachHandler("/mouseDownLeft", handleMouseDownLeft);
  h.attachHandler("/mouseDownLeftTurn", handleMouseDownLeftTurn);
  h.attachHandler("/mouseDownRightTurn", handleMouseDownRightTurn);
  h.attachHandler("/mouseUp", handleMouseUp);
  h.attachHandler("/sliderSpeed?val=", handleSliderSpeed);
  h.attachHandler("/xcoord?val=", handleXcoord);
  h.attachHandler("/ycoord?val=", handleYcoord);
  h.attachHandler("/startNavigation", handleStartNavigation);
  h.attachHandler("/updateX", handleUpdateX);
  h.attachHandler("/updateY", handleUpdateY);
  h.attachHandler("/calibrate", handleCalibrate);
  h.attachHandler("/wallFollowing", handleWallFollowing);
  h.attachHandler("/beacon23", handleBeacon23);
}
char s[13];
void loop() {
  h.serve();

  if (vive1.status() == VIVE_LOCKEDON) {
        Serial.printf("X %d, Y %d\n", vive1.xCoord(), vive1.yCoord());
        //delay(100);
  }
  else
    vive1.sync(15); // try to resync (nonblocking);
  //  Serial.println("vive not locked on");
  //  delay(100);

  int16_t x = vive1.xCoord();
  int16_t y = vive1.yCoord(); // some data, like  xy position

  setRobotID();
  // store into a string with format #:####,####, which is robotid, x, y
  sprintf(s, "%1d:%4d,%4d", robotID, x, y);
  fncUdpSend(s, 13);
  //Serial.printf("sending data: %s", s);
  delay(500);
}

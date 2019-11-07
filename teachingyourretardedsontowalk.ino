#include <MeMCore.h>

#define LIGHT_SENSOR A6
#define LEFT_IR A2
#define RIGHT_IR A3
#define ULTRASONIC 12
#define LOW_PIN A0
#define HIGH_PIN A1

#define TIMEOUT 15000
#define MIN_DISTANCE 700
#define MAX_SPEED 100
#define FULL_LEFT 900
#define FULL_RIGHT 900
#define UTURN 1800
#define ONE_CELL 2500
#define AVOID_WALL 60
//
#define MAX 150
#define DELAY_TIME 500
#define NUM_READINGS 3
#define TOL 30

void measureRGBraw(int []);
void printArr (int []);
char determineColor (int []);
void colorCommand (char);

// Initialize sensors
MeLightSensor lightSensor(PORT_6);
MeRGBLed led(0,30);


// Initialize both motors
MeDCMotor motor1(M1); //Right forwards
MeDCMotor motor2(M2); //Left backwards
MeBuzzer buzzer;
MeLineFollower lineFinder(PORT_2);

int colorArr[3] = {0};
double inputLeft, inputRight;
int sensorState;
char inputColour;

void setup() {
  Serial.begin(9600);
  led.setpin(13);
  
  led.setColorAt(0,0,0,0);
  led.setColorAt(1,0,0,0);
  led.show();                                                                                                     
}

void loop() {
  delay(40);
  Serial.println(analogRead(RIGHT_IR));
  inputLeft = analogRead(LEFT_IR);
  inputRight = analogRead(RIGHT_IR); 

  //
  sensorState = lineFinder.readSensors();
  if (sensorState != S1_OUT_S2_OUT){
    motor1.stop();
    motor2.stop();
    colorChallenge();
  }
  
  if( ultrasonic() <= 7){
    if(inputLeft < inputRight){
      turnRight(UTURN);
    } else if(inputRight <= inputLeft){
      turnLeft(UTURN);
    }
  }
  if(inputRight <= 700 && inputRight > 150){
    turnLeft(AVOID_WALL);    
  }else if(inputLeft <= 700 && inputLeft > 150){
    turnRight(AVOID_WALL);
  }else{
    moveForward();
  }
}

// moves the mBot forward
void moveForward(){
 motor1.run(MAX_SPEED);
 motor2.run(-MAX_SPEED);
}

// moves mBot forward for time t
void moveForward(int t){
  motor1.run(MAX_SPEED);
  motor2.run(-MAX_SPEED);
  delay(t);
  motor1.stop();
  motor2.stop();
}

// turns the mBot left
void turnLeft(int t){
 motor1.run(MAX_SPEED);
 motor2.run(MAX_SPEED);
 delay(t);
}

// turns the mBot right
void turnRight(int t){
 motor1.run(-MAX_SPEED);
 motor2.run(-MAX_SPEED);
 delay(t);
}

//returns distance in cm
long ultrasonic() {
  pinMode(ULTRASONIC, OUTPUT);

  digitalWrite(ULTRASONIC, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC, HIGH);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC, LOW);

  pinMode(ULTRASONIC, INPUT);

  // empirically derived
  return pulseIn(ULTRASONIC, HIGH, TIMEOUT)/ 2 / 28.89;  
}

void colorChallenge(){
  measureRGBraw(colorArr);
  switch(determineColor(colorArr)){
    case 'R':
      turnLeft(FULL_LEFT);
      break;
    case 'G':
      turnRight(FULL_RIGHT);
      break;
    case 'B':
      turnRight(FULL_RIGHT);
      moveForward(ONE_CELL);
      turnRight(FULL_RIGHT);
      break;
    case 'Y':
      turnRight(UTURN);
      break;
    case 'P':
      turnLeft(FULL_LEFT);
      moveForward(ONE_CELL);
      turnLeft(FULL_LEFT);
      break;
    case 'X':
      buzzer.tone(200, 200); //BUZZ a TUNE
      motor1.stop();
      motor2.stop();
      delay(2000);
      break;
    case '$':
      motor1.stop();
      motor2.stop();
      buzzer.tone(2000, 200);
      delay(2000);
      break;
  }
}

void measureRGBraw(int colorArr[]) {
  for (int i=0; i<NUM_READINGS; i++) {
    // RED LED
    led.setColor(MAX, 0, 0);
    led.show();
  
    delay(DELAY_TIME);
  
    colorArr[0] += lightSensor.read();
  
    // GREEN LED
    led.setColorAt(0, 0, MAX, 0);
    led.setColorAt(1, 0, MAX, 0);
    led.show();
  
    delay(DELAY_TIME);
  
    colorArr[1] += lightSensor.read();
  
    // BLUE LED
    led.setColorAt(0, 0, 0, MAX);
    led.setColorAt(1, 0, 0, MAX);
    led.show();
  
    delay(DELAY_TIME);
   
    colorArr[2] += lightSensor.read();
    
  }
    led.setColorAt(0,0,0,0);
    led.setColorAt(1,0,0,0);
    led.show();
    
    colorArr[0]/=NUM_READINGS;
    colorArr[1]/=NUM_READINGS;
    colorArr[2]/=NUM_READINGS;
    
}

// Return 0 for no color detected,
//        X for black,
//        R for red,
//        G for green,
//        B for blue,
//        Y for yellow,
//        P for purple.
//        $ for error.
char determineColor(int colorArr[]) {
  int avg = (colorArr[0]+colorArr[1]+colorArr[2])/3;
  int r = colorArr[0]-avg,
      g = colorArr[1]-avg,
      b = colorArr[2]-avg;

  if (avg>800) return '0';
  else if (avg<250) return 'X';
  else if (173-TOL<r && -80-TOL<g && g<-80+TOL && -21-TOL<b && b<-21+TOL) return 'R';
  else if (19-TOL<r  && r<19+TOL  && 5-TOL<g   && g<5+TOL   && -23-TOL<b  && b<-23+TOL  ) 
    return 'G';
  else if (8-TOL<r   && r<8+TOL   && -19-TOL<g && g<-19+TOL && 12-TOL<b   && b<12+TOL   ) 
    return 'B';
  else if (134-TOL<r && r<134+TOL && -29-TOL<g && g<-36+TOL && -105-TOL<b && b<-105+TOL ) 
    return 'Y';
  else if (61-TOL<r  && r<61+TOL  && -62-TOL<g && g<-62+TOL && -TOL<b     && b<+TOL     ) 
    return 'P';
  else {
    return '$';
  }

  colorArr[0] = 0;
  colorArr[1] = 0;
  colorArr[2] = 0;  
}

int soundCheck() {
  long lowVal, highVal = 0;
  for(int i=0; i<5; i++){
    lowVal += analogRead(LOW_PIN);
    highVal += analogRead(HIGH_PIN);
    delay(50);
  }
  lowVal /= 5;
  highVal /= 5;
  
  if( highVal > 1){ // for high freq
    turnRight(FULL_RIGHT);
  }
  else if ( lowVal > 1 && highVal < 1) { // for low freq
    turnLeft(FULL_LEFT);
  } 
  else if (lowVal < 1 && highVal < 1) { //no sound signal detected
    buzzer.tone(200, 200);
    buzzer.tone(200, 200);
    buzzer.tone(200, 200);
    buzzer.tone(200, 200);
    buzzer.tone(200, 200);
    buzzer.tone(200, 200);
    buzzer.tone(200, 200);
    buzzer.tone(200, 200);
  }
}

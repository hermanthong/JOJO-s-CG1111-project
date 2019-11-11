#include <MeMCore.h>

#define LIGHT_SENSOR A6
#define LEFT_IR A2
#define RIGHT_IR A3
#define ULTRASONIC 12
#define LOW_PIN A1
#define HIGH_PIN A0

#define TIMEOUT 15000

#define MIN_ULTRASONIC 8
#define MIN_DISTANCE 700
#define MAX_SPEED 150
#define FULL_LEFT 475
#define FULL_RIGHT 475
#define UTURN 950
#define ONE_CELL 1300
#define AVOID_WALL 40

#define MAX 255
#define DELAY_TIME 500
#define NUM_READINGS 3


// Initialize sensors
MeLightSensor lightSensor(PORT_6);
MeRGBLed led(0,30);

// Initialize both motors
MeDCMotor motor1(M1); //Right forwards
MeDCMotor motor2(M2); //Left backwards
MeBuzzer buzzer;
MeLineFollower lineFinder(PORT_2);


int colorArr[3] = {0};
int whiteArr[3] = {360, 232, 266};
int blackArr[3] = {314, 202, 229};
double inputLeft, inputRight;
int sensorState;

void setup() {
  Serial.begin(9600);
  led.setpin(13);
  
  led.setColorAt(0,0,0,0);
  led.setColorAt(1,0,0,0);
  led.show();                                                                                                     
}

void loop() {
  delay(40);
  inputLeft = analogRead(LEFT_IR);
  inputRight = analogRead(RIGHT_IR); 
  //
  sensorState = lineFinder.readSensors();
  if (sensorState != S1_OUT_S2_OUT){
    motor1.stop();
    motor2.stop();
    moveBackward(AVOID_WALL);
    colorChallenge();
  }
  
  if(ultrasonic() <= MIN_ULTRASONIC){
    moveBackward(4*AVOID_WALL);
    if(inputLeft < inputRight){
      turnRight(2*AVOID_WALL);
    } else if(inputRight <= inputLeft){
      turnLeft(2*AVOID_WALL);
    }
  }
  if(inputRight <= MIN_DISTANCE && inputRight > 150){
    turnLeft(AVOID_WALL);    
  }else if(inputLeft <= MIN_DISTANCE && inputLeft > 150){
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
// moves the mBot backward
void moveBackward(int t){
 motor1.run(-MAX_SPEED);
 motor2.run(MAX_SPEED);
 delay(t);
  motor1.stop();
  motor2.stop();
}
// turns the mBot left
void turnLeft(int t){
 motor1.run(MAX_SPEED);
 motor2.run(MAX_SPEED);
 delay(t);
 motor1.stop();
 motor2.stop();
}
// turns the mBot right
void turnRight(int t){
 motor1.run(-MAX_SPEED);
 motor2.run(-MAX_SPEED);
 delay(t);
 motor1.stop();
 motor2.stop();
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
void colorChallenge() {
  int colorArr[3];
  int r, g, b;
  char color;

  for (int i=0; i<3; i++) colorArr[i] = 0;
  for (int i=0; i<3; i++) {
    led.setColor( (i==0) ? MAX : 0, (i==1) ? MAX : 0 , (i==2) ? MAX : 0);
    led.show();
    delay(DELAY_TIME);
    colorArr[i] = (lightSensor.read() - blackArr[i])*255/(whiteArr[i]-blackArr[i]);
  }
  led.setColor(0,0,0);
  led.show();

  r = colorArr[0],
  g = colorArr[1],
  b = colorArr[2];
      
  // Conditionals to determine color
  // Return 0 for no color detected,
  //        X for black,
  //        R for red,
  //        G for green,
  //        B for blue,
  //        Y for yellow,
  //        P for purple.
  //        $ for error.
 if (b>200) {
    if (g>240) color = 'B';
    else color = 'P';
  }
  else if (r>240) {
    if (g>240 && b>100) color = 'Y';
    else color = 'R';
  }
  else if (g>200) {
     color = 'G';
  }
  else if (r<110 && g<110 && b<110)
    color = 'X';
  else 
    color = '$';
  
  // INSTRUCTIONS FOR EACH COLOR
  switch(color){
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
      inputLeft = analogRead(LEFT_IR);
      inputRight = analogRead(RIGHT_IR); 
      if(inputLeft < inputRight)
        turnRight(UTURN);
      else if(inputRight <= inputLeft)
        turnLeft(UTURN);
      break;
    case 'P':
      turnLeft(FULL_LEFT);
      moveForward(ONE_CELL);
      turnLeft(FULL_LEFT);
      break;
    case 'X':
      soundChallenge();
      break;
    case '$':
      motor1.stop();
      motor2.stop();
      buzzer.tone(2000, 200);
      delay(2000);
      break;
  }
}


void soundChallenge() {
  long lowVal, highVal = 0;
  
  // sample 10 times in 500ms, then take the average value
  for(int i=0; i<10; i++){
    lowVal += analogRead(LOW_PIN);
    highVal += analogRead(HIGH_PIN);
    delay(50);
  }
  lowVal /= 10;
  highVal /= 10;
  
  if( highVal >= 100){ // for high freq
    turnRight(FULL_RIGHT);
  }
  else if ( lowVal >= 500 && highVal < 100) { // for low freq
    turnLeft(FULL_LEFT);
  } 
  else if (lowVal < 500 && highVal < 100) { //no sound signal detected
    victory();
  }
}

#define NOTE_DS5 622
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831 
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_D6 1175
#define NOTE_E6 1319

void victory(){ // Thomas the Tank Engine Theme Song
 int melody[] = {
    NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6,
    NOTE_D6, NOTE_E6,
    NOTE_GS5, 0, 
    NOTE_A5, NOTE_F5, NOTE_A5, NOTE_G5, 0,
    NOTE_GS5, NOTE_A5, NOTE_F5, NOTE_F5, NOTE_A5, NOTE_G5,
    NOTE_FS5, NOTE_G5, NOTE_FS5, NOTE_G5, NOTE_FS5,
    NOTE_G5, NOTE_G5, 0,
    NOTE_FS5, NOTE_G5, NOTE_FS5, NOTE_G5,
    NOTE_GS5, NOTE_GS5,
    0, NOTE_DS5, NOTE_DS5, NOTE_F5, NOTE_FS5,
    NOTE_G5, NOTE_AS5,
    NOTE_F5, NOTE_G5,
    NOTE_GS5, 0    
 };
 int noteDurations[] = {// measured in semiquavers
    4, 4, 4, 8,
    4, 8,
    8, 24,
    4, 4, 4, 8, 10,
    2, 4, 4, 4, 2, 8,
    1, 3, 1, 3, 1,
    8, 8, 7,
    1, 3, 1, 4,
    8, 8,
    2, 2, 4, 4, 4,
    8, 8,
    8, 8,
    4, 12
    };
 
 // crotchet = 190, note delay = 158ms
 double noteDelay = 1000.0 * 60.0/190.0 / 2 /4;
 for (int i=0; i<44; i++) {
  int thisDuration = noteDelay * noteDurations[i];
  if(melody[i] == 0)
    delay(thisDuration);
  else
    buzzer.tone(8, melody[i], thisDuration);

  // delay to differentiate between notes
  delay(thisDuration);
  buzzer.noTone(8);
 }
}

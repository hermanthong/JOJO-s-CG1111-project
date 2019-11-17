#include <MeMCore.h>

// SETTING SENSOR PINS
#define LIGHT_SENSOR A6
#define LEFT_IR A2
#define RIGHT_IR A3
#define ULTRASONIC 12
#define LOW_SOUND_PIN A1
#define HIGH_SOUND_PIN A0

#define TIMEOUT 15000
#define ULTRASONIC_DIVIDER 57.78

// DEFINITIONS FOR BASIC MOVEMENTS
#define MIN_ULTRASONIC 8
#define MIN_DISTANCE 700
#define MAX_SPEED 150 
#define MAX_SPEED_FWD 200
#define FULL_LEFT 475
#define FULL_RIGHT 475
#define UTURN 970
#define ONE_CELL 1300
#define AVOID_WALL 40

// DEFINITIONS FOR THE COLOR SENSOR
#define MAX 255
#define DELAY_TIME 250

// FREQUENCIES FOR VICTORY TUNE
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


// Initialize sensors
MeLightSensor lightSensor(PORT_6);
MeRGBLed led(0,30);

// Initialize both motors
MeDCMotor motor1(M1); //Right forwards
MeDCMotor motor2(M2); //Left backwards
MeBuzzer buzzer;
MeLineFollower lineFinder(PORT_2);

double inputLeft, inputRight;
int sensorState;


void setup() {
  Serial.begin(9600);
  led.setpin(13);
  
  led.setColor(0,0,0);
  led.show();                                                                                                     
}

void loop() {
  delay(40);
  inputLeft = analogRead(LEFT_IR);
  inputRight = analogRead(RIGHT_IR); 
  
  sensorState = lineFinder.readSensors();
  if (sensorState != S1_OUT_S2_OUT){
    motor1.stop();
    motor2.stop();
    delay(DELAY_TIME);    
    colorChallenge();
  }
  
  if(ultrasonic() <= MIN_ULTRASONIC){
    moveBackward(6 * AVOID_WALL);
    if(inputLeft < inputRight){
      turnRight(2 * AVOID_WALL);
    } else if(inputRight <= inputLeft){
      turnLeft(2 * AVOID_WALL);
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

/* START OF BASIC MOVEMENT FUNCTIONS */

// moves the mBot forward
void moveForward(){
  motor1.run(MAX_SPEED_FWD);
  motor2.run(-MAX_SPEED_FWD);
}

// moves mBot forward for time t
void moveForward(int t){
  motor1.run(MAX_SPEED_FWD);
  motor2.run(-MAX_SPEED_FWD);
  delay(t);
  motor1.stop();
  motor2.stop();
}

// moves the mBot backward
void moveBackward(int t){
  motor1.run(-MAX_SPEED_FWD);
  motor2.run(MAX_SPEED_FWD);
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
  
  // Creates an ultrasonic pulse
  pinMode(ULTRASONIC, OUTPUT);
  digitalWrite(ULTRASONIC, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC, HIGH);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC, LOW);
  pinMode(ULTRASONIC, INPUT);
  
  // Return pulse in
  // empirically derived
  return pulseIn(ULTRASONIC, HIGH, TIMEOUT) / 28.89 / 2;  
}

/* CHALLENGE FUNCTIONS */

// The color challenge
void colorChallenge() {
  int r, g, b;
  char color = '\0';
  int colorArr[3];
  
  // Values from calibration with white paper and black paper
  int whiteArr[3] = {598, 355, 495};
  int blackArr[3] = {253, 138, 183};
  int greyDiff[3] = {whiteArr[0]-blackArr[0], 
                     whiteArr[1]-blackArr[1], 
                     whiteArr[2]-blackArr[2]};

  // Reads the value of light sensor for each LED color
  for (int i=0; i<3; i++) {
    led.setColor( (i==0) ? MAX : 0, (i==1) ? MAX : 0 , (i==2) ? MAX : 0);
    led.show();
    delay(DELAY_TIME);
    colorArr[i] = (lightSensor.read() - blackArr[i]) * 255 / greyDiff[i];
  }
  led.setColor(0,0,0);
  led.show();

  // Assign r g b with values from the color sensor
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

  // Empirically derived algorithm
  if (r+g+b>100*3) {
    color = '$';
  } else if (r<-80) {
    color = 'Y';
  } else if (r>65) {
    color ='R';
  } else if (r>30 && r<55) {
    if (g>80)
      color = 'B';
    else 
      color = 'P';
  } else if (g>30) {
    color = 'G';
  } else if (r<0 && g<0 && b<0){
    color = 'X';
  } else {
    color = '$';
  }
  
  // Instructions for each color
  switch(color){
    case 'R':
      buzzer.tone(1200, 400);
      turnLeft(FULL_LEFT);
      break;
      
    case 'G':
      buzzer.tone(1200, 400);
      delay(160);
      buzzer.tone(1200, 400);
      turnRight(FULL_RIGHT);
      break;
      
    case 'B':
      buzzer.tone(200, 400);
      delay(160);
      buzzer.tone(200, 400);
      turnRight(FULL_RIGHT);
      while(ultrasonic() > 9.5)
        moveForward(AVOID_WALL);
      turnRight(FULL_RIGHT);
      break;
      
    case 'Y':
      buzzer.tone(1200, 400);
      delay(160);
      buzzer.tone(1200, 400);
      delay(160);
      buzzer.tone(1200, 400);
      inputLeft = analogRead(LEFT_IR);
      inputRight = analogRead(RIGHT_IR); 
      // U turn direction is based on IR sensor readings.
      if(inputLeft <= inputRight)
        turnRight(UTURN);
      else if(inputLeft > inputRight)
        turnLeft(UTURN);
      break;
      
    case 'P':
      buzzer.tone(200, 400);
      turnLeft(FULL_LEFT);
      while(ultrasonic() > 9.5)
        moveForward(AVOID_WALL);
      turnLeft(FULL_LEFT);
      break;
      
    case 'X':
      buzzer.tone(400, 200);
      delay(160);
      buzzer.tone(400, 200);
      delay(160);
      buzzer.tone(400, 200);
      soundChallenge();
      break;
      
    case '$':
      motor1.stop();
      motor2.stop();
      buzzer.tone(2000, 200);
      delay(1000);
      break;
  }
}

// Do the sound Challenge
void soundChallenge() {
  long lowVal=0, highVal = 0;
  
  // sample 20 times in 1s, then take the average value
  for(int i=0; i<20; i++){
    lowVal += analogRead(LOW_SOUND_PIN);
    highVal += analogRead(HIGH_SOUND_PIN);
    delay(50);
  }
  
  lowVal /= 20;
  highVal /= 20;
  
  if (lowVal >= 300 && lowVal > highVal){ // for low frequencies
    turnLeft(FULL_LEFT);
  }
  else if(highVal >= 100) { // for high frequencies
    turnRight(FULL_RIGHT);
  } 
  else { // no sound signal detected
    victory();
    exit(0); // Shut down the bot after END OF MAZE
  }
}

// When all the challenges are finished play
// Thomas the Tank Engine Theme Song
void victory() { 
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
  int noteDurations[] = { // measured in semiquavers
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
  
  // Playing the music according to tempo
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

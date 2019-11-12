#include <MeMCore.h>

#define MIN_DISTANCE 700
#define MAX_SPEED 255
#define FULL_LEFT 300
#define FULL_RIGHT 300
#define UTURN 600
#define ONE_CELL 700
#define AVOID_WALL 25

MeDCMotor motor1(M1);
MeDCMotor motor2(M2);

void setup() {  
  moveForward(ONE_CELL);
  turnLeft(FULL_LEFT);
  moveForward(ONE_CELL);
  turnRight(UTURN);
  moveForward(ONE_CELL);
  turnRight(FULL_RIGHT);
  moveForward(ONE_CELL);
  motor1.stop();
  motor2.stop();
}

void loop() {
  
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

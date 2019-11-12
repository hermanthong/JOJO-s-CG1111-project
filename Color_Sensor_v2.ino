#include <MeMCore.h>

#define MAX 255
#define DELAY_TIME 250

// Initialize sensors
MeLightSensor lightSensor(PORT_6);
MeRGBLed led(0,30);
MeBuzzer buzzer;

int colorArr[3] = {0};
int whiteArr[3] = {601, 359, 426};
int blackArr[3] = {256, 141, 185};
int greyDiff[3] = {whiteArr[0]-blackArr[0], 
                  whiteArr[1]-blackArr[1], 
                  whiteArr[2]-blackArr[2]};

void setup() {
  // put your setup code here, to run once:
  led.setpin(13);
  pinMode(7, INPUT); //Define button pin as input
  
  //begin serial communication
  Serial.begin(9600);

  led.setColor(0,0,0);
  led.show();

  // PRESS BUTTON WITHIN 5 SECS TO CALIBRATE
  delay(5000);
  if (analogRead(7) < 100) {
    calibrateColor();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
  colorChallenge();
  delay(2000);
  
}

void colorChallenge() {
  int colorArr[3];
  int r, g, b;
  char color = '\0';
  
  for (int i=0; i<3; i++) {
    led.setColor( (i==0) ? MAX : 0, (i==1) ? MAX : 0 , (i==2) ? MAX : 0);
    led.show();
    delay(DELAY_TIME);
    colorArr[i] = (lightSensor.read() - blackArr[i]) * 255 / greyDiff[i];
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

  printArr(colorArr);
  Serial.println(color);

  // INSTRUCTIONS FOR EACH COLOR
  switch (color) { 
    // PLEASE UPDATE INPUT INSTRUCTIONS
     case 'X': break;   
     case 'R': break;
     case 'G': break;
     case 'B': break;
     case 'Y': break;
     case 'P': break;
   }
}

void calibrateColor () {
  int i;

  // QUICKLY PUT BLACK DURING THE FIRST BUZZER
  buzzer.tone (600, 2000);
  
  for (int i=0; i<3; i++) {
    led.setColor( (i==0) ? MAX : 0, (i==1) ? MAX : 0 , (i==2) ? MAX : 0);
    led.show();
    delay(DELAY_TIME);
    blackArr[i] = lightSensor.read();
  }
  
  led.setColor(0,0,0);
  led.show();

  // QUICKLY PUT WHITE DURING THE SECOND BUZZER
  buzzer.tone (600, 2000);
  
  for (int i=0; i<3; i++) {
    led.setColor( (i==0) ? MAX : 0, (i==1) ? MAX : 0 , (i==2) ? MAX : 0);
    led.show();
    delay(DELAY_TIME);
    whiteArr[i] = lightSensor.read();
  }
  
  led.setColor(0,0,0);
  led.show();

  for (int i=0; i<3; i++) {
    greyDiff[i] = whiteArr[i]-blackArr[i];
  }

  Serial.print("black: ");
  printArr(blackArr);
  Serial.print("\n");
  Serial.print("white: ");
  printArr(whiteArr);\
  Serial.print("\n");
    
  // PLAY A TUNE AFTER DONE CALIBRATING
  buzzer.tone (440, 200);
  buzzer.tone (880, 200);
  buzzer.tone (1320, 200);
}

void printArr (int arr[]) {
  for (int i=0; i<3; i++) {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
//  Serial.print("\n");
}

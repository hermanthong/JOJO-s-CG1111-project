#include <MeMCore.h>

#define LOW_PIN A1
#define HIGH_PIN A0

void setup() {
  Serial.begin(9600);
}

void loop() {
  int highVal=0, lowVal =0;
  for(int i=0; i<10; i++){
    lowVal += analogRead(LOW_PIN);
    highVal += analogRead(HIGH_PIN);
    delay(50);
  }
  lowVal /= 10;
  highVal /= 10;
  Serial.print(highVal);
  Serial.print("\t");
  Serial.println(lowVal);
  if ( lowVal >= 600 ) { // for low freq
    Serial.println("LEFT");
  }else if( highVal >= 100){ // for high freq
    Serial.println("RIGHT");
  }
  else { //no sound signal detected
    Serial.println("DANK");
  }
}

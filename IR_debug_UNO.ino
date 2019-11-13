#define RIGHT_IR A2
#define LEFT_IR A1

int inputLeft;
int inputRight;

void setup() {
  // put your setup code here, to run once:
  pinMode (RIGHT_IR, INPUT);
  pinMode (LEFT_IR, INPUT);
  pinMode (0, OUTPUT);
  pinMode (1, OUTPUT);

  digitalWrite(0, HIGH);
  digitalWrite(1, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

  inputLeft = analogRead(LEFT_IR);
  inputRight = analogRead(RIGHT_IR); 
  
  Serial.print(inputLeft);
  Serial.print("\t");
  Serial.println(inputRight);

  delay(10);
}

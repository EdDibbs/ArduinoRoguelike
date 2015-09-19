int UpDown    = 0;
int LeftRight = 0;


void setup() {  
  //setup serial communication
  Serial.begin(9600);
}

void loop() {
  UpDown = analogRead(A7);
  LeftRight = analogRead(A6);
  Serial.print("UpDown = ");
  Serial.print(UpDown, DEC);
  Serial.print(", LeftRight = ");
  Serial.println(LeftRight, DEC);
  delay(200);
  
}

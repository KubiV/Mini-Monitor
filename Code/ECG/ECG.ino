void setup() {

  Serial.begin(115200); // initialize the serial communication
  pinMode(10, INPUT); // Setup for leads off detection LO +
  pinMode(11, INPUT); // Setup for leads off detection LO -

}
 
void loop() {

  if((digitalRead(10) == 1)||(digitalRead(11) == 1)) {
    Serial.println('!');
  }
  else {
    Serial.println(analogRead(A0)); // send the value of analog input 0
  }
  delay(10); //Wait for a bit to keep serial data from saturating
}

void setup() {
  pinMode(2, INPUT);   // Connect HX710 OUT to Arduino pin 2
  pinMode(3, OUTPUT);  // Connect HX710 SCK to Arduino pin 3
  Serial.begin(115200);
}

void loop() {
  // wait for the current reading to finish
  while (digitalRead(2)) {}

  // read 24 bits
  long result = 0;
  for (int i = 0; i < 24; i++) {
    digitalWrite(3, HIGH);
    digitalWrite(3, LOW);
    result = result << 1;
    if (digitalRead(2)) {
      result++;
    }
  }

  // get the 2s compliment
  result = result ^ 0x800000;

  // pulse the clock line 3 times to start the next pressure reading
  for (char i = 0; i < 3; i++) {
    digitalWrite(3, HIGH);
    digitalWrite(3, LOW);
  }

  // display pressure
  Serial.println(result);
}

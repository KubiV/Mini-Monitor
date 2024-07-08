#define BAUD_RATE 115200
#define OUT_PIN 2
#define SCK_PIN 3

void setup() {
  pinMode(OUT_PIN, INPUT);   // Připoj modul HX710 OUT pin na Arduino např. pin D2
  pinMode(SCK_PIN, OUTPUT);  // Připoj modul HX710 SCK pin na Arduino např. pin D3
  Serial.begin(BAUD_RATE);
}

void loop() {
  // čekání na konec čtení
  while (digitalRead(OUT_PIN)) {}

  // přečti všech 24 bitů
  long result = 0;
  for (int i = 0; i < 24; i++) {
    digitalWrite(SCK_PIN, HIGH);
    digitalWrite(SCK_PIN, LOW);
    result = result << 1;
    if (digitalRead(OUT_PIN)) {
      result++;
    }
  }

  // převěď na dvojkový doplněk - způsob řešení záporných čísel
  result = result ^ 0x800000;

  // 3x pulz na hodinový pin pro začátek měření
  for (char i = 0; i < 3; i++) {
    digitalWrite(SCK_PIN, HIGH);
    digitalWrite(SCK_PIN, LOW);
  }

  // zobraz hodnotu
  Serial.println(result);
}
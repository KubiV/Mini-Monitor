#define BAUD_RATE 115200
#define OUT_PIN 2
#define SCK_PIN 3

// proměnné pro ukládání kalibračních dat
float pressure1_mmHg = 40;
float pressure2_mmHg = 200;
long adcValue1 = 9252292;
long adcValue2 = 10223993;
bool averaging1 = false;
bool averaging2 = false;
long sum1 = 0;
long sum2 = 0;
int count1 = 0;
int count2 = 0;

void setup() {
  pinMode(OUT_PIN, INPUT);   // připoj modul HX710 OUT pin na Arduino např. pin D2
  pinMode(SCK_PIN, OUTPUT);  // připoj modul HX710 SCK pin na Arduino např. pin D3
  Serial.begin(BAUD_RATE);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');

    if (command == "start40") {
      averaging1 = true;
      sum1 = 0;
      count1 = 0;
      Serial.println("Starting averaging for 40 mmHg...");
    } else if (command == "stop40") {
      averaging1 = false;
      if (count1 > 0) {
        adcValue1 = sum1 / count1;
        Serial.print("Averaged ADC Value for 40 mmHg: ");
        Serial.println(adcValue1);
      } else {
        Serial.println("No data to average for 40 mmHg.");
      }
    } else if (command == "start200") {
      averaging2 = true;
      sum2 = 0;
      count2 = 0;
      Serial.println("Starting averaging for 200 mmHg...");
    } else if (command == "stop200") {
      averaging2 = false;
      if (count2 > 0) {
        adcValue2 = sum2 / count2;
        Serial.print("Averaged ADC Value for 200 mmHg: ");
        Serial.println(adcValue2);
      } else {
        Serial.println("No data to average for 200 mmHg.");
      }
    }
  }

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

  // Průměrování hodnot pro kalibraci
  if (averaging1) {
    sum1 += result;
    count1++;
  }
  if (averaging2) {
    sum2 += result;
    count2++;
  }

  // Vypočítej tlak v mmHg pomocí lineární interpolace mezi dvěma kalibračními body
  float pressure_mmHg = mapCalibration(result, adcValue1, adcValue2, pressure1_mmHg, pressure2_mmHg);

  // 3x pulz na hodinový pin pro začátek měření
  for (char i = 0; i < 3; i++) {
    digitalWrite(SCK_PIN, HIGH);
    digitalWrite(SCK_PIN, LOW);
  }

  // zobraz hodnotu
  Serial.print("Pressure: ");
  Serial.print(pressure_mmHg);
  Serial.println(" mmHg");

  // pauza před dalším měřením
  delay(10);
}

// funkce pro lineární mapování
float mapCalibration(long x, long in1, long in2, float out1, float out2) {
  return (float)(x - in1) * (out2 - out1) / (float)(in2 - in1) + out1;
}

#include <SoftwareSerial.h>
#include <NewPing.h>

// Serial komunikasi HMI
#define RX_PIN A0
#define TX_PIN A1
SoftwareSerial mySerial(RX_PIN, TX_PIN);

// Sensor Ultrasonik HC-SR04
#define TRIGGER_PIN  3
#define ECHO_PIN     4
#define MAX_DISTANCE 20
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Motor Pompa
#define enB 9
#define in3 10
#define in4 11


String data;
bool sistem = false;
bool sistemAktif = false;

// Serial 115200 --> Arduino ke Laptop
unsigned long lastSendToSerialOff = 0;
unsigned long lastSendToSerialOn = 0;


// Variabel sistem
float jarak = 0.0;
float water = 0.0;

float setpoint = 5.0;  // Setpoint dapat diubah dari 5 - 10 cm

unsigned long lastPingTime = 0;
const unsigned long pingInterval = 100;



void setup() {
  Serial.begin(115200);
  mySerial.begin(4800);
  mySerial.setTimeout(100);  // Tunggu maksimal 100ms
  delay(500);                // Waktu sinkronisasi serial

  Serial.println("Serial started...");

  // Motor setup
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  digitalWrite(enB, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void loop() {

  unsigned long now = millis();

  // Baca data HMI
  if (mySerial.available() > 0) {
    data = mySerial.readStringUntil('\n');
    data.trim();

    if (data.startsWith("A:") && data.length() > 2 && data.length() <= 10) {
      sistem = data.substring(2).toInt();
      sistemAktif = sistem;

      if (!sistemAktif) {
        analogWrite(enB, 0); // Pompa Off saat sistem tidak aktif
      }
    }
  } // HMI


  // Pembacaan sensor berkala
  if (now - lastPingTime >= pingInterval) {
    unsigned int waktuPantulan = sonar.ping();
    float jarakBaca = waktuPantulan / 58.0;  // konversi μs → cm
    lastPingTime = now;

    // Validasi pembacaan
    if (jarakBaca == 0 || jarakBaca >= MAX_DISTANCE) {
      return; // Abaikan pembacaan dan jaga nilai water sebelumnya
    }

    jarak = jarakBaca;

    // Regresi Polinomial
    water = -0.0149 * jarak * jarak - 0.712 * jarak + 17.910;

    // Amankan agar tidak negatif
    if (water < 0) water = 0;
  }


  // Logika open-loop kontrol pompa
  if (sistemAktif) {
    if (water <= setpoint) {
      analogWrite(enB, 250);  // Pompa On
    }

    else {
      analogWrite(enB, 0);  // Pompa Off
    }

    // Kirim ke Laptop (Aktif)
    if (now - lastSendToSerialOn >= 1000) {
      Serial.print(setpoint); Serial.print(",");
      Serial.println(water);
      lastSendToSerialOn = now;
    }
  }

  // kirim ke Laptop (Tidak aktif)
  if ((now - lastSendToSerialOff >= 1000) && !sistemAktif) {
    Serial.print("Sistem: "); Serial.print(sistemAktif);
    Serial.print(" | Sp: "); Serial.println(setpoint);
    lastSendToSerialOff = now;
  }

  delay(10);
} 

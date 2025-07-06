#include <SoftwareSerial.h>
#include <NewPing.h>

// Komunikasi ke HMI
#define RX_PIN A0
#define TX_PIN A1
SoftwareSerial mySerial(RX_PIN, TX_PIN);

// Sensor HC-SR04
#define TRIGGER_PIN  3
#define ECHO_PIN     4
#define MAX_DISTANCE 20
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Motor Pompa
#define enB 9
#define in3 10
#define in4 11

// Variabel
String data;
bool sistem = false;
bool sistemAktif = false;

unsigned long lastSendToSerialOff = 0;
unsigned long lastSendToSerialOn = 0;

float setpoint = 0.0;
float jarak = 0.0;
float water = 0.0;

float inputAir = 0.0;
float output = 0.0;
float Kp = 0.0, Ki = 0.0, Kd = 0.0;
float error = 0.0, lastError = 0.0;
float integral = 0.0, derivative = 0.0;
int pwm = 0;

unsigned long lastPIDTime = 0;
unsigned long lastPingTime = 0;
const unsigned long pingInterval = 100;
const unsigned long pidInterval = 200;

unsigned long lastSendToHMIOn = 0;
unsigned long lastSendToHMIOff = 0;


void setup() {
  mySerial.begin(4800);
  mySerial.setTimeout(100);
  delay(500);

  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  digitalWrite(enB, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}


void loop() {
  unsigned long now = millis();

  // Data dari HMI
  if (mySerial.available() > 0) {
    data = mySerial.readStringUntil('\n');
    data.trim();

    if (data.startsWith("A:") && data.length() > 2 && data.length() <= 10) {
      sistem = data.substring(2).toInt();
      sistemAktif = sistem;

      if (sistemAktif) {
        integral = 0;
        lastError = 0;

      } else {
        analogWrite(enB, 0);
      }
    } else if (!sistemAktif && data.startsWith("B:") && data.length() > 2 && data.length() <= 10) {
      setpoint = data.substring(2).toFloat();
    } else if (!sistemAktif && data.startsWith("C:") && data.length() > 2 && data.length() <= 10) {
      Kp = data.substring(2).toFloat();
    } else if (!sistemAktif && data.startsWith("D:") && data.length() > 2 && data.length() <= 10) {
      Ki = data.substring(2).toFloat();
    } else if (!sistemAktif && data.startsWith("E:") && data.length() > 2 && data.length() <= 10) {
      Kd = data.substring(2).toFloat();
    }
  }


  // Update Sensor
  if (now - lastPingTime >= pingInterval) {
    unsigned int waktuPantulan = sonar.ping();
    float jarakBaca = waktuPantulan / 58.0;  // konversi μs → cm
    lastPingTime = now;

    // Validasi pembacaan
    if (jarakBaca == 0 || jarakBaca >= MAX_DISTANCE) {
      return;
    }

    jarak = jarakBaca;

    // Regresi Polinomial
    water = -0.0149 * jarak * jarak - 0.712 * jarak + 17.910;

    // Nilai tidak negatif
    if (water < 0) water = 0;

  }

  // Program PID
  if (sistemAktif && (now - lastPIDTime >= pidInterval)) {
    float deltaTime = (now - lastPIDTime) / 1000.0; // konversi ms → detik
    lastPIDTime = now;

    inputAir = water;

    error = setpoint - inputAir;

    // Anti-windup pada integral
    if (!((output >= 250 && error > 0) || (output <= 0 && error < 0))) {
      integral += error * deltaTime;
    }

    derivative = (error - lastError) / deltaTime;

    // PID output
    output = Kp * error + Ki * integral + Kd * derivative;
    output = constrain(output, 0, 250);
    pwm = (int)output;
    analogWrite(enB, pwm);

    lastError = error;

    // Kirim ke HMI Sistem Aktif
    if (now - lastSendToHMIOn >= 500){
      mySerial.print("B:"); mySerial.println(inputAir);
      mySerial.print("C:"); mySerial.println(pwm);
      lastSendToHMIOn = now;
    }

  }

  // Kirim ke HMI Sistem Tidak Aktif
  if ((now - lastSendToHMIOff >= 500) && !sistemAktif){
    mySerial.print("B:"); mySerial.println(0);
    mySerial.print("C:"); mySerial.println(0);
    lastSendToHMIOff = now;
  }

  delay(10); // Mengurangi beban CPU
}

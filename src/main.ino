/**
 * ============================================================
 *  Mobil Navigasi Otonomus Berbasis GPS dan Kompas dengan Kontrol PID
 *  Mata Kuliah  : Sistem Otomasi (CCE61307)
 *  Institusi    : Fakultas Ilmu Komputer, Universitas Brawijaya
 *  Tahun        : 2025
 * ============================================================
 *
 *  Hardware:
 *    - Arduino Uno
 *    - Motor Driver L298N
 *    - Kompas QMC5883
 *    - GPS NEO-6M (via SoftwareSerial)
 *    - Sensor Ultrasonik HC-SR04 (opsional, untuk penghindaran rintangan)
 *    - Servo (untuk memutar sensor ultrasonik)
 *
 *  Deskripsi Singkat:
 *    Program ini mengendalikan arah kendaraan menggunakan kontrol PID.
 *    Bearing target dihitung dari data GPS, lalu dibandingkan dengan
 *    heading aktual dari sensor kompas QMC5883. Selisihnya (error)
 *    diproses PID untuk menghasilkan sinyal koreksi pada kecepatan
 *    motor kiri & kanan.
 * ============================================================
 */

#include <Wire.h>
#include <DFRobot_QMC5883.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// ====================================================================
// KONFIGURASI PIN
// ====================================================================

// Motor kiri
#define LEFT_PWM   5
#define LEFT_IN1   3
#define LEFT_IN2   7

// Motor kanan
#define RIGHT_PWM  10
#define RIGHT_IN3  8
#define RIGHT_IN4  6

// GPS (SoftwareSerial)
#define GPS_RX     11
#define GPS_TX     12

// ====================================================================
// OBJEK PERIPHERAL
// ====================================================================

SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus    gps;
DFRobot_QMC5883 compass(&Wire, 0x0D);   // Alamat I2C QMC5883

// ====================================================================
// VARIABEL KOMPAS
// ====================================================================

float compass_heading  = 0.0;
float headingOffset    = 0.0;

// Offset kalibrasi hard-iron (sesuaikan berdasarkan hasil kalibrasi)
const int   OFFSET_X         = -100;
const int   OFFSET_Y         = 1700;

// Deklinasi magnetik lokal (derajat). Malang ~+0.5°, Jakarta ~-0.5°
const float DECLINATION_ANGLE = -0.5;

// ====================================================================
// PARAMETER PID
// ====================================================================

float kp = 3.0;
float ki = 0.5;
float kd = 0.8;

float previousError = 0.0;
float integral      = 0.0;
unsigned long lastTime = 0;

// ====================================================================
// PARAMETER NAVIGASI
// ====================================================================

int   baseSpeed          = 175;          // Kecepatan dasar motor (0-255)
float directionTolerance = 1.0;          // Toleransi error arah (derajat)

// Mode navigasi: gunakan manualTargetHeading untuk pengujian tanpa GPS,
// atau ganti dengan bearing yang dihitung dari GPS waypoint.
float manualTargetHeading = 180.0;       // Target heading (derajat, 0-360)

float error = 0.0;

// ====================================================================
// TIMING
// ====================================================================

unsigned long lastLoopTime  = 0;
unsigned long lastPrintTime = 0;
unsigned long startTime     = 0;

const unsigned long LOOP_INTERVAL_MS  = 10;   // ms antar iterasi loop
const unsigned long PRINT_INTERVAL_MS = 10;   // ms antar log serial

// ====================================================================
// SETUP
// ====================================================================

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Wire.begin();

  // --- Inisialisasi Kompas ---
  while (!compass.begin()) {
    Serial.println(F("[ERROR] Sensor QMC5883 tidak ditemukan! Periksa koneksi I2C."));
    delay(500);
  }
  Serial.println(F("[OK] QMC5883 berhasil diinisialisasi."));

  compass.setRange(QMC5883_RANGE_2GA);
  compass.setMeasurementMode(QMC5883_CONTINOUS);
  compass.setDataRate(QMC5883_DATARATE_100HZ);
  compass.setSamples(QMC5883_SAMPLES_8);

  // --- Konfigurasi Pin Motor ---
  pinMode(LEFT_PWM,   OUTPUT);
  pinMode(LEFT_IN1,   OUTPUT);
  pinMode(LEFT_IN2,   OUTPUT);
  pinMode(RIGHT_PWM,  OUTPUT);
  pinMode(RIGHT_IN3,  OUTPUT);
  pinMode(RIGHT_IN4,  OUTPUT);

  // --- Inisialisasi Waktu ---
  startTime    = millis();
  lastTime     = millis();
  lastLoopTime = millis();

  // --- Cetak Parameter PID ---
  Serial.println(F("===== Parameter PID ====="));
  Serial.print(F("  Kp : ")); Serial.println(kp);
  Serial.print(F("  Ki : ")); Serial.println(ki);
  Serial.print(F("  Kd : ")); Serial.println(kd);
  Serial.println(F("========================="));
}

// ====================================================================
// LOOP UTAMA
// ====================================================================

void loop() {
  unsigned long now = millis();

  // Throttle loop agar stabil di ~100 Hz
  if (now - lastLoopTime < LOOP_INTERVAL_MS) return;
  lastLoopTime = now;

  float dt = (now - lastTime) / 1000.0;
  if (dt <= 0.0) dt = 0.01;
  lastTime = now;

  // 1. Baca heading aktual dari kompas
  readCompass();

  // 2. Hitung error arah
  error = manualTargetHeading - compass_heading;
  if (error >  180.0) error -= 360.0;
  if (error < -180.0) error += 360.0;

  // 3. Hitung koreksi PID (hanya jika error di luar toleransi)
  float correction = 0.0;
  if (abs(error) > directionTolerance) {
    correction = computePID(manualTargetHeading, compass_heading, dt);
  }

  // 4. Kirim perintah ke motor
  driveWithCorrection(correction);

  // 5. Log serial
  if (now - lastPrintTime >= PRINT_INTERVAL_MS) {
    float elapsed = (now - startTime) / 1000.0;
    Serial.print(F("t=")); Serial.print(elapsed, 2);
    Serial.print(F("s | Heading=")); Serial.print(compass_heading, 1);
    Serial.print(F("° | Target=")); Serial.print(manualTargetHeading, 1);
    Serial.print(F("° | Error=")); Serial.println(error, 1);
    lastPrintTime = now;
  }
}

// ====================================================================
// FUNGSI: Baca Kompas QMC5883
// ====================================================================

/**
 * Membaca data raw dari QMC5883, menerapkan offset hard-iron,
 * menghitung heading (0-360°), dan menyimpan ke compass_heading.
 */
void readCompass() {
  sVector_t raw = compass.readRaw();

  float x = (float)(raw.XAxis - OFFSET_X);
  float y = (float)(raw.YAxis - OFFSET_Y);

  float heading = atan2(y, x) * (180.0 / M_PI);
  heading += DECLINATION_ANGLE;

  if (heading <   0.0) heading += 360.0;
  if (heading >= 360.0) heading -= 360.0;

  compass_heading = heading;
}

// ====================================================================
// FUNGSI: Hitung Output PID
// ====================================================================

/**
 * Menghitung output PID berdasarkan target heading dan heading aktual.
 *
 * @param target  Arah yang diinginkan (derajat)
 * @param actual  Arah saat ini dari kompas (derajat)
 * @param dt      Selang waktu sejak iterasi terakhir (detik)
 * @return        Nilai koreksi PID
 */
float computePID(float target, float actual, float dt) {
  float err = target - actual;

  // Normalisasi ke -180 .. +180
  if (err >  180.0) err -= 360.0;
  if (err < -180.0) err += 360.0;

  integral     += err * dt;
  float derivative = (err - previousError) / dt;
  previousError = err;

  return (kp * err) + (ki * integral) + (kd * derivative);
}

// ====================================================================
// FUNGSI: Kendalikan Motor dengan Koreksi PID
// ====================================================================

/**
 * Mengatur PWM motor kiri & kanan berdasarkan nilai koreksi PID.
 * Koreksi positif → belok kiri; negatif → belok kanan.
 *
 * @param correction  Output dari computePID()
 */
void driveWithCorrection(float correction) {
  correction = constrain(correction, -75.0, 75.0);

  int leftSpeed  = constrain((int)(baseSpeed + correction), 0, 255);
  int rightSpeed = constrain((int)(baseSpeed - correction), 0, 255);

  // Arah maju
  digitalWrite(LEFT_IN1,  LOW);
  digitalWrite(LEFT_IN2,  HIGH);
  digitalWrite(RIGHT_IN3, LOW);
  digitalWrite(RIGHT_IN4, HIGH);

  // PWM kecepatan (offset 75 pada roda kanan untuk kompensasi mekanis)
  analogWrite(LEFT_PWM,  leftSpeed);
  analogWrite(RIGHT_PWM, rightSpeed - 75);
}

/**
 * ============================================================
 *  Kalibrasi Kompas QMC5883 — Hard-Iron Calibration
 *  
 *  Cara penggunaan:
 *    1. Upload sketch ini ke Arduino
 *    2. Buka Serial Monitor (baud 9600)
 *    3. Putar kendaraan perlahan 360° sebanyak 2-3 kali
 *       di lingkungan yang jauh dari benda logam besar
 *    4. Lihat nilai Min/Max yang muncul di Serial Monitor
 *    5. Hitung offset:
 *         offsetX = (maxX + minX) / 2
 *         offsetY = (maxY + minY) / 2
 *    6. Masukkan nilai tersebut ke main.ino:
 *         const int OFFSET_X = <nilai>;
 *         const int OFFSET_Y = <nilai>;
 * ============================================================
 */

#include <Wire.h>
#include <DFRobot_QMC5883.h>

DFRobot_QMC5883 compass(&Wire, 0x0D);

int minX =  32767, maxX = -32768;
int minY =  32767, maxY = -32768;

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  while (!compass.begin()) {
    Serial.println(F("QMC5883 tidak ditemukan! Periksa koneksi."));
    delay(500);
  }

  compass.setRange(QMC5883_RANGE_2GA);
  compass.setMeasurementMode(QMC5883_CONTINOUS);
  compass.setDataRate(QMC5883_DATARATE_100HZ);
  compass.setSamples(QMC5883_SAMPLES_8);

  Serial.println(F("=== Kalibrasi Kompas QMC5883 ==="));
  Serial.println(F("Putar kendaraan 360° secara perlahan..."));
  Serial.println();
}

void loop() {
  sVector_t raw = compass.readRaw();

  // Update min/max
  if (raw.XAxis < minX) minX = raw.XAxis;
  if (raw.XAxis > maxX) maxX = raw.XAxis;
  if (raw.YAxis < minY) minY = raw.YAxis;
  if (raw.YAxis > maxY) maxY = raw.YAxis;

  // Cetak setiap 200ms
  if (millis() - lastPrint >= 200) {
    lastPrint = millis();

    int offsetX = (maxX + minX) / 2;
    int offsetY = (maxY + minY) / 2;

    Serial.print(F("Raw  X=")); Serial.print(raw.XAxis);
    Serial.print(F("  Y=")); Serial.println(raw.YAxis);

    Serial.print(F("Min  X=")); Serial.print(minX);
    Serial.print(F("  Y=")); Serial.println(minY);

    Serial.print(F("Max  X=")); Serial.print(maxX);
    Serial.print(F("  Y=")); Serial.println(maxY);

    Serial.print(F(">>> Offset X=")); Serial.print(offsetX);
    Serial.print(F("  Offset Y=")); Serial.println(offsetY);
    Serial.println(F("---"));
  }
}

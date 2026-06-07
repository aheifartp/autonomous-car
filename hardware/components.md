# Daftar Komponen

## Bill of Materials (BOM)

| No | Komponen | Model/Tipe | Jumlah | Keterangan |
|----|----------|-----------|--------|------------|
| 1 | Mikrokontroler | Arduino Uno R3 | 1 | Otak utama sistem |
| 2 | Chassis | Kit Mobil Arduino 2WD/4WD | 1 | Rangka + motor DC |
| 3 | Motor Driver | L298N H-Bridge | 1 | Mengendalikan 2 motor DC |
| 4 | Sensor Jarak | HC-SR04 Ultrasonic | 1 | Deteksi rintangan |
| 5 | Servo | SG90 / MG90S | 1 | Memutar sensor ultrasonik |
| 6 | GPS | NEO-6M (U-Blox) | 1 | Koordinat posisi |
| 7 | Kompas / IMU | QMC5883L | 1 | Heading (yaw) kendaraan |
| 8 | Baterai | 18650 Li-Ion | 2 | Sumber daya utama |
| 9 | Battery Holder | Seri 2× 18650 | 1 | Dudukan baterai |
| 10 | Kabel Jumper | Male-Male / Male-Female | ~30 | Koneksi antar komponen |
| 11 | Breadboard mini | 400 titik | 1 | Prototyping koneksi |

---

## Spesifikasi Komponen Utama

### Arduino Uno R3
- Mikrokontroler: ATmega328P
- Clock: 16 MHz
- Flash: 32 KB
- SRAM: 2 KB
- Pin Digital I/O: 14 (6 PWM)
- Pin Analog: 6
- Tegangan operasi: 5V
- Tegangan input: 7–12V

### L298N H-Bridge Motor Driver
- Tegangan motor: 5–35V
- Arus per channel: 2A (maks 3A)
- Output: 2 channel (4 motor atau 2 motor + arah)
- Kontrol kecepatan: via PWM (ENA/ENB)
- Kontrol arah: via IN1–IN4

### HC-SR04 Ultrasonic Sensor
- Tegangan: 5V
- Jangkauan: 2 cm – 400 cm
- Akurasi: ±3 mm
- Sudut deteksi: 15°
- Frekuensi ultrasonik: 40 kHz
- Antarmuka: Digital (TRIG + ECHO)

### GPS NEO-6M (U-Blox)
- Protokol: NMEA 0183
- Baud rate default: 9600
- Akurasi posisi: 2.5 m CEP
- Update rate: 1 Hz (bisa hingga 5 Hz)
- Tegangan: **3.3V** (⚠️ bukan 5V)
- Antarmuka: UART (TX/RX)
- Cold start: ~27 detik

### QMC5883L Magnetometer (Kompas)
- Antarmuka: I2C (alamat 0x0D)
- Tegangan: 3.3V – 5V
- Range pengukuran: ±2 Gauss
- Resolusi: 12-bit
- ODR: 10 / 50 / 100 / 200 Hz
- Akurasi heading: ±1°–2° (setelah kalibrasi)

### Servo SG90
- Torque: 1.8 kg·cm (4.8V)
- Kecepatan: 0.1 s/60° (4.8V)
- Sudut: 0°–180°
- Tegangan: 4.8V – 6V
- Kontrol: PWM (50 Hz, pulse 1–2 ms)

### Baterai 18650 Li-Ion
- Kapasitas: ~2500–3000 mAh (per sel)
- Tegangan nominal: 3.7V per sel
- Tegangan total (seri 2×): ~7.4V
- Arus discharge maks: 5A (umum)

---

## Library Arduino yang Diperlukan

| Library | Sumber | Fungsi |
|---------|--------|--------|
| `TinyGPS++` | Mikal Hart (Arduino Library Manager) | Parse data NMEA dari GPS |
| `DFRobot_QMC5883` | DFRobot (Arduino Library Manager / GitHub) | Driver kompas QMC5883 |
| `SoftwareSerial` | Bawaan Arduino IDE | Serial software untuk GPS |
| `Wire` | Bawaan Arduino IDE | Komunikasi I2C |

### Cara Install Library

**Via Arduino Library Manager (cara termudah):**
1. Buka Arduino IDE
2. `Sketch` → `Include Library` → `Manage Libraries...`
3. Cari nama library dan klik **Install**

**Via GitHub (untuk DFRobot_QMC5883 jika tidak ada di Library Manager):**
```bash
cd ~/Documents/Arduino/libraries
git clone https://github.com/DFRobot/DFRobot_QMC5883.git
```

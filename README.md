# 🚗 Mobil Navigasi Otonomus — GPS + Kompas + PID

**Laporan Proyek Akhir Sistem Otomasi (CCE61307)**  
Fakultas Ilmu Komputer, Universitas Brawijaya — 2025

---

## 📌 Deskripsi Proyek

Proyek ini mengembangkan kendaraan skala kecil berbasis **Arduino Uno** yang mampu:

- Menavigasi secara otonom menuju koordinat GPS yang ditentukan
- Mempertahankan arah menggunakan kontrol **PID** dengan umpan balik dari sensor kompas **QMC5883**
- Mendeteksi dan menghindari rintangan menggunakan sensor **ultrasonik HC-SR04**

Sistem membandingkan *bearing* yang dihitung dari modul GPS dengan *heading* aktual dari kompas, lalu menggunakan algoritma PID untuk mengoreksi perbedaannya secara real-time.

---

## 👥 Anggota Tim

| Nama | NIM |
|------|-----|
| Gilang Shido Faizalhaq | 235150300111011 |
| Peter Abednego Wijaya | 235150300111013 |
| Phasya Vigo Khalil Nugroho | 235150300111004 |
| Reditya Imanuel Yuwono | 235150301111007 |
| Muchammad Ryan Afif | 235150301111008 |
| Muhammad Rafie Habibi Fauzi | 235150301111009 |

---

## 🗂️ Struktur Repositori

```
autonomous-car/
├── src/
│   └── main.ino              # Kode utama Arduino
├── hardware/
│   ├── wiring_diagram.md     # Panduan koneksi pin
│   └── components.md         # Daftar & spesifikasi komponen
├── docs/
│   ├── laporan.pdf           # Laporan proyek akhir
│   └── pid_analysis.md       # Analisis dan tuning PID
├── scripts/
│   └── calibrate_compass.ino # Sketch kalibrasi kompas
├── assets/
│   └── (gambar & diagram)
├── .gitignore
└── README.md
```

---

## 🔧 Komponen Hardware

| Komponen | Fungsi |
|----------|--------|
| Arduino Uno | Mikrokontroler utama |
| Kit Chassis Mobil | Rangka & roda kendaraan |
| L298N H-Bridge | Driver motor DC |
| HC-SR04 | Sensor ultrasonik (deteksi rintangan) |
| Servo Motor | Memutar sensor ultrasonik |
| GPS NEO-6M | Posisi koordinat (latitude/longitude) |
| QMC5883 / MPU9250 | Kompas / IMU untuk heading (yaw) |
| Baterai 18650 (×2) | Sumber daya |

---

## 📐 Diagram Sistem Kendali

```
        ┌─────────┐   Bearing    ┌────────────┐   Koreksi   ┌──────────┐
GPS ───►│ Bearing │─────────────►│ Kontroler  │────────────►│  Motor   │
        │ Hitung  │   (target)   │    PID     │             │ L + R DC │
        └─────────┘              └────────────┘             └──────────┘
                                        ▲                         │
                                        │  Heading (yaw)          │ Gerak
                                   ┌────┴────┐                    ▼
                                   │QMC5883  │◄──────────── [Kendaraan]
                                   │ Kompas  │
                                   └─────────┘
```

---

## ⚙️ Algoritma PID

Persamaan PID yang digunakan:

$$u(t) = K_p \cdot e(t) + K_i \int e(t)\,dt + K_d \frac{d}{dt}e(t)$$

Di mana:
- `e(t) = Bearing_GPS − Heading_Kompas`
- Error dinormalisasi ke rentang **−180° s/d +180°**

### Parameter PID (saat ini)

| Parameter | Nilai |
|-----------|-------|
| Kp | 3.0 |
| Ki | 0.5 |
| Kd | 0.8 |
| Base Speed | 175 (PWM) |
| Toleransi | 1° |

> **Catatan:** Parameter ini masih perlu di-*tuning* lebih lanjut pada uji lapangan.

---

## 🗺️ Koneksi Pin (Ringkasan)

| Komponen | Pin Arduino |
|----------|-------------|
| Motor Kiri PWM | D5 |
| Motor Kiri IN1 | D3 |
| Motor Kiri IN2 | D7 |
| Motor Kanan PWM | D10 |
| Motor Kanan IN3 | D8 |
| Motor Kanan IN4 | D6 |
| GPS RX | D11 |
| GPS TX | D12 |
| QMC5883 SDA | A4 (SDA) |
| QMC5883 SCL | A5 (SCL) |

> Lihat [`hardware/wiring_diagram.md`](hardware/wiring_diagram.md) untuk detail lengkap.

---

## 🛠️ Cara Upload & Jalankan

### Prasyarat
- [Arduino IDE](https://www.arduino.cc/en/software) versi 1.8+ atau 2.x
- Library yang dibutuhkan (install via Library Manager):
  - `TinyGPS++` oleh Mikal Hart
  - `DFRobot_QMC5883`
  - `SoftwareSerial` (bawaan Arduino)

### Langkah

```bash
# 1. Clone repositori
git clone https://github.com/<username>/autonomous-car.git
cd autonomous-car

# 2. Buka file utama di Arduino IDE
# File → Open → src/main.ino

# 3. Install library yang diperlukan
# Sketch → Include Library → Manage Libraries...
# Cari dan install: TinyGPS++, DFRobot_QMC5883

# 4. Pilih board & port
# Tools → Board → Arduino Uno
# Tools → Port → (pilih port yang sesuai)

# 5. Upload
# Klik tombol Upload (→)
```

---

## 📡 Format Log Serial

Buka Serial Monitor (baud 9600) setelah upload. Format output:

```
[OK] QMC5883 berhasil diinisialisasi.
===== Parameter PID =====
  Kp : 3.00
  Ki : 0.50
  Kd : 0.80
=========================
t=0.01s | Heading=182.3° | Target=180.0° | Error=-2.3
t=0.02s | Heading=181.1° | Target=180.0° | Error=-1.1
t=0.03s | Heading=180.2° | Target=180.0° | Error=-0.2
```

---

## 📋 Catatan Penting

1. **Kalibrasi Kompas:** Jalankan `scripts/calibrate_compass.ino` terlebih dahulu untuk mendapatkan nilai `OFFSET_X` dan `OFFSET_Y` yang akurat di lokasi penggunaan.

2. **Deklinasi Magnetik:** Sesuaikan konstanta `DECLINATION_ANGLE` di kode sesuai lokasi. Nilai untuk beberapa kota:
   - Malang: `+0.5°`
   - Jakarta: `-0.5°`
   - (Cek nilai akurat di [magnetic-declination.com](https://www.magnetic-declination.com/))

3. **GPS NEO-6M:** Modul ini bekerja pada logika 3.3V. Gunakan SoftwareSerial (pin D11/D12) dan pastikan VCC menggunakan pin 3.3V Arduino, **bukan** 5V, untuk menghindari kerusakan.

4. **Tuning PID:** Mulai dengan Ki = 0 dan Kd = 0, tuning Kp terlebih dahulu, kemudian tambahkan Kd untuk mengurangi overshoot, dan terakhir Ki untuk menghilangkan steady-state error.

---

## 📄 Lisensi

Proyek ini dibuat untuk keperluan akademik. Bebas digunakan dan dimodifikasi dengan menyertakan atribusi.

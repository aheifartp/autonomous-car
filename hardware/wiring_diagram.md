# Wiring Diagram — Koneksi Pin Arduino

## Arduino Uno → Motor Driver L298N

| Arduino Pin | L298N Pin | Keterangan |
|-------------|-----------|------------|
| D5 (PWM) | ENA | Kecepatan motor kiri |
| D3 | IN1 | Arah motor kiri (A) |
| D7 | IN2 | Arah motor kiri (B) |
| D10 (PWM) | ENB | Kecepatan motor kanan |
| D8 | IN3 | Arah motor kanan (A) |
| D6 | IN4 | Arah motor kanan (B) |
| GND | GND | Ground bersama |

### Output L298N → Motor DC

| L298N Pin | Koneksi |
|-----------|---------|
| OUT1, OUT2 | Motor DC kiri |
| OUT3, OUT4 | Motor DC kanan |
| 12V | Baterai positif |
| GND | Baterai negatif |

---

## Arduino Uno → GPS NEO-6M

> ⚠️ **PENTING:** GPS NEO-6M menggunakan logika 3.3V. Gunakan pin 3.3V Arduino sebagai VCC dan SoftwareSerial untuk komunikasi.

| Arduino Pin | GPS NEO-6M | Keterangan |
|-------------|------------|------------|
| D11 | TX | Arduino menerima data GPS |
| D12 | RX | Arduino mengirim ke GPS |
| 3.3V | VCC | **Gunakan 3.3V, bukan 5V!** |
| GND | GND | Ground |

---

## Arduino Uno → Kompas QMC5883 (I2C)

| Arduino Pin | QMC5883 | Keterangan |
|-------------|---------|------------|
| A4 (SDA) | SDA | Data I2C |
| A5 (SCL) | SCL | Clock I2C |
| 3.3V | VCC | Daya |
| GND | GND | Ground |

> Alamat I2C default: `0x0D`

---

## Arduino Uno → Sensor Ultrasonik HC-SR04

| Arduino Pin | HC-SR04 | Keterangan |
|-------------|---------|------------|
| A1 | TRIG | Trigger sinyal pulsa |
| A2 | ECHO | Echo sinyal pantul |
| 5V | VCC | Daya |
| GND | GND | Ground |

---

## Arduino Uno → Servo Motor

| Arduino Pin | Servo | Keterangan |
|-------------|-------|------------|
| D10 (PWM) | Signal | Sinyal kontrol servo |
| 5V | VCC | Daya |
| GND | GND | Ground |

> Servo digunakan untuk memutar sensor HC-SR04 saat memindai rintangan.

---

## Diagram Blok Keseluruhan

```
                         ┌─────────────┐
                         │ Baterai     │
                         │ 18650 ×2    │
                         └──────┬──────┘
                                │ 7.4V
                    ┌───────────┼────────────┐
                    │           │            │
               ┌────▼────┐  ┌──▼──┐    ┌───▼───┐
               │ L298N   │  │ UNO │    │ Step  │
               │ Driver  │  │ Vin │    │ Down  │
               └────┬────┘  └──┬──┘    └───────┘
                    │          │
         ┌──────────┤          │
     Motor       Motor     ┌───┴──────────────────────┐
     Kiri        Kanan     │        Arduino Uno        │
                           │                           │
                           │  D3,D5,D6,D7,D8,D10      │──► L298N
                           │  D11,D12 (SoftSerial)    │──► GPS
                           │  A4 (SDA), A5 (SCL)      │──► QMC5883
                           │  A1 (TRIG), A2 (ECHO)    │──► HC-SR04
                           │  D10 (PWM)               │──► Servo
                           └───────────────────────────┘
```

---

## Catatan Pemasangan

1. **Ground bersama:** Pastikan GND Arduino, L298N, GPS, dan semua sensor terhubung ke titik ground yang sama.
2. **Bypass capacitor:** Pasang kapasitor 100µF pada jalur daya motor untuk meredam noise.
3. **Jarak kabel:** Gunakan kabel sependek mungkin antara GPS dan Arduino untuk mengurangi interferensi.
4. **Posisi kompas:** Jauhkan QMC5883 dari motor dan kabel daya (minimal 10 cm) agar tidak terganggu medan magnet.

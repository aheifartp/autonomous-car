# Analisis Kontrol PID

## Persamaan Dasar

$$u(t) = K_p \cdot e(t) + K_i \int e(t)\,dt + K_d \frac{d}{dt}e(t)$$

Di mana:
- **e(t)** = error = `Bearing_GPS − Heading_Kompas`
- **u(t)** = sinyal koreksi kecepatan motor

Error dinormalisasi ke rentang **−180° s/d +180°** untuk menghindari lonjakan saat melintasi titik 0°/360°.

---

## Peran Tiap Komponen PID

### Proporsional (Kp)
- Menghasilkan koreksi **sebanding** dengan besarnya error saat ini
- Kp besar → respons cepat tapi cenderung overshoot dan osilasi
- Kp kecil → respons lambat, mungkin tidak bisa mencapai setpoint

### Integral (Ki)
- Menghitung **akumulasi error** dari waktu ke waktu
- Berguna untuk menghilangkan *steady-state error* (misal: bias sensor, beda beban roda)
- Ki terlalu besar → *integral windup*, osilasi tak terkendali

### Derivatif (Kd)
- Menghitung **laju perubahan error** untuk meredam overshoot
- Berfungsi seperti "rem" saat kendaraan mendekati setpoint
- Kd terlalu besar → sensitif terhadap noise sensor

---

## Parameter Saat Ini

| Parameter | Nilai | Satuan |
|-----------|-------|--------|
| Kp | 3.0 | — |
| Ki | 0.5 | — |
| Kd | 0.8 | — |
| Base Speed | 175 | PWM (0–255) |
| Batas koreksi | ±75 | PWM |
| Toleransi | 1.0 | derajat |

---

## Panduan Tuning (Ziegler–Nichols Manual)

### Langkah-langkah:

1. **Set Ki = 0, Kd = 0**
2. Naikkan Kp perlahan sampai kendaraan mulai **berosilasi** (bergelombang kiri-kanan tanpa henti)
   → Catat nilai ini sebagai **Ku** (ultimate gain)
   → Catat periode osilasi sebagai **Tu** (detik)
3. Atur parameter berdasarkan tabel:

| Tipe Controller | Kp | Ki | Kd |
|----------------|----|----|-----|
| P | 0.5 × Ku | 0 | 0 |
| PI | 0.45 × Ku | 1.2/Tu | 0 |
| PID | 0.6 × Ku | 2/Tu | Tu/8 |

4. Lakukan fine-tuning empiris berdasarkan perilaku aktual kendaraan

---

## Interpretasi Perilaku

| Perilaku Kendaraan | Kemungkinan Penyebab | Solusi |
|--------------------|---------------------|--------|
| Lambat mencapai arah | Kp terlalu kecil | Naikkan Kp |
| Berosilasi terus | Kp terlalu besar | Turunkan Kp, naikkan Kd |
| Overshoot lalu stabil | Kd perlu dinaikkan | Naikkan Kd sedikit |
| Error kecil tapi tidak hilang | Ki terlalu kecil | Naikkan Ki |
| Osilasi di sekitar setpoint | Ki terlalu besar | Turunkan Ki |
| Goyang saat sensor noise | Kd terlalu besar | Tambahkan low-pass filter pada error |

---

## Normalisasi Error

```cpp
float err = target - actual;
if (err >  180.0) err -= 360.0;
if (err < -180.0) err += 360.0;
```

Normalisasi ini penting agar robot memilih jalur belok yang **terpendek** (misal: dari 350° ke 10° → putar 20° ke kanan, bukan 340° ke kiri).

---

## Anti-Windup Integral

Pada implementasi saat ini, integral tidak dibatasi. Jika kendaraan terhalang lama atau berosilasi panjang, nilai integral bisa membesar tak terkendali (*windup*). Solusi yang disarankan untuk iterasi berikutnya:

```cpp
// Anti-windup: batasi nilai integral
integral = constrain(integral, -100.0, 100.0);
```

---

## Rencana Pengujian

- [ ] Uji lurus: Target heading tetap 0°, rekam waktu konvergensi
- [ ] Uji belok kecil: Error ±10°, rekam overshoot
- [ ] Uji belok besar: Error ±90°, rekam waktu dan osilasi
- [ ] Uji dengan GPS aktif: navigasi ke satu waypoint
- [ ] Uji penghindaran rintangan + navigasi GPS sekaligus

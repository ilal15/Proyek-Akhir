# Proyek-Akhir

## Sistem Kontrol Level Air Otomatis Menggunakan Metode PID dan Monitoring HMI

Tujuan dari proyek akhir ini adalah merancang sebuah prototipe sistem kontrol level air otomatis yang dapat digunakan sebagai media pembelajaran di laboratorium. Sistem ini dilengkapi dengan metode kontrol PID untuk menjaga kestabilan ketinggian air, serta antarmuka HMI (*Human-Machine Interface*) yang memungkinkan pengguna memasukkan nilai setpoint, parameter PID (Kp, Ki, Kd), dan memantau kondisi sistem secara *real-time*.

## Komponen Elektronik

- Arduino Nano  
- Sensor Ultrasonik HC-SR04  
- Driver motor L293D  
- Pompa DC 5V  
- Layar HMI Kapasitif 3,5”  
- Adaptor 9V DC 2A  

## Keterangan Program

### PID_Air_HMI

Program untuk mengirim semua data ke layar HMI.

### PID_Air_Laptop

Program untuk mengirim data ke laptop, tetapi pengaturan nilai setpoint, parameter PID (Kp, Ki, Kd), serta mengaktifkan atau menonaktifkan sistem tetap dari layar HMI.

### Proyek_Akhir_Open_Loop

Program untuk mengirim data ke laptop tanpa kontrol PID, Tombol Run dan Stop HMI digunakan untuk menjalankan atau menghentikan sistem. Nilai Setpoint pada program dapat diubah seperti 5.0, 6.0, 7.0, 8.0, 9.0, atau 10.0.

### Microsoft Excel Datastream untuk membuat grafik

---

**Create by:**  
**Nama:** Al Bilal Sapto Bahari  
**NIM:** 2141170028  
**Kelas:** 4C-D4TE


#include <WiFi.h>
#include <HTTPClient.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_HMC5883_U.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <MFRC522.h>
// serial qrcode
#include <SoftwareSerial.h>
#include "jpeg5.h"
#include "Free_Fonts.h"

// pin untuk RFID
#define SS_PIN 21
#define RST_PIN 22

MFRC522 mfrc522(SS_PIN, RST_PIN);

TFT_eSPI tft = TFT_eSPI();


// Konfigurasi WiFi
const char* ssid = "GAKS";
const char* password = "mata3lang";

const char* endpoint = "http://192.168.0.100/goalpara/gate_val";
const char* endpointRf = "http://192.168.0.100/goalpara/gate_rfid";

// Pin untuk sensor inframerah dan mesin scanner
const int infraredPin = 13;  // Ganti dengan pin yang sesuai
SoftwareSerial myserial(17, 16);
String kode = "";
int btnQr2 = 4;
int btnGate = 2;

String ip = "";
int jmlSaldo = 0;

//penting!!!
const String gateID = "1";
const String gateName = "CHECK IN";

String saldoData[12];

String id = "";


// Fungsi untuk menghitung jumlah kemunculan substring dalam string
int countOccurrences(String str, String substr) {
  int count = 0;
  size_t pos = str.indexOf(substr, 0);
  while (pos != -1) {
    count++;
    pos = str.indexOf(substr, pos + 1);
  }
  return count;
}

void sendData(String qrCode) {
  // Buat objek HTTPClient
  String qr = qrCode;
  HTTPClient http;
  Serial.println("http://192.168.0.100/goalpara/gate_val");
  http.begin(endpoint);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String body = "idgate=" + gateID + "&qrcode=" + qr;

  int httpResponCode = http.POST(body);

  if (httpResponCode > 0) {
    String payload = http.getString();
    Serial.print("Response: ");
    Serial.println(payload);
    Serial.println("QR READY: " + qr);
    // Cari posisi 'buka='
    int bukaPos = payload.indexOf("buka=");
    int saldoPos = payload.indexOf("saldo=");
    if (bukaPos != -1) {
      // Ambil nilai setelah 'buka=' hingga karakter berikutnya
      int saldoEndPos = payload.indexOf("*", saldoPos);
      String saldoValue = payload.substring(saldoPos + 6, saldoEndPos);
      String bukaValue = payload.substring(bukaPos + 5, bukaPos + 6);
      Serial.print("Nilai 'buka': ");
      Serial.println(bukaValue);
      if (bukaValue == "1") {
        digitalWrite(btnGate, LOW);
        kode = "";
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(2);
        tft.setCursor(30, 130);
        tft.print("SALDO " + gateName + ": " + saldoValue);
        delay(5000);
        tft.fillScreen(TFT_BLACK);
        tft.pushImage(0, 0, 320, 70, jpeg5);
        tft.setTextSize(3);
        tft.setCursor(100, 170);
        tft.print(gateName);
      } else if (bukaValue == "2") {  // ini untuk cadangan gate UTV
        digitalWrite(btnGate, LOW);
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(2);
        tft.setCursor(30, 130);
        tft.print("SALDO " + gateName + ": " + saldoValue);
        delay(5000);
        digitalWrite(btnGate, LOW);
        tft.fillScreen(TFT_BLACK);
        tft.pushImage(0, 0, 320, 70, jpeg5);
        tft.setTextSize(3);
        tft.setCursor(100, 170);
        tft.print(gateName);
        delay(5000);
        kode = "";
      } else if (bukaValue == "3") {  // ini untuk cadangan gate RB
        digitalWrite(btnGate, LOW);
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(2);
        tft.setCursor(30, 130);
        tft.print("SALDO " + gateName + ": " + saldoValue);
        delay(5000);
        tft.fillScreen(TFT_BLACK);
        tft.pushImage(0, 0, 320, 70, jpeg5);
        tft.setTextSize(3);
        tft.setCursor(100, 170);
        tft.print(gateName);
        digitalWrite(btnGate, LOW);
        delay(5000);
        digitalWrite(btnGate, LOW);
        delay(5000);
        kode = "";
      } else if (bukaValue == "4") {  // ini untuk cadangan gate RB
        digitalWrite(btnGate, LOW);
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(2);
        tft.setCursor(30, 130);
        tft.print("SALDO " + gateName + ": " + saldoValue);
        delay(5000);
        tft.fillScreen(TFT_BLACK);
        tft.pushImage(0, 0, 320, 70, jpeg5);
        tft.setTextSize(3);
        tft.setCursor(100, 170);
        tft.print(gateName);
        digitalWrite(btnGate, LOW);
        delay(5000);
        digitalWrite(btnGate, LOW);
        delay(5000);
        kode = "";
      }
      kode = "";
    } else {
      Serial.println("Tidak ditemukan 'buka=' dalam respons.");
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(30, 170);
      tft.print("TERJADI KESALAHAN!");
      delay(3000);
      tft.fillScreen(TFT_BLACK);
      tft.pushImage(0, 0, 320, 70, jpeg5);
      tft.setTextSize(3);
      tft.setCursor(100, 170);
      tft.print(gateName);
    }
  } else {
    Serial.print("Error ");
    Serial.println(httpResponCode);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(30, 170);
    tft.print("TERJADI KESALAHAN!");
    delay(3000);
    tft.fillScreen(TFT_BLACK);
    tft.pushImage(0, 0, 320, 70, jpeg5);
    tft.setTextSize(3);
    tft.setCursor(100, 170);
    tft.print(gateName);
  }
  http.end();
}

void sendDataRfid(String rfid) {
  // Buat objek HTTPClient
  String qr = rfid;
  HTTPClient http;
  Serial.println("http://192.168.0.100/goalpara/gate_rfid?" + qr);
  http.begin(endpointRf);
  String gId = "id";
  String postData = "idgate=" + gateID + "&rfid=" + qr;
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponCode = http.POST(postData);

  if (httpResponCode > 0) {
    String payload = http.getString();
    Serial.print("Response: ");
    Serial.println(payload);
    Serial.println("QR READY: " + qr);
    // Cari posisi 'buka='
    int bukaPos = payload.indexOf("buka=");
    if (bukaPos != -1) {
      // Ambil nilai setelah 'buka=' hingga karakter berikutnya
      String bukaValue = payload.substring(bukaPos + 5, bukaPos + 6);
      Serial.print("Nilai 'buka': ");
      Serial.println(bukaValue);
      if (bukaValue == "1") {
        digitalWrite(btnGate, LOW);
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(30, 170);
        tft.print("AKSES DITERIMA!");
        kode = "";
        delay(3000);
        tft.fillScreen(TFT_BLACK);
        tft.pushImage(0, 0, 320, 70, jpeg5);
        tft.setTextSize(3);
        tft.setCursor(100, 170);
        tft.print(gateName);
      } else {  // ini untuk cadangan gate UTV
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(30, 170);
        tft.print("AKSES DITOLAK!");
        kode = "";
        delay(3000);
        tft.fillScreen(TFT_BLACK);
        tft.pushImage(0, 0, 320, 70, jpeg5);
        tft.setTextSize(3);
        tft.setCursor(100, 170);
        tft.print(gateName);
      }
      kode = "";
    } else {
      Serial.println("Tidak ditemukan 'buka=' dalam respons.");
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(30, 170);
      tft.print("TERJADI KESALAHAN!");
      delay(3000);
      tft.fillScreen(TFT_BLACK);
      tft.pushImage(0, 0, 320, 70, jpeg5);
      tft.setTextSize(3);
      tft.setCursor(100, 170);
      tft.print(gateName);
    }
  } else {
    Serial.print("Error ");
    Serial.println(httpResponCode);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(30, 170);
    tft.print("TERJADI KESALAHAN!");
    delay(3000);
    tft.fillScreen(TFT_BLACK);
    tft.pushImage(0, 0, 320, 70, jpeg5);
    tft.setTextSize(3);
    tft.setCursor(100, 170);
    tft.print(gateName);
  }
  http.end();
}



void setup() {
  // Inisialisasi Serial Monitor
  Serial.begin(115200);
  Serial2.begin(9600);
  SPI.begin();  // init SPI bus
  mfrc522.PCD_Init();
  tft.init();
  tft.setRotation(3);
  tft.setSwapBytes(true);
  tft.setTextSize(3);
  Serial.println(gateName + " wiht ID " + gateID);
  // Inisialisasi WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Koneksi WiFi...");
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 30);
    tft.print("Menghubungkan...");
  }
  Serial.println("Koneksi WiFi berhasil");
  ip = String(WiFi.localIP());
  Serial.println("IP ADD: " + ip);

  // Tampilkan teks selamat datang
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 30);
  tft.print("Wifi Terhubung!");
  Serial.println("Koneksi WiFi berhasil");
  Serial.println("Start");
  // masikin qr code yang udah terdaftar buat test koneksi
  // cekKoneksi("8998989100120");
  delay(1000);

  tft.fillScreen(TFT_BLACK);
  tft.pushImage(0, 0, 320, 70, jpeg5);
  tft.setCursor(100, 170);
  tft.print(gateName);
  delay(1000);

  myserial.begin(9600);
  myserial.setTimeout(100);

  pinMode(btnQr2, OUTPUT);
  pinMode(btnGate, OUTPUT);
  digitalWrite(btnQr2, HIGH);
  digitalWrite(btnGate, HIGH);

  //testing server connection
}


void loop() {

  kode = myserial.readString();
  kode.trim();
  Serial.println("QR Code: " + kode);
  if (!kode.isEmpty()) {
    sendData(String(kode));
    kode = "";
  } else {
    Serial.println("Kode QR Kosong!");
    //tft.setCursor(10, 30);
    // tft.print("Gelang Tidak Terbaca!");
    digitalWrite(btnGate, HIGH);
  }

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  id = "";
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.println("Message : ");
  content.toUpperCase();
  id = content.substring(1);
  Serial.println("RFID : " + id);
  if (!id.isEmpty()) {
    //send data ke server
    // buka ini jika BE sudah aman
    sendDataRfid(id);
  } else {
    Serial.println("RFID Kosong!");
  }
  delay(600);
}

void cekRfid(String rfid) {
  Serial.println("RFID: " + rfid);
}
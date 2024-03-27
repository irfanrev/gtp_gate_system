
# GTP Gate System


## Library yang digunakan
- TFT_eSPI
- SPI
- MFRC522
- SoftwareSerial

## Pin yang digunakan
// pin untuk RFID
- SS_PIN 21
- RST_PIN 22
// pin motor gate
- pin 2


## API Reference

#### Get all items

```http
  Ganti Gate ID
```

| Code | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `const String gateID = "1";` | `string` | **Required**. Ganti gate id di sini, sesuaikan dengan data id gate terbaru |

#### Get item

```http
  Ganti Gate Name
```

| Parameter | Type     | Description                       |
| :-------- | :------- | :-------------------------------- |
| `const String gateName = "CHECK IN";`      | `string` | **Required**. Ganti nama gate secara full bukan kode gate |




#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN 9
#define SS_PIN 10
#define BUZZER_PIN 8
#define SERVO_PIN 7

MFRC522 rfid(SS_PIN, RST_PIN);
Servo myServo;

// Replace this with your real UID
String authorizedUIDs[] = {
  "C6 E3 41 02"
};
int numberOfAuthorizedUids = sizeof(authorizedUIDs) / sizeof(authorizedUIDs[0]);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  delay(50);

  myServo.attach(SERVO_PIN);
  myServo.write(0); // start locked
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.println("Scan your card...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String scannedUID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) scannedUID += "0";
    scannedUID += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) scannedUID += " ";
  }
  scannedUID.toUpperCase();

  Serial.print("Scanned UID: ");
  Serial.println(scannedUID);

  bool isAuthorized = false;
  for (int i = 0; i < numberOfAuthorizedUids; i++) {
    if (scannedUID.equals(authorizedUIDs[i])) {
      isAuthorized = true;
      break;
    }
  }

  if (isAuthorized) {
    Serial.println("✅ Authorized Card Detected");
    digitalWrite(BUZZER_PIN, HIGH);   
    delay(500);              // 2 sec
    digitalWrite(BUZZER_PIN, LOW);
    myServo.write(0);        // rotate
    delay(3000);              // 2 sec
    myServo.write(90);         // back to 0
  } else {
    Serial.println("❌ Unauthorized Card");
    digitalWrite(BUZZER_PIN, HIGH);   
    myServo.write(0);            // stay locked
    delay(2000);
    digitalWrite(BUZZER_PIN, LOW);              // 2 sec
  }

  delay(1000);
  rfid.PICC_HaltA();
}

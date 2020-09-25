#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9

Servo myservo;
LiquidCrystal_I2C lcd(0x27,16,2);
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522 mfrc522(SS_PIN, RST_PIN);  
MFRC522::MIFARE_Key key; 

int button123 = 6;
// Init array that will store new NUID 
byte saveByte[4];
byte nuidPICC[4];
byte whatByte[4];
byte heart[8] = { B00000, B01010, B11111, B11111, B01110, B00100, B00000, B00000};

void setup() { 
  lcd.init(); // initialize the lcd 
  lcd.backlight(); // LCD Backlight on
  lcd.createChar(0, heart); //heart icon 
  pinMode(button123,INPUT_PULLUP);
  pinMode(4, OUTPUT);
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  myservo.attach(3);
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  myservo.write(30);
  delay(300);
 // myservo.detach();
  myservo.write(90);
}

void loop() {
  Serial.println(myservo.read());
  saveByte[0]=0xF2;
  saveByte[1]=0x13;
  saveByte[2]=0x50;
  saveByte[3]=0xC3;
  int ftt=0;

  if(digitalRead(button123)==1){
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
      delay(50);
      return;
    }
  // Now a card is selected. The UID and SAK is in mfrc522.uid.
  // Dump UID
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    whatByte[i]=(mfrc522.uid.uidByte[i]);
  } 
  Serial.println();

  byte newUid[4];
  // Set new UID
  newUid[0] = (rfid.uid.uidByte[0]);
  newUid[1] = (rfid.uid.uidByte[1]);
  newUid[2] = (rfid.uid.uidByte[2]); 
  newUid[3] = (rfid.uid.uidByte[3]);
///////////////////////////////////////////////////////


  //카드의 UID가 정상적으로 바뀌었다면 FALSE
  if ( mfrc522.MIFARE_SetUid(newUid, (byte)4, true) ) {
    Serial.println(F("Wrote new UID to card."));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(0); 
    lcd.print("FAKE CARD");
     for(int i=0; i<3;i++){
       int melody[]={330,330,330}; 
       whatByte[i]=0;
       tone(4,melody[i]);
       delay(200);
       noTone(4);
     } 
     for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
      }
   
  Serial.println(F("The NUID tag is:"));
  Serial.print(F("In hex: "));
  printHex(rfid.uid.uidByte, rfid.uid.size);
  }
     
  saveByte[0]=0xF2;
  saveByte[1]=0x13;
  saveByte[2]=0x50;
  saveByte[3]=0xC3;
  
//저장 된 UID와 입력 된 카드의 UID가 일치하면 OPEN
  if(saveByte[0]==whatByte[0]&&
    saveByte[1]==whatByte[1]&&
    saveByte[2]==whatByte[2]&&
    saveByte[3]==whatByte[3]){
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("OPEN THE DOOR");
    int melody[]={262,330,392,523};
    for(int i=0; i<4;i++){
      tone(4,melody[i]);
      delay(200);
      noTone(4);
    } 

  
    myservo.write(120);
    delay(800);
 // myservo.detach();
    myservo.write(90);
    delay(5000);
    myservo.write(30);
    delay(300);
    myservo.write(90);
    }
  
    if(saveByte[0]!=whatByte[0]||
    saveByte[1]!=whatByte[1]||
    saveByte[2]!=whatByte[2]||
    saveByte[3]!=whatByte[3]){
      int melody[]={330,330,330};
      lcd.setCursor(0, 0);
      lcd.clear();
      myservo.attach(3);
      lcd.print("Failed");
        for(int i=0; i<3;i++){
          tone(4,melody[i]);
          delay(200);
          noTone(4);
        }
    } 
  // Halt PICC and re-select it so DumpToSerial doesn't get confused
  mfrc522.PICC_HaltA();
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    return;
  }
  
  // Dump the new memory contents
  Serial.println(F("New UID and contents:"));
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
 
  delay(2000);
  }
}


void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

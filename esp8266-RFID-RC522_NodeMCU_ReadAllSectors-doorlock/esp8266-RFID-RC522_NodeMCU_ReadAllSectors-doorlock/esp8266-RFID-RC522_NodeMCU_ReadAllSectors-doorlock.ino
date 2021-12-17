#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN D8
#define RST_PIN D0  
MFRC522 mfrc522(SS_PIN, RST_PIN);
void setup()
{
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();   // آماده کردن MFRC522 
  Serial.println("کارت خود را نزدیک ماژول کنید...");
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(10,INPUT);
  pinMode(2,INPUT);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);

}
void loop()
{
  if(digitalRead(10))
  {
  Serial.print("10");
  }
 if(digitalRead(2))
   Serial.print("2");
  //بررسی کارت جدید
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // انتخاب و خواندن کارت
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //نمایش شناسه UID کارت
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "C6 3E 7D 2B") //شناسه UID کارت خود را اینجا جایگزین کنید
  {
    Serial.println("کارت مورد تایید است.");
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    delay(1000);
  }
  else
  {
    Serial.println(" کارت غیر مجاز");
    delay(1000);
  }
}

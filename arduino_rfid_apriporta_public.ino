/*
  Con questo sketch è possibile fare accendere un led o commutare un relè
  al passaggio di una tessera magnetica sul lettore RFID.
  Se la tessera viene riconosciuta si accende un led verde e commuta il relè,
  altrimenti un led rosso per indicarci l`inaccessibilità.

  PINOUT:

  RC522 MODULO    Leonardo/DuemilaNove
  SDA             D10
  SCK             ICSP3
  MOSI            ICSP4
  MISO            ICSP1
  IRQ             N/A
  GND             GND
  RST             RESET
  3.3V            3.3V

  Led verde al Pin 7
  Led rosso al Pin 8
  Relè 1 al Pin 4
  Relè 2 al Pin 2
*/

#include <SPI.h>
#include <MFRC522.h>

/* Vengono definiti PIN del RFID reader*/
#define SDA_DIO 10  // Pin 53 per Arduino Mega
#define RESET_DIO 13
#define delayRelay 350  // Tempo (in millisecondi) relè in stato ON
#define delayLed 2000

#define ledVerde 9
#define ledRosso 7

#define RELAY1  2
#define RELAY2  4

MFRC522 rfid(SDA_DIO, RESET_DIO); // Instance of the class

MFRC522::MIFARE_Key key;

// inserire tutti i codici esadecimali delle schede riconosciute
String codiceAutorizzato1 = "[CHIAVE_TESSERA]"; 

void setup()
{
  Serial.begin(9600);
  /* Abilita SPI*/
  SPI.begin();
  /* Viene inizilizzato RFID reader */
  rfid.PCD_Init();

  //If you set Antenna Gain to Max it will increase reading distance
  //rfid.PCD_SetAntennaGain(mfrc522.RxGain_max);
  Serial.println("Setup");

  pinMode(ledVerde, OUTPUT);
  pinMode(ledRosso, OUTPUT);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  digitalWrite (RELAY1, HIGH);
  digitalWrite (RELAY2, HIGH);

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  Serial.println(hex2str(key.keyByte, MFRC522::MF_KEY_SIZE));
}

void loop()
{
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  // Store NUID
  //Serial.println("Codice delle tessera letto:");
  String codiceLetto = hex2str(rfid.uid.uidByte, rfid.uid.size);
  //Serial.println(codiceLetto);

  if (verificaCodice(codiceLetto, codiceAutorizzato1)) {
    //Serial.println("Tessera autorizzata");
    //accendiLed(ledVerde);
    digitalWrite (RELAY1, LOW);
    digitalWrite (RELAY2, LOW);

    delay (delayRelay);
    digitalWrite (RELAY1, HIGH);
    digitalWrite (RELAY2, HIGH);
  } else {
    //Serial.println("Tessera non autorizzata");

    //accendiLed(ledRosso);
    digitalWrite (RELAY1, HIGH);
    digitalWrite (RELAY2, HIGH);

    delay (delayRelay);
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

/**
   Helper routine to dump a byte array as hex values to Serial.
*/
String hex2str(byte *buffer, byte bufferSize) {
  String tmp = "";
  for (byte i = 0; i < bufferSize; i++) {
    tmp += String (buffer[i], HEX);
  }
  tmp.toUpperCase();
  return tmp;
}


// Questa funzione verifica se il codice Letto è autorizzato
boolean verificaCodice(String codiceLetto, String codiceAutorizzato) {
  if (codiceLetto.equals(codiceAutorizzato)) {
    return true;
  } else {
    return false;
  }
}

// Questa funzione permette di accendere un LED per un determinato periodo
void accendiLed(int ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(delayLed);
  digitalWrite(ledPin, LOW);
}


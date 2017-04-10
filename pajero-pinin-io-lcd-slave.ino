#include <SPI.h>

uint8_t TIME[6];
uint8_t TIME_BUFFER[6];

uint8_t TEMPERATURE[2];
uint8_t TEMPERATURE_BUFFER[2];


volatile uint8_t OFFSET;


void setup() {
    OFFSET = 0;
    Serial.begin(250000);
    SPI.setDataMode (SPI_MODE3);
    SPI.setBitOrder (LSBFIRST);
    SPCR |= bit (SPE);
    pinMode (MISO, OUTPUT);
    SPI.attachInterrupt();
}




ISR(SPI_STC_vect) {
  uint8_t value = SPDR;
  switch (OFFSET++) {
    case 0: if (value != 0x45) OFFSET = 0; break;
    case 4: TEMPERATURE_BUFFER[0] = value; break;
    case 5: TEMPERATURE_BUFFER[1] = value; break;
    case 10: if (value >> 6 != 0b00) OFFSET = 0; break;
    case 11: if (value != 0x45) OFFSET = 0; break;
//    case 18: METERAGE_BUFFER[0] = value; break;
//    case 19: METERAGE_BUFFER[1] = value; break;
//    case 20: METERAGE_BUFFER[2] = value; break;
    case 21: if (value >> 6 != 0b10) OFFSET = 0; break;
    case 22: if (value != 0x45) OFFSET = 0; break;
//    case 23: METERAGE_BUFFER[3] = value; break;
//    case 24: METERAGE_BUFFER[4] = value; break;
//    case 28: METERAGE_BUFFER[5] = value; break;
    case 32: if (value >> 6 != 0b01) OFFSET = 0; break;
    case 33: if (value != 0x45) OFFSET = 0; break;
    case 36: TIME_BUFFER[0] = value; break;
    case 37: TIME_BUFFER[1] = value; break;
    case 38: TIME_BUFFER[2] = value; break;
    case 39: TIME_BUFFER[3] = value; break;
    case 40: TIME_BUFFER[4] = value; break;
    case 41: TIME_BUFFER[5] = value; break;
    case 43: if (value >> 6 != 0b11) OFFSET = 0; else SPI.detachInterrupt(); break;
  }
}


char convertDigitToNumber(bool b, bool bl, bool m, bool tl, bool br, bool tr, bool t) {
    switch ((b << 7) | (bl << 6) | (m << 5) | (tl << 4) | (br << 3) | (tr << 2) | t) {
      case 32: return '-';
      case 221: return '0';
      case 12: return '1';
      case 229: return '2';
      case 173: return '3';
      case 60: return '4';
      case 185: return '5';
      case 249: return '6';
      case 13: return '7';
      case 253: return '8';
      case 189: return '9';
    }
}

void loop() {
  
  if (OFFSET == 44) {
    
    if (memcmp(TIME, TIME_BUFFER, 6)) {
      Serial.println("TIME_CHANGED");
      memcpy(TIME, TIME_BUFFER, 6);


      Serial.print(
        bitRead(TIME[0], 1) ? "1" : "0"
      );
     
      Serial.print(
        convertDigitToNumber(
          bitRead(TIME[0], 2),
          bitRead(TIME[0], 3),
          bitRead(TIME[0], 6),
          bitRead(TIME[0], 7),
          bitRead(TIME[1], 2),
          bitRead(TIME[1], 3),
          bitRead(TIME[1], 7)
        )
      );


      Serial.print(":");


      Serial.print(
        convertDigitToNumber(
          bitRead(TIME[2], 2),
          bitRead(TIME[2], 3),
          bitRead(TIME[2], 6),
          bitRead(TIME[2], 7),
          bitRead(TIME[3], 2),
          bitRead(TIME[3], 3),
          bitRead(TIME[3], 7)
        )
      );

      Serial.println(
        convertDigitToNumber(
          bitRead(TIME[4], 2),
          bitRead(TIME[4], 3),
          bitRead(TIME[4], 6),
          bitRead(TIME[4], 7),
          bitRead(TIME[5], 2),
          bitRead(TIME[5], 3),
          bitRead(TIME[5], 7)
        )
      );

      
      
    }

    if (memcmp(TEMPERATURE, TEMPERATURE_BUFFER, 2)) {
      memcpy(TEMPERATURE, TEMPERATURE_BUFFER, 2);

      char temp[4] = {
        bitRead(TEMPERATURE[0], 3) ? '-' : ' ',
        convertDigitToNumber(
          bitRead(TEMPERATURE[0], 7),
          bitRead(TEMPERATURE[0], 2),
          bitRead(TEMPERATURE[0], 5),
          bitRead(TEMPERATURE[0], 1),
          bitRead(TEMPERATURE[0], 6),
          bitRead(TEMPERATURE[0], 4),
          bitRead(TEMPERATURE[0], 0)
        ),
        convertDigitToNumber(
          bitRead(TEMPERATURE[1], 7),
          bitRead(TEMPERATURE[1], 2),
          bitRead(TEMPERATURE[1], 5),
          bitRead(TEMPERATURE[1], 1),
          bitRead(TEMPERATURE[1], 6),
          bitRead(TEMPERATURE[1], 4),
          bitRead(TEMPERATURE[1], 0)
        )
      };

      Serial.println(temp);
      
    
    
    }

    
    OFFSET = 0;
    SPI.attachInterrupt();
  }
  
}


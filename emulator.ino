#include <SPI.h>

uint8_t BUFFER[4][11];
volatile bool READY = false;
volatile uint8_t BUFFER_INDEX = 0;
volatile uint8_t BUFFER_POS = 0;

uint8_t CLOCK_1 = 0;
uint8_t CLOCK_2 = 0;
uint8_t CLOCK_3 = 0;
uint8_t CLOCK_4 = 0;

void setup() {
    Serial.begin(250000);
    SPI.setDataMode (SPI_MODE3);
    SPI.setBitOrder (LSBFIRST);

    SPCR |= bit (SPE);
    pinMode (MISO, OUTPUT);

    BUFFER_INDEX = 0;
    BUFFER_POS = 0;
    READY = false;


    SPI.attachInterrupt();

}



ISR (SPI_STC_vect) {

  if (READY == false) {
    BUFFER[BUFFER_INDEX][BUFFER_POS] = SPDR;
    
    BUFFER_POS++;
  
    if (BUFFER_POS == 11) {
      BUFFER_POS = 0;
      BUFFER_INDEX++;
      if (BUFFER_INDEX == 4) {
        READY = true;
      }
    }
  }

}


uint8_t convertDigitToNumber(uint8_t digit) {
    switch (digit) {
      case 63: return 0;
      case 6: return 1;
      case 91: return 2;
      case 79: return 3;
      case 102: return 4;
      case 109: return 5;
      case 125: return 6;
      case 7: return 7;
      case 127: return 8;
      case 111: return 9;
    }
}



void loop() {

  if (READY == true) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 11; j++) {
        for (int b = 0; b < 8; b++) {
            
            uint8_t bitNum = (j * 8 + b) - 8;
            bool bitVal = bitRead(BUFFER[i][j], b);

  
            if (i == 3 && bitNum == 17) {
              bitWrite(CLOCK_1, 1, bitVal);
              bitWrite(CLOCK_1, 2, bitVal);
            }

            if (i == 3 && bitNum == 18) {
              bitWrite(CLOCK_2, 3, bitVal);
            }

            if (i == 3 && bitNum == 19) {
              bitWrite(CLOCK_2, 4, bitVal);
            }

            if (i == 3 && bitNum == 22) {
              bitWrite(CLOCK_2, 6, bitVal);
            }

            if (i == 3 && bitNum == 23) {
              bitWrite(CLOCK_2, 5, bitVal);
            }

            if (i == 3 && bitNum == 26) {
              bitWrite(CLOCK_2, 2, bitVal);
            }

            if (i == 3 && bitNum == 27) {
              bitWrite(CLOCK_2, 1, bitVal);
            }


            if (i == 3 && bitNum == 31) {
              bitWrite(CLOCK_2, 0, bitVal);
            }





            if (i == 3 && bitNum == 34) {
              bitWrite(CLOCK_3, 3, bitVal);
            }

            if (i == 3 && bitNum == 35) {
              bitWrite(CLOCK_3, 4, bitVal);
            }

            if (i == 3 && bitNum == 38) {
              bitWrite(CLOCK_3, 6, bitVal);
            }

            if (i == 3 && bitNum == 39) {
              bitWrite(CLOCK_3, 5, bitVal);
            }

            if (i == 3 && bitNum == 42) {
              bitWrite(CLOCK_3, 2, bitVal);
            }

            if (i == 3 && bitNum == 43) {
              bitWrite(CLOCK_3, 1, bitVal);
            }

            if (i == 3 && bitNum == 47) {
              bitWrite(CLOCK_3, 0, bitVal);
            }





            if (i == 3 && bitNum == 50) {
              bitWrite(CLOCK_4, 3, bitVal);
            }

            if (i == 3 && bitNum == 51) {
              bitWrite(CLOCK_4, 4, bitVal);
            }

            if (i == 3 && bitNum == 54) {
              bitWrite(CLOCK_4, 6, bitVal);
            }

            if (i == 3 && bitNum == 55) {
              bitWrite(CLOCK_4, 5, bitVal);
            }

            if (i == 3 && bitNum == 58) {
              bitWrite(CLOCK_4, 2, bitVal);
            }

            if (i == 3 && bitNum == 59) {
              bitWrite(CLOCK_4, 1, bitVal);
            }

            if (i == 3 && bitNum == 63) {
              bitWrite(CLOCK_4, 0, bitVal);
            }



            /*
           if (!bitVal) {
              Serial.print("0");
           } else {


            
            Serial.print("1");

             
           }
           */
        }
      }
//      Serial.println("\n");
    }

    Serial.print(convertDigitToNumber(CLOCK_1));
    Serial.print(convertDigitToNumber(CLOCK_2));
    Serial.print(":");
    Serial.print(convertDigitToNumber(CLOCK_3));
    Serial.print(convertDigitToNumber(CLOCK_4));
    Serial.println("\n");
    

    
    BUFFER_INDEX = 0;
    BUFFER_POS = 0;
    READY = false;

  }
  
}


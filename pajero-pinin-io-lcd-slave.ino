#include <SPI.h>

// запас топлива, км
#define FUEL_KM 0b00000010
// запас топлива, миль
#define FUEL_MILES 0b10000000
// скорость, км/ч
#define SPEED_KMH 0b00000011
// скорость, миль/ч
#define SPEED_MPH 0b00010000
// расход, км/л
#define CONSUMPTION_KML 0b01100000
// расход, миль/галлон
#define CONSUMPTION_MPG 0b00001000
// расход, л/100км
#define CONSUMPTION_L100KM 0b01000100



uint32_t RAW_TIME;
uint32_t RAW_TEMPERATURE;
uint32_t RAW_FUEL;
uint32_t RAW_SPEED;
uint32_t RAW_CONSUMPTION;

volatile uint8_t OFFSET;
volatile uint32_t SPI_TIME;
volatile uint32_t SPI_METERAGE;
volatile uint32_t SPI_TEMPERATURE;
volatile uint8_t SPI_METERAGE_UNIT;


void setup() {
	OFFSET = 0;
	Serial.begin(250000);
	SPCR |= bit (SPE);
	SPI.setBitOrder(LSBFIRST);
	SPI.setDataMode(SPI_MODE3);
	SPI.attachInterrupt();
}



#define T_D1(value, bit) ((value >> bit & 1) << 0)
#define C_D1(value, bit) ((value >> bit & 1) << 1)
#define B_D1(value, bit) ((value >> bit & 1) << 2)
#define TL_D1(value, bit) ((value >> bit & 1) << 3)
#define TR_D1(value, bit) ((value >> bit & 1) << 4)
#define BL_D1(value, bit) ((value >> bit & 1) << 5)
#define BR_D1(value, bit) ((value >> bit & 1) << 6)

#define T_D2(value, bit) ((value >> bit & 1) << 7)
#define C_D2(value, bit) ((value >> bit & 1) << 8)
#define B_D2(value, bit) ((value >> bit & 1) << 9)
#define TL_D2(value, bit) ((value >> bit & 1) << 10)
#define TR_D2(value, bit) ((value >> bit & 1) << 11)
#define BL_D2(value, bit) ((value >> bit & 1) << 12)
#define BR_D2(value, bit) ((value >> bit & 1) << 13)

#define T_D3(value, bit) ((value >> bit & 1) << 14)
#define C_D3(value, bit) ((value >> bit & 1) << 15)
#define B_D3(value, bit) ((value >> bit & 1) << 16)
#define TL_D3(value, bit) ((value >> bit & 1) << 17)
#define TR_D3(value, bit) ((value >> bit & 1) << 18)
#define BL_D3(value, bit) ((value >> bit & 1) << 19)
#define BR_D3(value, bit) ((value >> bit & 1) << 20)

#define D4(value, bit) ((value >> bit & 1) << 21)
#define FLOAT_D(value, bit) ((value >> bit & 1) << 22)
#define NEGATIVE_D(value, bit) ((value >> bit & 1) << 23)

uint32_t bits_to_digit(uint32_t value) {
	switch (value) {
		case 125: return 0;
		case 80: return 1;
		case 55: return 2;
		case 87: return 3;
		case 90: return 4;
		case 79: return 5;
		case 111: return 6;
		case 81: return 7;
		case 127: return 8;
		case 95: return 9;
	}
}

float uint32_to_float(uint32_t value) {

	float result = (
		((value >> 21) & 1) * 1000 +
		bits_to_digit((value >> 14) & 0x7F) * 100 +
		bits_to_digit((value >> 7) & 0x7F) * 10 +
		bits_to_digit(value & 0x7F)
	);

	if ((value >> 22) & 1) result /= 10;
	if ((value >> 23) & 1) result = -result;

	return result;
}


ISR(SPI_STC_vect) {
	uint8_t value = SPDR;
	switch (OFFSET++) {

		case 0: if (value != 0x45) OFFSET = 0; break;
		case 4: SPI_TEMPERATURE = T_D2(value, 0) | TL_D2(value, 1) | BL_D2(value, 2) | NEGATIVE_D(value, 3) | TR_D2(value, 4) | C_D2(value, 5) | BR_D2(value, 6) | B_D2(value, 7); break;
		case 5: SPI_TEMPERATURE |= T_D1(value, 0) | TL_D1(value, 1) | BL_D1(value, 2) | TR_D1(value, 4) | C_D1(value, 5) | BR_D1(value, 6) | B_D1(value, 7); break;
		case 10: if (value >> 6 != 0b00) OFFSET = 0; break;

		case 11: if (value != 0x45) OFFSET = 0; break;
		case 18: SPI_METERAGE = T_D3(value, 4) | TL_D3(value, 5) | BL_D3(value, 6) | D4(value, 7); break;
		case 19: SPI_METERAGE |= TR_D3(value, 0) | C_D3(value, 1) | BR_D3(value, 2) | B_D3(value, 3) | T_D2(value, 4) | TL_D2(value, 5) | BL_D2(value, 6); break;
		case 20: SPI_METERAGE |= TR_D2(value, 0) | C_D2(value, 1) | BR_D2(value, 2) | B_D2(value, 3); break;
		case 21: if (value >> 6 != 0b10) OFFSET = 0; break;

		case 22: if (value != 0x45) OFFSET = 0; break;
		case 23: SPI_METERAGE |= T_D1(value, 0) | TL_D1(value, 1) | BL_D1(value, 2) | TR_D1(value, 4) | C_D1(value, 5) | BR_D1(value, 6) | B_D1(value, 7); break;
		case 24: SPI_METERAGE_UNIT = value; break;
		case 28: SPI_METERAGE |= FLOAT_D(value, 7); break;
		case 32: if (value >> 6 != 0b01) OFFSET = 0; break;

		case 33: if (value != 0x45) OFFSET = 0; break;
		case 36: SPI_TIME = D4(value, 1) | B_D3(value, 2) | BL_D3(value, 3) | C_D3(value, 6) | TL_D3(value, 7); break;
		case 37: SPI_TIME |= BR_D3(value, 2) | TR_D3(value, 3) | T_D3(value, 7); break;
		case 38: SPI_TIME |= B_D2(value, 2) | BL_D2(value, 3) | C_D2(value, 6) | TL_D2(value, 7); break;
		case 39: SPI_TIME |= BR_D2(value, 2) | TR_D2(value, 3) | T_D2(value, 7); break;
		case 40: SPI_TIME |= B_D1(value, 2) | BL_D1(value, 3) | C_D1(value, 6) | TL_D1(value, 7); break;
		case 41: SPI_TIME |= BR_D1(value, 2) | TR_D1(value, 3) | T_D1(value, 7); break;
		case 43: if (value >> 6 != 0b11) OFFSET = 0; else SPI.detachInterrupt(); break;

	}
}


void loop() {

	if (OFFSET == 44) {

		if (RAW_TIME != SPI_TIME) {
			RAW_TIME = SPI_TIME;
			Serial.print("NEW_TIME: ");
			Serial.println(uint32_to_float(RAW_TIME));
		}

		if (RAW_TEMPERATURE != SPI_TEMPERATURE) {
			RAW_TEMPERATURE = SPI_TEMPERATURE;
			Serial.print("NEW_TEMPERATURE: ");
			Serial.println(uint32_to_float(RAW_TEMPERATURE));
		}

		if (SPI_METERAGE_UNIT == FUEL_KM ||
			SPI_METERAGE_UNIT == FUEL_MILES) {
			if (RAW_FUEL != SPI_METERAGE) {
				RAW_FUEL = SPI_METERAGE;
				Serial.print("NEW_FUEL: ");
				Serial.println(uint32_to_float(RAW_FUEL));
			}
		}

		else if (SPI_METERAGE_UNIT == SPEED_KMH ||
				SPI_METERAGE_UNIT == SPEED_MPH) {
			if (RAW_SPEED != SPI_METERAGE) {
				RAW_SPEED = SPI_METERAGE;
				Serial.print("NEW_SPEED: ");
				Serial.println(uint32_to_float(RAW_SPEED));
			}
		}

		else if (SPI_METERAGE_UNIT == CONSUMPTION_KML ||
				SPI_METERAGE_UNIT == CONSUMPTION_MPG ||
				SPI_METERAGE_UNIT == CONSUMPTION_L100KM) {
			if (RAW_CONSUMPTION != SPI_METERAGE) {
				RAW_CONSUMPTION = SPI_METERAGE;
				Serial.print("NEW_CONSUMPTION: ");
				Serial.println(uint32_to_float(RAW_CONSUMPTION));
			}
		}

		OFFSET = 0;
		SPI.attachInterrupt();
	}

}


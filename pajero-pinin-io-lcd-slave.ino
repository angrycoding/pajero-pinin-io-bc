#include <SPI.h>
#include "LCD.h"

// признак начала пакета
#define SPI_PACKET_START 0x45
// признак окончания первого пакета
#define SPI_PACKET1_END 0b00
// признак окончания второго пакета
#define SPI_PACKET2_END 0b10
// признак окончания третьего пакета
#define SPI_PACKET3_END 0b01
// признак окончания четвертого пакета
#define SPI_PACKET4_END 0b11

#define SPI_STATE_START 0
#define SPI_STATE_DONE 44

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



float BC_TIME;
float BC_TEMPERATURE;
float BC_FUEL;
float BC_SPEED;
float BC_CONSUMPTION;

volatile uint8_t SPI_STATE;

volatile uint32_t LCD_TIME;
volatile uint32_t LCD_METERAGE;
volatile uint32_t LCD_TEMPERATURE;
volatile uint8_t SPI_METERAGE_UNIT;

void attachSlaveInterrupt() {
	LCD_TIME = 0;
	LCD_METERAGE = 0;
	LCD_TEMPERATURE = 0;
	SPI_METERAGE_UNIT = 0;
	SPI_STATE = SPI_STATE_START;
	SPI.attachInterrupt();
}

void setup() {
	Serial.begin(115200);
	SPCR |= bit(SPE);
	SPI.setBitOrder(LSBFIRST);
	SPI.setDataMode(SPI_MODE3);
	attachSlaveInterrupt();
}

ISR(SPI_STC_vect) {
	uint8_t value = SPDR;
	switch (SPI_STATE++) {
		case 0: if (value != SPI_PACKET_START) SPI_STATE = SPI_STATE_START; break;
		case 4: LCD_TEMPERATURE = LCD_10T(value, 0) | LCD_10TL(value, 1) | LCD_10BL(value, 2) | LCD_MINUS(value, 3) | LCD_10TR(value, 4) | LCD_10C(value, 5) | LCD_10BR(value, 6) | LCD_10B(value, 7); break;
		case 5: LCD_TEMPERATURE |= LCD_1T(value, 0) | LCD_1TL(value, 1) | LCD_1BL(value, 2) | LCD_1TR(value, 4) | LCD_1C(value, 5) | LCD_1BR(value, 6) | LCD_1B(value, 7); break;
		case 10: if (value >> 6 != SPI_PACKET1_END) SPI_STATE = SPI_STATE_START; break;
		case 11: if (value != SPI_PACKET_START) SPI_STATE = SPI_STATE_START; break;
		case 18: LCD_METERAGE = LCD_100T(value, 4) | LCD_100TL(value, 5) | LCD_100BL(value, 6) | LCD_1000(value, 7); break;
		case 19: LCD_METERAGE |= LCD_100TR(value, 0) | LCD_100C(value, 1) | LCD_100BR(value, 2) | LCD_100B(value, 3) | LCD_10T(value, 4) | LCD_10TL(value, 5) | LCD_10BL(value, 6); break;
		case 20: LCD_METERAGE |= LCD_10TR(value, 0) | LCD_10C(value, 1) | LCD_10BR(value, 2) | LCD_10B(value, 3); break;
		case 21: if (value >> 6 != SPI_PACKET2_END) SPI_STATE = SPI_STATE_START; break;
		case 22: if (value != SPI_PACKET_START) SPI_STATE = SPI_STATE_START; break;
		case 23: LCD_METERAGE |= LCD_1T(value, 0) | LCD_1TL(value, 1) | LCD_1BL(value, 2) | LCD_1TR(value, 4) | LCD_1C(value, 5) | LCD_1BR(value, 6) | LCD_1B(value, 7); break;
		case 24: SPI_METERAGE_UNIT = value; break;
		case 28: LCD_METERAGE |= LCD_DOT(value, 7); break;
		case 32: if (value >> 6 != SPI_PACKET3_END) SPI_STATE = SPI_STATE_START; break;
		case 33: if (value != SPI_PACKET_START) SPI_STATE = SPI_STATE_START; break;
		case 36: LCD_TIME = LCD_1000(value, 1) | LCD_100B(value, 2) | LCD_100BL(value, 3) | LCD_100C(value, 6) | LCD_100TL(value, 7); break;
		case 37: LCD_TIME |= LCD_100BR(value, 2) | LCD_100TR(value, 3) | LCD_100T(value, 7); break;
		case 38: LCD_TIME |= LCD_10B(value, 2) | LCD_10BL(value, 3) | LCD_10C(value, 6) | LCD_10TL(value, 7); break;
		case 39: LCD_TIME |= LCD_10BR(value, 2) | LCD_10TR(value, 3) | LCD_10T(value, 7); break;
		case 40: LCD_TIME |= LCD_1B(value, 2) | LCD_1BL(value, 3) | LCD_1C(value, 6) | LCD_1TL(value, 7); break;
		case 41: LCD_TIME |= LCD_1BR(value, 2) | LCD_1TR(value, 3) | LCD_1T(value, 7); break;
		case 43: if (value >> 6 != SPI_PACKET4_END) SPI_STATE = SPI_STATE_START; else SPI.detachInterrupt(); break;
	}
}






void loop() {

	if (SPI_STATE == SPI_STATE_DONE) {

		float newTime = LCD_getValue(LCD_TIME);
		float newMeterage = LCD_getValue(LCD_METERAGE);
		float newTemperature = LCD_getValue(LCD_TEMPERATURE);

		if (BC_TIME != newTime) {
			BC_TIME = newTime;
			Serial.print("NEW_TIME: ");
			Serial.println(BC_TIME);
		}

		if (BC_TEMPERATURE != newTemperature) {
			BC_TEMPERATURE = newTemperature;
			Serial.print("NEW_TEMPERATURE: ");
			Serial.println(BC_TEMPERATURE);
		}

		if (SPI_METERAGE_UNIT == FUEL_KM ||
			SPI_METERAGE_UNIT == FUEL_MILES) {
			if (BC_FUEL != newMeterage) {
				BC_FUEL = newMeterage;
				Serial.print("NEW_FUEL: ");
				Serial.println(BC_FUEL);
			}
		}

		else if (SPI_METERAGE_UNIT == SPEED_KMH ||
			SPI_METERAGE_UNIT == SPEED_MPH) {
			if (BC_SPEED != newMeterage) {
				BC_SPEED = newMeterage;
				Serial.print("NEW_SPEED: ");
				Serial.println(BC_SPEED);
			}
		}

		else if (SPI_METERAGE_UNIT == CONSUMPTION_KML ||
			SPI_METERAGE_UNIT == CONSUMPTION_MPG ||
			SPI_METERAGE_UNIT == CONSUMPTION_L100KM) {
			if (BC_CONSUMPTION != newMeterage) {
				BC_CONSUMPTION = newMeterage;
				Serial.print("NEW_CONSUMPTION: ");
				Serial.println(BC_CONSUMPTION);
			}
		}

		attachSlaveInterrupt();
	}

}

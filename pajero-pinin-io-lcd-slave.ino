#include <SPI.h>

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

// единицы - верхний сегмент
#define D0_T(value, bit) ((value >> bit & 1) << 0)
// единицы - средний сегмент
#define D0_C(value, bit) ((value >> bit & 1) << 1)
// единицы - нижний сегмент
#define D0_B(value, bit) ((value >> bit & 1) << 2)
// единицы - верхний левый сегмент
#define D0_TL(value, bit) ((value >> bit & 1) << 3)
// единицы - верхний правый сегмент
#define D0_TR(value, bit) ((value >> bit & 1) << 4)
// единицы - нижний левый сегмент
#define D0_BL(value, bit) ((value >> bit & 1) << 5)
// единицы - нижний правый сегмент
#define D0_BR(value, bit) ((value >> bit & 1) << 6)
// десятки - верхний сегмент
#define D1_T(value, bit) ((value >> bit & 1) << 7)
// десятки - средний сегмент
#define D1_C(value, bit) ((value >> bit & 1) << 8)
// десятки - нижний сегмент
#define D1_B(value, bit) ((value >> bit & 1) << 9)
// десятки - верхний левый сегмент
#define D1_TL(value, bit) ((value >> bit & 1) << 10)
// десятки - верхний правый сегмент
#define D1_TR(value, bit) ((value >> bit & 1) << 11)
// десятки - нижний левый сегмент
#define D1_BL(value, bit) ((value >> bit & 1) << 12)
// десятки - нижний правый сегмент
#define D1_BR(value, bit) ((value >> bit & 1) << 13)
// сотни - верхний сегмент
#define D2_T(value, bit) ((value >> bit & 1) << 14)
// сотни - средний сегмент
#define D2_C(value, bit) ((value >> bit & 1) << 15)
// сотни - нижний сегмент
#define D2_B(value, bit) ((value >> bit & 1) << 16)
// сотни - верхний левый сегмент
#define D2_TL(value, bit) ((value >> bit & 1) << 17)
// сотни - верхний правый сегмент
#define D2_TR(value, bit) ((value >> bit & 1) << 18)
// сотни - нижний левый сегмент
#define D2_BL(value, bit) ((value >> bit & 1) << 19)
// сотни - нижний правый сегмент
#define D2_BR(value, bit) ((value >> bit & 1) << 20)
// тысячи - контроллируется одним сегментом
#define D3(value, bit) ((value >> bit & 1) << 21)
// десятичный разделитель между десятками и единицами
#define D_FLOAT(value, bit) ((value >> bit & 1) << 22)
// знак минуса
#define D_NEGATIVE(value, bit) ((value >> bit & 1) << 23)


uint32_t RAW_TIME;
uint32_t RAW_TEMPERATURE;
uint32_t RAW_FUEL;
uint32_t RAW_SPEED;
uint32_t RAW_CONSUMPTION;

volatile uint8_t SPI_STATE;
volatile uint32_t SPI_TIME;
volatile uint32_t SPI_METERAGE;
volatile uint32_t SPI_TEMPERATURE;
volatile uint8_t SPI_METERAGE_UNIT;


void setup() {
	SPI_STATE = SPI_STATE_START;
	Serial.begin(250000);
	SPCR |= bit(SPE);
	SPI.setBitOrder(LSBFIRST);
	SPI.setDataMode(SPI_MODE3);
	SPI.attachInterrupt();
}




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
		// тут нужно возвращать некий флаг ошибки
		// в ином случае пустые знакоместа 
		// и всякие вещи типа "-" и "HI" будут
		// принудительно приведены к нулю
		default: return 0;
	}
}

float uint32_to_float(uint32_t value) {

	uint32_t d1 = bits_to_digit(value & 0x7F);
	uint32_t d2 = bits_to_digit((value >> 7) & 0x7F);
	uint32_t d3 = bits_to_digit((value >> 14) & 0x7F);

	// нужна обработка ошибок / состояний
	// bits_to_digit - может вернуть

	// - пусто
	// - неопределяемая хуйня

	// также необходимо проверять / считать ошибкой что нибудь типа:
	// пусто ЦИФРА пусто ЦИФРА, то есть пусто может быть только слева


	float result = (
		((value >> 21) & 1) * 1000 +
		d3 * 100 +
		d2 * 10 +
		d1
	);

	if ((value >> 22) & 1) result /= 10;
	if ((value >> 23) & 1) result = -result;

	return result;
}


ISR(SPI_STC_vect) {
	uint8_t value = SPDR;
	switch (SPI_STATE++) {

		case 0: if (value != SPI_PACKET_START) SPI_STATE = SPI_STATE_START; break;
		case 4: SPI_TEMPERATURE = D1_T(value, 0) | D1_TL(value, 1) | D1_BL(value, 2) | D_NEGATIVE(value, 3) | D1_TR(value, 4) | D1_C(value, 5) | D1_BR(value, 6) | D1_B(value, 7); break;
		case 5: SPI_TEMPERATURE |= D0_T(value, 0) | D0_TL(value, 1) | D0_BL(value, 2) | D0_TR(value, 4) | D0_C(value, 5) | D0_BR(value, 6) | D0_B(value, 7); break;
		case 10: if (value >> 6 != SPI_PACKET1_END) SPI_STATE = SPI_STATE_START; break;

		case 11: if (value != SPI_PACKET_START) SPI_STATE = SPI_STATE_START; break;
		case 18: SPI_METERAGE = D2_T(value, 4) | D2_TL(value, 5) | D2_BL(value, 6) | D3(value, 7); break;
		case 19: SPI_METERAGE |= D2_TR(value, 0) | D2_C(value, 1) | D2_BR(value, 2) | D2_B(value, 3) | D1_T(value, 4) | D1_TL(value, 5) | D1_BL(value, 6); break;
		case 20: SPI_METERAGE |= D1_TR(value, 0) | D1_C(value, 1) | D1_BR(value, 2) | D1_B(value, 3); break;
		case 21: if (value >> 6 != SPI_PACKET2_END) SPI_STATE = SPI_STATE_START; break;

		case 22: if (value != SPI_PACKET_START) SPI_STATE = SPI_STATE_START; break;
		case 23: SPI_METERAGE |= D0_T(value, 0) | D0_TL(value, 1) | D0_BL(value, 2) | D0_TR(value, 4) | D0_C(value, 5) | D0_BR(value, 6) | D0_B(value, 7); break;
		case 24: SPI_METERAGE_UNIT = value; break;
		case 28: SPI_METERAGE |= D_FLOAT(value, 7); break;
		case 32: if (value >> 6 != SPI_PACKET3_END) SPI_STATE = SPI_STATE_START; break;

		case 33: if (value != SPI_PACKET_START) SPI_STATE = SPI_STATE_START; break;
		case 36: SPI_TIME = D3(value, 1) | D2_B(value, 2) | D2_BL(value, 3) | D2_C(value, 6) | D2_TL(value, 7); break;
		case 37: SPI_TIME |= D2_BR(value, 2) | D2_TR(value, 3) | D2_T(value, 7); break;
		case 38: SPI_TIME |= D1_B(value, 2) | D1_BL(value, 3) | D1_C(value, 6) | D1_TL(value, 7); break;
		case 39: SPI_TIME |= D1_BR(value, 2) | D1_TR(value, 3) | D1_T(value, 7); break;
		case 40: SPI_TIME |= D0_B(value, 2) | D0_BL(value, 3) | D0_C(value, 6) | D0_TL(value, 7); break;
		case 41: SPI_TIME |= D0_BR(value, 2) | D0_TR(value, 3) | D0_T(value, 7); break;
		case 43: if (value >> 6 != SPI_PACKET4_END) SPI_STATE = SPI_STATE_START; else SPI.detachInterrupt(); break;

	}
}


void loop() {

	if (SPI_STATE == SPI_STATE_DONE) {

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

		SPI_STATE = SPI_STATE_START;
		SPI.attachInterrupt();
	}

}


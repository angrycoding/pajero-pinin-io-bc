#ifndef BC_h
#define BC_h

#include <SPI.h>

// константы используемые для контроля целостности принимаемого пакета,
// позволяют понять что конкретно мы принимаем в данный момент и не потерялось ли чего

// признак начала пакета
#define LC75874_X_START 0x45
// признак окончания первого пакета
#define LC75874_1_END 0b00
// признак окончания второго пакета
#define LC75874_2_END 0b10
// признак окончания третьего пакета
#define LC75874_3_END 0b01
// признак окончания четвертого пакета
#define LC75874_4_END 0b11

// не принимаем данные от мастера
#define BC_STATE_IDLE 100
// ожидаем первого байта от мастера
#define BC_STATE_START 0
// успешно приняли данные от мастера
#define BC_STATE_DONE 44

// константы для перевода миль и галлонов в километры и литры
#define MILE_TO_KM 1.60934
#define MPG_TO_L100KM 282.48163

// 24й байт посылки содержит информацию о индикаторах единиц измерения, по ним
// можно определить какой из трех возможных параметров и в какой единице
// измерения в данный момент отображен на экране

// запас топлива, км
#define METERAGE_FUEL_KM 0b00000010
// запас топлива, миль
#define METERAGE_FUEL_MILES 0b10000000
// скорость, км/ч
#define METERAGE_SPEED_KMH 0b00000011
// скорость, миль/ч
#define METERAGE_SPEED_MPH 0b00010000
// расход, км/л
#define METERAGE_CONSUMPTION_KML 0b01100000
// расход, миль/галлон
#define METERAGE_CONSUMPTION_MPG 0b00001000
// расход, л/100км
#define METERAGE_CONSUMPTION_L100KM 0b01000100

// макросы для управления виртуальным LCD дисплеем,
// каждый из макросов проставляет соответствующий бит в 32х - битное число
// (по сути включает / выключает определенный сегмент определенного знакоместа)
// которое затем используется для получения символа (цифры) отображенного
// на соответствующем знакоместе, формат (0 - младший бит):
// - 0..6 - разряд единиц
// - 7..13 - разряд десятков
// - 14..20 - разряд сотен
// - 21 - разряд тысяч
// - 22 - точка
// - 23 - минус
// по сути нужны лишь для того, чтобы потом, при разборе значения
// знать что откуда доставать, поскольку в пакете от БК до дисплея
// биты сегментов - идут вразнобой

// единицы - верхний сегмент
#define LCD_1T(value, bit) ((value >> bit & 1UL) << 0)
// единицы - средний сегмент
#define LCD_1C(value, bit) ((value >> bit & 1UL) << 1)
// единицы - нижний сегмент
#define LCD_1B(value, bit) ((value >> bit & 1UL) << 2)
// единицы - верхний левый сегмент
#define LCD_1TL(value, bit) ((value >> bit & 1UL) << 3)
// единицы - верхний правый сегмент
#define LCD_1TR(value, bit) ((value >> bit & 1UL) << 4)
// единицы - нижний левый сегмент
#define LCD_1BL(value, bit) ((value >> bit & 1UL) << 5)
// единицы - нижний правый сегмент
#define LCD_1BR(value, bit) ((value >> bit & 1UL) << 6)
// десятки - верхний сегмент
#define LCD_10T(value, bit) ((value >> bit & 1UL) << 7)
// десятки - средний сегмент
#define LCD_10C(value, bit) ((value >> bit & 1UL) << 8)
// десятки - нижний сегмент
#define LCD_10B(value, bit) ((value >> bit & 1UL) << 9)
// десятки - верхний левый сегмент
#define LCD_10TL(value, bit) ((value >> bit & 1UL) << 10)
// десятки - верхний правый сегмент
#define LCD_10TR(value, bit) ((value >> bit & 1UL) << 11)
// десятки - нижний левый сегмент
#define LCD_10BL(value, bit) ((value >> bit & 1UL) << 12)
// десятки - нижний правый сегмент
#define LCD_10BR(value, bit) ((value >> bit & 1UL) << 13)
// сотни - верхний сегмент
#define LCD_100T(value, bit) ((value >> bit & 1UL) << 14)
// сотни - средний сегмент
#define LCD_100C(value, bit) ((value >> bit & 1UL) << 15)
// сотни - нижний сегмент
#define LCD_100B(value, bit) ((value >> bit & 1UL) << 16)
// сотни - верхний левый сегмент
#define LCD_100TL(value, bit) ((value >> bit & 1UL) << 17)
// сотни - верхний правый сегмент
#define LCD_100TR(value, bit) ((value >> bit & 1UL) << 18)
// сотни - нижний левый сегмент
#define LCD_100BL(value, bit) ((value >> bit & 1UL) << 19)
// сотни - нижний правый сегмент
#define LCD_100BR(value, bit) ((value >> bit & 1UL) << 20)
// тысячи - контроллируется одним сегментом
#define LCD_1000(value, bit) ((value >> bit & 1UL) << 21)
// десятичный разделитель между десятками и единицами
#define LCD_DOT(value, bit) ((value >> bit & 1UL) << 22)
// знак минуса
#define LCD_MINUS(value, bit) ((value >> bit & 1UL) << 23)

// константы используемые для конвертации 7 - битной маски сегмента в отображаемое значение

// цифра 0
#define LCD_CHAR_0 125
// цифра 1
#define LCD_CHAR_1 80
// цифра 2
#define LCD_CHAR_2 55
// цифра 3
#define LCD_CHAR_3 87
// цифра 4
#define LCD_CHAR_4 90
// цифра 5
#define LCD_CHAR_5 79
// цифра 6
#define LCD_CHAR_6 111
// цифра 7
#define LCD_CHAR_7 81
// цифра 8
#define LCD_CHAR_8 127
// цифра 9
#define LCD_CHAR_9 95
// пустое знакоместо
#define LCD_CHAR_SPACE 10000
// неизвестный символ
#define LCD_CHAR_UNKNOWN 10001

// ввиду того, что обработчик прерывания не может иметь доступа к приватным
// членам класса, это единственная возможность разграничить права доступа

namespace BC_PRIVATE {

	// сбросить среднюю скорость при следующем переключении режима
	bool DO_RESET_SPEED = false;
	// сбросить расход топлива при следующем переключении режима
	bool DO_RESET_CONSUMPTION = false;

	// текущее время
	float TIME = INFINITY;
	// внешняя температура
	float TEMPERATURE = INFINITY;
	// запас топлива (км)
	float FUEL_KM = INFINITY;
	// средняя скорость (км/ч)
	float SPEED_KMH = INFINITY;
	// расход топлива (л/100км)
	float CONSUMPTION_L100KM = INFINITY;

	volatile uint8_t BC_STATE;
	volatile uint32_t LCD_TIME;
	volatile uint32_t LCD_METERAGE;
	volatile uint32_t LCD_TEMPERATURE;
	volatile uint8_t LCD_METERAGE_UNIT;

	// конвертирует 7 - битную маску сегмента полученную из 32х - битного числа
	// описывающего состояние виртуального LCD - дисплея в отображаемое значение
	uint32_t LCD_getDigit(uint32_t value) {
		switch (value) {
			case LCD_CHAR_0: return 0;
			case LCD_CHAR_1: return 1;
			case LCD_CHAR_2: return 2;
			case LCD_CHAR_3: return 3;
			case LCD_CHAR_4: return 4;
			case LCD_CHAR_5: return 5;
			case LCD_CHAR_6: return 6;
			case LCD_CHAR_7: return 7;
			case LCD_CHAR_8: return 8;
			case LCD_CHAR_9: return 9;
			case 0: return LCD_CHAR_SPACE;
			default: return LCD_CHAR_UNKNOWN;
		}
	}

	// конвертирует 32х - битное число описывающее состояние
	// виртуального LCD - дисплея в отображаемое значение
	float LCD_getValue(uint32_t value) {
		bool isFloat = (value >> 22 & 1);
		bool isNegative = (value >> 23 & 1);
		uint32_t D0 = LCD_getDigit(value & 0x7F);
		if (D0 == LCD_CHAR_UNKNOWN || D0 == LCD_CHAR_SPACE) return INFINITY;
		uint32_t D1 = LCD_getDigit(value >> 7 & 0x7F);
		if (D1 == LCD_CHAR_UNKNOWN || (D1 == LCD_CHAR_SPACE ? isFloat : D0 == LCD_CHAR_SPACE)) return INFINITY;
		uint32_t D2 = LCD_getDigit(value >> 14 & 0x7F);
		if (D2 == LCD_CHAR_UNKNOWN || (D2 != LCD_CHAR_SPACE && D1 == LCD_CHAR_SPACE)) return INFINITY;
		uint32_t D3 = (value >> 21 & 1);
		if (D3 && D2 == LCD_CHAR_SPACE) return INFINITY;
		float result = (D3 * 1000);
		if (D2 != LCD_CHAR_SPACE) result += D2 * 100;
		if (D1 != LCD_CHAR_SPACE) result += D1 * 10;
		if (D0 != LCD_CHAR_SPACE) result += D0;
		if (isFloat) result /= 10;
		if (isNegative) result = -result;
		return result;
	}

	// обработчик SPI - прерывания
	ISR(SPI_STC_vect) {
		uint8_t value = SPDR;
		switch (BC_STATE++) {
			case 0: if (value != LC75874_X_START) BC_STATE = BC_STATE_START; break;
			case 4: LCD_TEMPERATURE = LCD_10T(value, 0) | LCD_10TL(value, 1) | LCD_10BL(value, 2) | LCD_MINUS(value, 3) | LCD_10TR(value, 4) | LCD_10C(value, 5) | LCD_10BR(value, 6) | LCD_10B(value, 7); break;
			case 5: LCD_TEMPERATURE |= LCD_1T(value, 0) | LCD_1TL(value, 1) | LCD_1BL(value, 2) | LCD_1TR(value, 4) | LCD_1C(value, 5) | LCD_1BR(value, 6) | LCD_1B(value, 7); break;
			case 10: if (value >> 6 != LC75874_1_END) BC_STATE = BC_STATE_START; break;
			case 11: if (value != LC75874_X_START) BC_STATE = BC_STATE_START; break;
			case 18: LCD_METERAGE = LCD_100T(value, 4) | LCD_100TL(value, 5) | LCD_100BL(value, 6) | LCD_1000(value, 7); break;
			case 19: LCD_METERAGE |= LCD_100TR(value, 0) | LCD_100C(value, 1) | LCD_100BR(value, 2) | LCD_100B(value, 3) | LCD_10T(value, 4) | LCD_10TL(value, 5) | LCD_10BL(value, 6); break;
			case 20: LCD_METERAGE |= LCD_10TR(value, 0) | LCD_10C(value, 1) | LCD_10BR(value, 2) | LCD_10B(value, 3); break;
			case 21: if (value >> 6 != LC75874_2_END) BC_STATE = BC_STATE_START; break;
			case 22: if (value != LC75874_X_START) BC_STATE = BC_STATE_START; break;
			case 23: LCD_METERAGE |= LCD_1T(value, 0) | LCD_1TL(value, 1) | LCD_1BL(value, 2) | LCD_1TR(value, 4) | LCD_1C(value, 5) | LCD_1BR(value, 6) | LCD_1B(value, 7); break;
			case 24: LCD_METERAGE_UNIT = value; break;
			case 28: LCD_METERAGE |= LCD_DOT(value, 7); break;
			case 32: if (value >> 6 != LC75874_3_END) BC_STATE = BC_STATE_START; break;
			case 33: if (value != LC75874_X_START) BC_STATE = BC_STATE_START; break;
			case 36: LCD_TIME = LCD_1000(value, 1) | LCD_100B(value, 2) | LCD_100BL(value, 3) | LCD_100C(value, 6) | LCD_100TL(value, 7); break;
			case 37: LCD_TIME |= LCD_100BR(value, 2) | LCD_100TR(value, 3) | LCD_100T(value, 7); break;
			case 38: LCD_TIME |= LCD_10B(value, 2) | LCD_10BL(value, 3) | LCD_10C(value, 6) | LCD_10TL(value, 7); break;
			case 39: LCD_TIME |= LCD_10BR(value, 2) | LCD_10TR(value, 3) | LCD_10T(value, 7); break;
			case 40: LCD_TIME |= LCD_1B(value, 2) | LCD_1BL(value, 3) | LCD_1C(value, 6) | LCD_1TL(value, 7); break;
			case 41: LCD_TIME |= LCD_1BR(value, 2) | LCD_1TR(value, 3) | LCD_1T(value, 7); break;
			case 43: if (value >> 6 == LC75874_4_END) SPI.detachInterrupt(); else BC_STATE = BC_STATE_START; break;
		}
	}

}

namespace BC {

	void init() {
		using namespace BC_PRIVATE;
		SPCR |= bit(SPE);
		SPI.setBitOrder(LSBFIRST);
		SPI.setDataMode(SPI_MODE3);
		BC_STATE = BC_STATE_IDLE;
	}

	bool update() {

		using namespace BC_PRIVATE;

		if (BC_STATE == BC_STATE_IDLE) {
			LCD_TIME = 0;
			LCD_METERAGE = 0;
			LCD_TEMPERATURE = 0;
			LCD_METERAGE_UNIT = 0;
			BC_STATE = BC_STATE_START;
			SPI.attachInterrupt();
			return false;
		}


		if (BC_STATE != BC_STATE_DONE) return false;
		BC_STATE = BC_STATE_IDLE;


		bool updated = false;
		float newTime = LCD_getValue(LCD_TIME);
		float newMeterage = LCD_getValue(LCD_METERAGE);
		float newTemperature = LCD_getValue(LCD_TEMPERATURE);

		if (TIME != newTime) {
			TIME = newTime;
			updated = true;
		}

		if (TEMPERATURE != newTemperature) {
			TEMPERATURE = newTemperature;
			updated = true;
		}

		if (LCD_METERAGE_UNIT == METERAGE_FUEL_KM || LCD_METERAGE_UNIT == METERAGE_FUEL_MILES) {

			if (newMeterage != INFINITY && newMeterage != 0) {
				if (LCD_METERAGE_UNIT == METERAGE_FUEL_MILES) {
					newMeterage = ceil(newMeterage * MILE_TO_KM);
				}
			}

			if (FUEL_KM != newMeterage) {
				FUEL_KM = newMeterage;
				updated = true;
			}
		}

		else if (LCD_METERAGE_UNIT == METERAGE_SPEED_KMH || LCD_METERAGE_UNIT == METERAGE_SPEED_MPH) {

			if (newMeterage != INFINITY && newMeterage != 0) {
				if (LCD_METERAGE_UNIT == METERAGE_SPEED_MPH) {
					newMeterage = ceil(newMeterage * MILE_TO_KM);
				}
			}

			if (SPEED_KMH != newMeterage) {
				SPEED_KMH = newMeterage;
				updated = true;
			}
		}

		else if (LCD_METERAGE_UNIT == METERAGE_CONSUMPTION_L100KM || LCD_METERAGE_UNIT == METERAGE_CONSUMPTION_KML || LCD_METERAGE_UNIT == METERAGE_CONSUMPTION_MPG) {


			if (newMeterage != INFINITY && newMeterage != 0) {
				if (LCD_METERAGE_UNIT == METERAGE_CONSUMPTION_MPG) newMeterage = round(10 * (MPG_TO_L100KM / newMeterage)) / 10;
				else if (LCD_METERAGE_UNIT == METERAGE_CONSUMPTION_KML) newMeterage = round(10 * (100 / newMeterage)) / 10;
			}

			if (CONSUMPTION_L100KM != newMeterage) {
				CONSUMPTION_L100KM = newMeterage;
				updated = true;
			}

		}


		return updated;
	}

	void resetSpeed() {
		using namespace BC_PRIVATE;
		DO_RESET_SPEED = true;
	}

	void resetConsumption() {
		using namespace BC_PRIVATE;
		DO_RESET_CONSUMPTION = true;
	}

	float getTime() {
		using namespace BC_PRIVATE;
		return TIME;
	}

	float getTemperature() {
		using namespace BC_PRIVATE;
		return TEMPERATURE;
	}

	float getFuel() {
		using namespace BC_PRIVATE;
		return FUEL_KM;
	}

	float getSpeed() {
		using namespace BC_PRIVATE;
		return SPEED_KMH;
	}

	float getConsumption() {
		using namespace BC_PRIVATE;
		return CONSUMPTION_L100KM;
	}

}

#endif
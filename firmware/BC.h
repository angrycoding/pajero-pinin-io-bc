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
// инициирован сброс показателя
#define BC_STATE_RESET_PRESS 101
// сброс показателя в процессе
#define BC_STATE_RESET_RELEASE 102
// инициировано переключение режима
#define BC_STATE_MODE_PRESS 103
// переключение режима в процессе
#define BC_STATE_MODE_RELEASE 104
// ожидание истечения интервала между обновлениями
#define BC_STATE_UPDATE_DELAY 105
// ожидаем первого байта от мастера
#define BC_STATE_START 0
// успешно приняли данные от мастера
#define BC_STATE_DONE 44

// задержка удержания кнопки переключения режимов
#define MODE_ACTION_DELAY_MS 500
// задержка удержания кнопки сброса показателя
#define RESET_ACTION_DELAY_MS 3000

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

// константы результата выполнения BC::update
#define BC_UPDATE_NOTHING 0
#define BC_UPDATE_FUEL 1
#define BC_UPDATE_SPEED 2
#define BC_UPDATE_CONSUMPTION 3
#define BC_UPDATE_TEMPERATURE 4

// ввиду того, что обработчик прерывания не может иметь доступа к приватным
// членам класса, это единственная возможность разграничить права доступа
namespace BC_private {

	// пин контроллирующий кнопку режима
	uint8_t PIN_MODE;
	// пин контроллирующий кнопку сброса
	uint8_t PIN_RESET;
	// минимальный интервал между обновлениями БК
	uint32_t UPDATE_INTERVAL;

	// внешняя температура
	float temperature = INFINITY;
	// запас топлива (км)
	float fuel = INFINITY;
	// средняя скорость (км/ч)
	float speed = INFINITY;
	// расход топлива (л/100км)
	float consumption = INFINITY;

	// время начала сброса / переключения режима
	uint32_t actionTime;
	// сбросить среднюю скорость при следующем переключении режима
	bool doResetSpeed = false;
	// сбросить расход топлива при следующем переключении режима
	bool doResetConsumption = false;
	// статус принудительного обновления всех показателей
	uint8_t forceUpdateState = BC_UPDATE_NOTHING;

	volatile uint8_t state;
	volatile uint32_t lcdMeterage;
	volatile uint32_t lcdTemperature;
	volatile uint8_t lcdMeterageUnit;

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
		uint32_t D0 = LCD_getDigit(value & 0x7F);
		if (D0 >= LCD_CHAR_UNKNOWN || D0 == LCD_CHAR_SPACE) return INFINITY;
		uint32_t D1 = LCD_getDigit(value >> 7 & 0x7F);
		bool isFloat = (value >> 22 & 1);
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
		// признак отрицательного значения
		if (value >> 23 & 1) result = -result;
		return result;
	}

	// вызывается после того как мы приняли последний байт от мастера
	// конвертирует LCD - значения в float (конвертирует в общепринятые еденицы
	// измерения, при необходимости), сравнивает с предыдущими и в случае если
	// новое значение не соответствует тому, что было принято в прошлый раз,
	// выдает статус который указывает на то, что конкретно поменялось с прошлого раза
	// кроме того, контроллирует флаги принудительного сброса показаний
	// средней скорости и расхода, обновляя стэйт соответствующим образом
	uint8_t doUpdate() {

		float value;
		uint8_t result = BC_UPDATE_NOTHING;

		switch (lcdMeterageUnit) {
			
			// читаем показатель остатка топлива
			case METERAGE_FUEL_KM:
			case METERAGE_FUEL_MILES: {

				// преобразуем LCD - значение в число
				value = LCD_getValue(lcdMeterage);

				// конвертируем в км, в случае если установлены мили и мы прочитали что - то осмысленное
				if (lcdMeterageUnit == METERAGE_FUEL_MILES && value != INFINITY && value != 0) {
					value = ceil(value * MILE_TO_KM);
				}

				// проверяем изменилось - ли значение
				if (fuel != value) {
					fuel = value;
					result = BC_UPDATE_FUEL;
				}

				break;
			}

			// читаем показатель средней скорости
			case METERAGE_SPEED_KMH:
			case METERAGE_SPEED_MPH: {

				// проверяем нужно ли сбросить показатель
				if (doResetSpeed) {
					doResetSpeed = false;
					value = INFINITY;
					state = BC_STATE_RESET_PRESS;
				}

				// в случае, если сбрасывать не нужно, преобразуем LCD - значение в число
				else value = LCD_getValue(lcdMeterage);

				// конвертируем в км/ч, в случае если установлены мили и мы прочитали что - то осмысленное
				if (lcdMeterageUnit == METERAGE_SPEED_MPH && value != INFINITY && value != 0) {
					value = ceil(value * MILE_TO_KM);
				}

				// проверяем изменилось - ли значение
				if (speed != value) {
					speed = value;
					result = BC_UPDATE_SPEED;
				}

				break;
			}

			// читаем показатель расхода
			case METERAGE_CONSUMPTION_L100KM:
			case METERAGE_CONSUMPTION_KML:
			case METERAGE_CONSUMPTION_MPG: {

				// проверяем нужно ли сбросить показатель
				if (doResetConsumption) {
					doResetConsumption = false;
					value = INFINITY;
					state = BC_STATE_RESET_PRESS;
				}

				// в случае, если сбрасывать не нужно, преобразуем LCD - значение в число
				else value = LCD_getValue(lcdMeterage);

				// конвертируем в л/100км, в случае если мы прочитали что - то осмысленное и установлены км/л или галлоны
				if (value != INFINITY && value != 0) {
					if (lcdMeterageUnit == METERAGE_CONSUMPTION_MPG) value = round(10 * (MPG_TO_L100KM / value)) / 10;
					else if (lcdMeterageUnit == METERAGE_CONSUMPTION_KML) value = round(10 * (100 / value)) / 10;
				}

				// проверяем изменилось - ли значение
				if (consumption != value) {
					consumption = value;
					result = BC_UPDATE_CONSUMPTION;
				}

				break;
			}
		}

		// если ни один показатель не изменился, то читаем температуру
		// поскольку она самая низкоприоритетная вещь в данном случае
		if (result == BC_UPDATE_NOTHING) {
			// преобразуем LCD - значение в число
			value = LCD_getValue(lcdTemperature);
			// проверяем изменилось - ли значение
			if (temperature != value) {
				temperature = value;
				result = BC_UPDATE_TEMPERATURE;
			}
		}

		// в случае если не нужно сбрасывать показатель, переключаем режим
		if (state == BC_STATE_DONE) state = BC_STATE_MODE_PRESS;

		return result;
	}

	// обработчик SPI - прерывания
	ISR(SPI_STC_vect) {
		uint8_t value = SPDR;
		switch (state++) {
			case 0: if (value != LC75874_X_START) state = BC_STATE_START; break;
			case 4: lcdTemperature = LCD_10T(value, 0) | LCD_10TL(value, 1) | LCD_10BL(value, 2) | LCD_MINUS(value, 3) | LCD_10TR(value, 4) | LCD_10C(value, 5) | LCD_10BR(value, 6) | LCD_10B(value, 7); break;
			case 5: lcdTemperature |= LCD_1T(value, 0) | LCD_1TL(value, 1) | LCD_1BL(value, 2) | LCD_1TR(value, 4) | LCD_1C(value, 5) | LCD_1BR(value, 6) | LCD_1B(value, 7); break;
			case 10: if (value >> 6 != LC75874_1_END) state = BC_STATE_START; break;
			case 11: if (value != LC75874_X_START) state = BC_STATE_START; break;
			case 18: lcdMeterage = LCD_100T(value, 4) | LCD_100TL(value, 5) | LCD_100BL(value, 6) | LCD_1000(value, 7); break;
			case 19: lcdMeterage |= LCD_100TR(value, 0) | LCD_100C(value, 1) | LCD_100BR(value, 2) | LCD_100B(value, 3) | LCD_10T(value, 4) | LCD_10TL(value, 5) | LCD_10BL(value, 6); break;
			case 20: lcdMeterage |= LCD_10TR(value, 0) | LCD_10C(value, 1) | LCD_10BR(value, 2) | LCD_10B(value, 3); break;
			case 21: if (value >> 6 != LC75874_2_END) state = BC_STATE_START; break;
			case 22: if (value != LC75874_X_START) state = BC_STATE_START; break;
			case 23: lcdMeterage |= LCD_1T(value, 0) | LCD_1TL(value, 1) | LCD_1BL(value, 2) | LCD_1TR(value, 4) | LCD_1C(value, 5) | LCD_1BR(value, 6) | LCD_1B(value, 7); break;
			case 24: lcdMeterageUnit = value; break;
			case 28: lcdMeterage |= LCD_DOT(value, 7); break;
			case 32: if (value >> 6 != LC75874_3_END) state = BC_STATE_START; break;
			case 33: if (value != LC75874_X_START) state = BC_STATE_START; break;
			case 43: if (value >> 6 == LC75874_4_END) SPI.detachInterrupt(); else state = BC_STATE_START; break;
		}
	}

}

namespace BC {

	const uint8_t UPDATE_FUEL = BC_UPDATE_FUEL;
	const uint8_t UPDATE_SPEED = BC_UPDATE_SPEED;
	const uint8_t UPDATE_CONSUMPTION = BC_UPDATE_CONSUMPTION;
	const uint8_t UPDATE_TEMPERATURE = BC_UPDATE_TEMPERATURE;

	// процедура инициализации, вызывается в setup(), принимает номера пинов
	// на которые повешаны кнопки сброса и режима, а также интервал обновления показаний
	void init(uint8_t PIN_MODE, uint8_t PIN_RESET, uint32_t UPDATE_INTERVAL) {
		using namespace BC_private;
		// настройка SPI в режиме slave
		SPCR |= bit(SPE);
		SPI.setBitOrder(LSBFIRST);
		SPI.setDataMode(SPI_MODE3);
		// инициализация переменных и настройка пинов
		state = BC_STATE_IDLE;
		BC_private::UPDATE_INTERVAL = UPDATE_INTERVAL;
		pinMode(BC_private::PIN_MODE = PIN_MODE, OUTPUT);
		pinMode(BC_private::PIN_RESET = PIN_RESET, OUTPUT);
		// просто для того, чтобы было понятно в каком состоянии должны
		// находиться соответствующие пины по - умолчанию
		digitalWrite(PIN_MODE, LOW);
		digitalWrite(PIN_RESET, LOW);
	}

	// процедура обновления, вызывается в loop(), возвращает true, в случае,
	// если доступны новые данные
	uint8_t update() {

		using namespace BC_private;

		switch (state) {

			// начинаем получать данные от мастера
			case BC_STATE_IDLE:
				if (forceUpdateState) return forceUpdateState--;
				lcdMeterage = 0;
				lcdTemperature = 0;
				lcdMeterageUnit = 0;
				state = BC_STATE_START;
				SPI.attachInterrupt();
				break;

			// приняли данные от мастера, обновляем показатели
			case BC_STATE_DONE:
				return doUpdate();

			// нажимаем кнопку сброса
			case BC_STATE_RESET_PRESS:
				noInterrupts();
				PORTD |= (1 << PIN_RESET);
				actionTime = millis();
				interrupts();
				state = BC_STATE_RESET_RELEASE;
				break;

			// отпускаем кнопку сброса
			case BC_STATE_RESET_RELEASE:
				noInterrupts();
				if (millis() - actionTime >= RESET_ACTION_DELAY_MS) {
					PORTD &= ~(1 << PIN_RESET);
					state = BC_STATE_MODE_PRESS;
				}
				interrupts();
				break;

			// нажимаем кнопку переключения режима
			case BC_STATE_MODE_PRESS:
				noInterrupts();
				PORTD |= (1 << PIN_MODE);
				actionTime = millis();
				interrupts();
				state = BC_STATE_MODE_RELEASE;
				break;

			// отпускаем кнопку переключения режима
			case BC_STATE_MODE_RELEASE:
				noInterrupts();
				if (millis() - actionTime >= MODE_ACTION_DELAY_MS) {
					PORTD &= ~(1 << PIN_MODE);
					actionTime = millis();
					state = BC_STATE_UPDATE_DELAY;
				}
				interrupts();
				break;

			// ждем до следующего цикла обновления
			case BC_STATE_UPDATE_DELAY:
				if (millis() - actionTime >= UPDATE_INTERVAL)
					state = BC_STATE_IDLE;
				break;

		}

		return BC_UPDATE_NOTHING;
	}

	// принудительно обновляет все показатели
	// вне зависимости от того изменились они или нет
	void forceUpdate() {
		using namespace BC_private;
		if (forceUpdateState == BC_UPDATE_NOTHING) {
			forceUpdateState = BC_UPDATE_TEMPERATURE;
		}
	}

	// выполняет сброс показателя средней скорости, сброс будет произведен
	// при следующем обновлении
	void resetSpeed() {
		using namespace BC_private;
		doResetSpeed = true;
	}

	// выполняет сброс показателя среднего расхода, сброс будет произведен
	// при следующем обновлении
	void resetConsumption() {
		using namespace BC_private;
		doResetConsumption = true;
	}

	// возвращает внешнюю температуру
	float getTemperature() {
		using namespace BC_private;
		return temperature;
	}

	// возвращает остаток топлива в километрах
	float getFuel() {
		using namespace BC_private;
		return fuel;
	}

	// возвращает среднюю скорость в км/ч
	float getSpeed() {
		using namespace BC_private;
		return speed;
	}

	// возвращает средний расход, в л/100км
	float getConsumption() {
		using namespace BC_private;
		return consumption;
	}

}

#endif
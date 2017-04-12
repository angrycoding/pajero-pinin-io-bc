#ifndef LCD_h
#define LCD_h

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

// константы используемые для конвертации 7 - битной
// маски сегмента в отображаемое значение

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
	if (D0 == LCD_CHAR_UNKNOWN || D0 == LCD_CHAR_SPACE) return LCD_CHAR_UNKNOWN;
	uint32_t D1 = LCD_getDigit(value >> 7 & 0x7F);
	if (D1 == LCD_CHAR_UNKNOWN || (D1 == LCD_CHAR_SPACE ? isFloat : D0 == LCD_CHAR_SPACE)) return LCD_CHAR_UNKNOWN;
	uint32_t D2 = LCD_getDigit(value >> 14 & 0x7F);
	if (D2 == LCD_CHAR_UNKNOWN || (D2 != LCD_CHAR_SPACE && D1 == LCD_CHAR_SPACE)) return LCD_CHAR_UNKNOWN;
	uint32_t D3 = (value >> 21 & 1);
	if (D3 && D2 == LCD_CHAR_SPACE) return LCD_CHAR_UNKNOWN;
	float result = (D3 * 1000);
	if (D2 != LCD_CHAR_SPACE) result += D2 * 100;
	if (D1 != LCD_CHAR_SPACE) result += D1 * 10;
	if (D0 != LCD_CHAR_SPACE) result += D0;
	if (isFloat) result /= 10;
	if (isNegative) result = -result;
	return result;
}

#endif
#include "BC.h"
#include "KL.h"
#include "RPC.h"
#include <avr/wdt.h>

// пин контроллирующий кнопку режима
#define PIN_BC_MODE 2
// пин контроллирующий кнопку сброса
#define PIN_BC_RESET 6

// пин контроллирующий прием данных из K-line
#define PIN_KL_RX 5
// пин контроллирующий отправку данных в K-line
#define PIN_KL_TX 4

// интервал watch-dog таймера
#define WDT_INTERVAL WDTO_4S
// скорость последовательного порта
#define SERIAL_SPEED 115200
// минимальный интервал между обновлениями БК
#define BC_UPDATE_INTERVAL_MS 2000

#define CMD_RESET_SPEED 65
#define CMD_RESET_CONSUMPTION 66




uint8_t pidIndex = 0;

uint8_t PIDS[] = {

	0x00,
	0x01,
	0x02,
	0x03, // тут есть тормоза
	0x04,
	0x05,

	0x06, // угол опережения зажигания
	0x07,
	0x08,
	0x09,
	0x10, // температура охлаждающей жидкости
	0x11, // температура воздуха на впуске
	0x12,
	0x13,
	0x14, // напряжение аккумулятора
	0x15, // датчик барометрического давления (0.49 * x = КПА)
	0x16, // кол-во шагов регулятора холостого хода
	0x17, // положение дроссельной заслонки
	0x18,
	0x19,
	0x20,
	0x21, // обороты двигателя
	0x22,
	0x23,
	0x24,
	0x25,
	0x26,
	0x27,
	0x28,
	0x29, // длительность впрыска (в микросекундах)
	0x2A,
	0x2B,
	0x2C,
	0x2D,
	0x2E,
	0x2F, // текущая скорость

	0x30,
	0x31,
	0x32,
	0x33,
	0x34,
	0x35,
	0x36,
	0x37,
	0x38,
	0x39,
	0x3A,
	0x3B,
	0x3C,
	0x3D,
	0x3E,
	0x3F,

	0x40,
	0x41,
	0x42,
	0x43,
	0x44,
	0x45,
	0x46,
	0x47,
	0x48,
	0x49,
	0x4A,
	0x4B,
	0x4C,
	0x4D,
	0x4E,
	0x4F,

	0x50,
	0x51,
	0x52,
	0x53,
	0x54,
	0x55,
	0x56,
	0x57,
	0x58,
	0x59,
	0x5A,
	0x5B,
	0x5C,
	0x5D,
	0x5E,
	0x5F,

	0x60,
	0x61,
	0x62,
	0x63,
	0x64,
	0x65,
	0x66,
	0x67,
	0x68,
	0x69,
	0x6A,
	0x6B,
	0x6C,
	0x6D,
	0x6E,
	0x6F,

	0x70,
	0x71,
	0x72,
	0x73,
	0x74,
	0x75,
	0x76,
	0x77,
	0x78,
	0x79,
	0x7A,
	0x7B,
	0x7C,
	0x7D,
	0x7E,
	0x7F,





};


void setup() {
	wdt_enable(WDT_INTERVAL);
	Serial.begin(SERIAL_SPEED);
	KL::init(PIN_KL_RX, PIN_KL_TX);
	BC::init(PIN_BC_MODE, PIN_BC_RESET, BC_UPDATE_INTERVAL_MS);
	// otherwise there is a risk to send something several times in millis() - time
	delay(1);
}

void serialEvent() {
	if (RPC::process()) switch (RPC::read()) {
		case CMD_RESET_SPEED: BC::resetSpeed(); break;
		case CMD_RESET_CONSUMPTION: BC::resetConsumption(); break;
	}
}


	uint32_t asyncDelayTime = 0;

	bool asyncDelay(uint32_t delay) {
		if (asyncDelayTime == 0) asyncDelayTime = millis();
		else if (millis() - asyncDelayTime >= delay) {
			asyncDelayTime = 0;
			return false;
		}
		return true;
	}

void loop() {

	wdt_reset();

	if (BC::update()) {
		RPC::write(0xF1, BC::getFuel());
		RPC::write(0xF2, BC::getSpeed());
		RPC::write(0xF3, BC::getConsumption());
		RPC::write(0xF4, BC::getTemperature());
	}

	if (!asyncDelay(500))
		RPC::write(0xFF, KL_private::disconnectCount);

	switch (KL::write(PIDS[pidIndex])) {

		case 1:
			RPC::write(PIDS[pidIndex++], KL::read());
			if (pidIndex == sizeof(PIDS)) pidIndex = 0;
			break;

		case 2:
			RPC::write(PIDS[pidIndex++]);
			if (pidIndex == sizeof(PIDS)) pidIndex = 0;
			break;


	}


}

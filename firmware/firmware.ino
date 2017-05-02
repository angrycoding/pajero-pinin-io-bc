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

	0x06, // угол опережения зажигания
	0x10, // температура охлаждающей жидкости
	0x11, // температура воздуха на впуске
	0x14, // напряжение аккумулятора
	0x15, // датчик барометрического давления (0.49 * x = КПА)
	0x16, // кол-во шагов регулятора холостого хода
	0x17, // положение дроссельной заслонки
	0x21, // обороты двигателя
	0x29, // длительность впрыска (в микросекундах)
	0x2F, // текущая скорость

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
	0x5F

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

void loop() {

	wdt_reset();

	if (true || BC::update()) {
		RPC::write(0xF1, BC::getFuel());
		RPC::write(0xF2, BC::getSpeed());
		RPC::write(0xF3, BC::getConsumption());
		RPC::write(0xF4, BC::getTemperature());
	}

	if (true || KL::write(PIDS[pidIndex])) {
		RPC::write(PIDS[pidIndex++], KL::read());
		if (pidIndex == sizeof(PIDS)) pidIndex = 0;
	}


}

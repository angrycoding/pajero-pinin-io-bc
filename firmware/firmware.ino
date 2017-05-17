#include "BC.h"
#include "KL.h"
#include "RPC.h"
#include <avr/wdt.h>

// пин контроллирующий кнопку режима
#define PIN_BC_MODE 2
// пин контроллирующий кнопку сброса
#define PIN_BC_RESET 6
// минимальный интервал между обновлениями БК
#define BC_UPDATE_INTERVAL_MS 2000

// пин контроллирующий прием данных из K-line
#define PIN_KL_RX 5
// пин контроллирующий отправку данных в K-line
#define PIN_KL_TX 4
// минимальный интервал между опросами K-line
#define KL_REQUEST_INTERVAL 0

// интервал watch-dog таймера
#define WDT_INTERVAL WDTO_4S
// скорость последовательного порта
#define SERIAL_SPEED 230400

// запросы для получения данных фронтэндом
#define CMD_KL_LAST 0x7F
#define CMD_BC_RESET_SPEED 0x80
#define CMD_BC_RESET_CONSUMPTION 0x81
#define CMD_BC_GET_FUEL 0x82
#define CMD_BC_GET_SPEED 0x83
#define CMD_BC_GET_TEMPERATURE 0x84
#define CMD_BC_GET_CONSUMPTION 0x85





uint8_t readingPid = 0x00;
uint8_t KL_PID_VALUES[CMD_KL_LAST + 1] = {0};




void setup() {
	wdt_enable(WDT_INTERVAL);
	Serial.begin(SERIAL_SPEED);
	KL::init(PIN_KL_RX, PIN_KL_TX, KL_REQUEST_INTERVAL);
	BC::init(PIN_BC_MODE, PIN_BC_RESET, BC_UPDATE_INTERVAL_MS);
	// otherwise there is a risk to send something several times in millis() - time
	delay(1);
}

void serialEvent() {
	if (!RPC::process()) return;
	uint8_t key = RPC::readKey();
	if (key > CMD_KL_LAST) switch (key) {
		case CMD_BC_RESET_SPEED: BC::resetSpeed(); break;
		case CMD_BC_RESET_CONSUMPTION: BC::resetConsumption(); break;
		case CMD_BC_GET_FUEL: RPC::writeFloat(CMD_BC_GET_FUEL, BC::getFuel()); break;
		case CMD_BC_GET_SPEED: RPC::writeFloat(CMD_BC_GET_SPEED, BC::getSpeed()); break;
		case CMD_BC_GET_TEMPERATURE: RPC::writeFloat(CMD_BC_GET_TEMPERATURE, BC::getTemperature()); break;
		case CMD_BC_GET_CONSUMPTION: RPC::writeFloat(CMD_BC_GET_CONSUMPTION, BC::getConsumption()); break;
	} else RPC::writeUInt8(key, KL_PID_VALUES[key]);
}

void loop() {
	wdt_reset();
	BC::update();
	switch (KL::write(readingPid)) {

		case KL::WRITE_SUCCESS:
			KL_PID_VALUES[readingPid++] = KL::read();
			if (readingPid > CMD_KL_LAST) readingPid = 0x00;
			break;

		case KL::WRITE_FAIL:
			readingPid = 0x00;
			memset(KL_PID_VALUES, 0, CMD_KL_LAST + 1);
			break;

	}
}

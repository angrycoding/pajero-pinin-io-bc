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
#define WDT_INTERVAL WDTO_1S
// скорость последовательного порта
#define SERIAL_SPEED 115200
// минимальный интервал между обновлениями БК
#define BC_UPDATE_INTERVAL_MS 5000

#define CMD_RESET_SPEED 65
#define CMD_RESET_CONSUMPTION 66

void setup() {
	wdt_enable(WDT_INTERVAL);
	Serial.begin(SERIAL_SPEED);
	KL::init(PIN_KL_RX, PIN_KL_TX);
	BC::init(PIN_BC_MODE, PIN_BC_RESET, BC_UPDATE_INTERVAL_MS);
	// otherwise there is a risk to send something several times in millis() - time
	delay(1);
}

void serialEvent() {
	if (RPC::process()) switch (RPC::getCommand()) {
		case CMD_RESET_SPEED: BC::resetSpeed(); break;
		case CMD_RESET_CONSUMPTION: BC::resetConsumption(); break;
	}
}

void clearAndHome() {
	Serial.write(27); 
	Serial.print("[2J"); // clear screen 
	Serial.write(27); // ESC 
	Serial.print("[H"); // cursor to home 
}

void loop() {

	wdt_reset();

	bool xUpd = BC::update();
	bool yUpd = KL_private::mutLoop();

	if (xUpd || yUpd) {

		clearAndHome();

		Serial.print("TEMPERATURE: ");
		Serial.println(BC::getTemperature());

		Serial.print("FUEL_KM: ");
		Serial.println(BC::getFuel());

		Serial.print("SPEED_KMH: ");
		Serial.println(BC::getSpeed());

		Serial.print("CONSUMPTION_L100KM: ");
		Serial.println(BC::getConsumption());



		Serial.print("RPM: ");
		Serial.println(KL_private::rpm);

		Serial.print("SPEED: ");
		Serial.println(KL_private::speed);

		Serial.print("barometer: ");
		Serial.println(KL_private::barometer);

		Serial.print("COOLANT: ");
		Serial.println(KL_private::coolantTemp);

		Serial.print("ambientTemp: ");
		Serial.println(KL_private::ambientTemp);

		Serial.print("BATTERY_VOLTAGE: ");
		Serial.println(KL_private::batteryVoltage);

		Serial.print("INJ_PULSE_WIDTH: ");
		Serial.println(KL_private::injPulseWidth);

		Serial.print("INTAKE_AIR_TEMP: ");
		Serial.println(KL_private::intakeAirTemp);

		Serial.print("THROTTLE_POSITION: ");
		Serial.println(KL_private::throttlePosition);


		Serial.print("pid39: ");
		Serial.println(KL_private::pid39);

		Serial.print("pid49: ");
		Serial.println(KL_private::pid49);

		Serial.print("pid84: ");
		Serial.println(KL_private::pid84);

		Serial.print("pid87: ");
		Serial.println(KL_private::pid87);

		Serial.print("pid88: ");
		Serial.println(KL_private::pid88);

		Serial.print("pid9A: ");
		Serial.println(KL_private::pid9A);

		Serial.print("pidA8: ");
		Serial.println(KL_private::pidA8);

		Serial.print("pidAA: ");
		Serial.println(KL_private::pidAA);

		Serial.print("pidB3: ");
		Serial.println(KL_private::pidB3);

		Serial.print("pidB4: ");
		Serial.println(KL_private::pidB4);

		Serial.print("pidB8: ");
		Serial.println(KL_private::pidB8);


	}

}

#ifndef RPC_h
#define RPC_h

#define REQUEST_B0 0x28
#define REQUEST_B1 0x7B
#define REQUEST_B3 0x7D
#define REQUEST_B4 0x29

#define RESPONSE_UINT8 'B'
#define RESPONSE_UINT32 'L'
#define RESPONSE_FLOAT 'F'

namespace RPC_private {

	uint8_t command;
	uint8_t position = 0;

	uint8_t iso_checksum(uint8_t *data, uint8_t length) {
		uint8_t result = 0;
		for(uint8_t c = 0; c < length; c++)
			result += data[c];
		return result;
	}

}

namespace RPC {

	bool process() {
		using namespace RPC_private;
		while (Serial.available()) switch (position++) {
			case 0: if (Serial.read() != REQUEST_B0) position = 0; break;
			case 1: if (Serial.read() != REQUEST_B1) position = 0; break;
			case 2: command = Serial.read(); break;
			case 3: if (Serial.read() != REQUEST_B3) position = 0; break;
			case 4: if (position = 0, Serial.read() == REQUEST_B4) return true; break;
		}
		return false;
	}

	uint8_t read() {
		using namespace RPC_private;
		return command;
	}

	void write(uint8_t key, uint8_t value) {
		using namespace RPC_private;
		uint8_t response[4] = {RESPONSE_UINT8, value, key, 0};
		response[3] = iso_checksum(response, 3);
		Serial.write(response, 4);
	}

	void write(uint8_t key, uint32_t value) {
		using namespace RPC_private;
		uint8_t response[7] = {RESPONSE_UINT32, 0, 0, 0, 0, key, 0};
		*reinterpret_cast<uint32_t*>(&response[1]) = value;
		response[6] = iso_checksum(response, 6);
		Serial.write(response, 7);
	}

	void write(uint8_t key, float value) {
		using namespace RPC_private;
		uint8_t response[7] = {RESPONSE_FLOAT, 0, 0, 0, 0, key, 0};
		*reinterpret_cast<float*>(&response[1]) = value;
		response[6] = iso_checksum(response, 6);
		Serial.write(response, 7);
	}

}

#endif
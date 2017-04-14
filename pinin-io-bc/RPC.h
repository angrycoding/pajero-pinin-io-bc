#ifndef RPC_h
#define RPC_h

namespace RPC_PRIVATE {

	uint8_t command;
	uint8_t position = 0;

}

namespace RPC {

	using namespace RPC_PRIVATE;

	bool process() {
		byte size;
		while (size = Serial.available()) switch (position++) {
			case 0: if (Serial.read() != 0x28) position = 0; break;
			case 1: if (Serial.read() != 0x7B) position = 0; break;
			case 2: command = Serial.read(); break;
			case 3: if (Serial.read() != 0x7D) position = 0; break;
			case 4: if (position = 0, Serial.read() == 0x29) return true; break;
		}
		return false;
	}

	uint8_t getCommand() {
		return command;
	}

}

#endif
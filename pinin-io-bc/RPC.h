#ifndef RPC_h
#define RPC_h

#define PACKET_B1 0x28
#define PACKET_B2 0x7B
#define PACKET_B4 0x7D
#define PACKET_B5 0x29

namespace RPC_PRIVATE {
	uint8_t command;
	uint8_t position = 0;
}

namespace RPC {

	bool process() {
		using namespace RPC_PRIVATE;
		byte size;
		while (size = Serial.available()) switch (position++) {
			case 0: if (Serial.read() != PACKET_B1) position = 0; break;
			case 1: if (Serial.read() != PACKET_B2) position = 0; break;
			case 2: command = Serial.read(); break;
			case 3: if (Serial.read() != PACKET_B4) position = 0; break;
			case 4: if (position = 0, Serial.read() == PACKET_B5) return true; break;
		}
		return false;
	}

	uint8_t getCommand() {
		using namespace RPC_PRIVATE;
		return command;
	}

}

#endif
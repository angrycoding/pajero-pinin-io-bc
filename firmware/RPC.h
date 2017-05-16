#ifndef RPC_h
#define RPC_h

#define RPC_KEY 0
#define RPC_TYPE 1
#define RPC_CRC 2

#define RPC_NULL 'N'
#define RPC_UINT8 'B'
#define RPC_UINT32 'L'
#define RPC_FLOAT 'F'

namespace RPC_private {

	uint8_t rxBuffer[7];
	uint8_t txBuffer[7];

	uint8_t state = 0;
	uint8_t buffSize = 0;

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

		while (Serial.available()) switch (state) {

			// читаем ключ
			case RPC_KEY: {
				rxBuffer[buffSize = 0] = Serial.read();
				state = RPC_TYPE;
				break;
			}

			// читаем тип значения
			case RPC_TYPE: {
				switch (rxBuffer[++buffSize] = Serial.read()) {
					case RPC_NULL: state = RPC_CRC; break;
					case RPC_UINT8: state = RPC_UINT8; break;
					default: state = RPC_KEY; break;
				}
				break;
			}

			case RPC_UINT8: {
				rxBuffer[++buffSize] = Serial.read();
				state = RPC_CRC;
				break;
			}

			// проверяем контрольную сумму
			case RPC_CRC: {
				state = RPC_KEY;
				if (Serial.read() == iso_checksum(rxBuffer, ++buffSize))
					return true;
				break;
			}
		}

		return false;
	}

	uint8_t readKey() {
		using namespace RPC_private;
		return rxBuffer[0];
	}

	uint8_t readUInt8() {
		using namespace RPC_private;
		return rxBuffer[2];
	}

	void writeNull(uint8_t key) {
		using namespace RPC_private;
		txBuffer[0] = key;
		txBuffer[1] = RPC_NULL;
		txBuffer[2] = iso_checksum(txBuffer, 2);
		Serial.write(txBuffer, 3);
	}

	void writeUInt8(uint8_t key, uint8_t value) {
		using namespace RPC_private;
		txBuffer[0] = key;
		txBuffer[1] = RPC_UINT8;
		txBuffer[2] = value;
		txBuffer[3] = iso_checksum(txBuffer, 3);
		Serial.write(txBuffer, 4);
	}

	void writeUInt32(uint8_t key, uint32_t value) {
		using namespace RPC_private;
		txBuffer[0] = key;
		txBuffer[1] = RPC_UINT32;
		*reinterpret_cast<uint32_t*>(&txBuffer[2]) = value;
		txBuffer[6] = iso_checksum(txBuffer, 6);
		Serial.write(txBuffer, 7);
	}

	void writeFloat(uint8_t key, float value) {
		using namespace RPC_private;
		txBuffer[0] = key;
		txBuffer[1] = RPC_FLOAT;
		*reinterpret_cast<float*>(&txBuffer[2]) = value;
		txBuffer[6] = iso_checksum(txBuffer, 6);
		Serial.write(txBuffer, 7);
	}

}

#endif
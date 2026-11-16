
#include "structs.h"
#include <vector>
#include <memory>

namespace mscip {

extern int serialFd;
P_Checksum Checksum(uint8_t buf[261], int end);
bool goodPacket(uint8_t packet[261], int end);
bool toStruct(uint8_t type, uint8_t buf[261], int end);
bool parsePacket(uint8_t buf[261], int end);
std::unique_ptr<uint8_t[]> makeMsg(uint8_t msg[], int end);
bool sendMsg(uint8_t msg[], int end);
std::unique_ptr<uint8_t[]> ping();
std::unique_ptr<uint8_t[]> selectSensorData(uint8_t opt1, uint8_t opt2);
std::unique_ptr<uint8_t[]> resetSensorData();
std::unique_ptr<uint8_t[]> resetDevice();
std::unique_ptr<uint8_t[]> sendGetDevCmd(uint8_t cmd);
std::unique_ptr<uint8_t[]> sendCorrGpsTime(uint8_t WNmsb, uint8_t WNlsb, uint8_t newTimeMSB, uint8_t newTimeB2, uint8_t newTimeB1, uint8_t newTimeLSB);

int parseSerial(uint8_t serial[261], int end);
std::unique_ptr<uint8_t[]> getFlags();
}

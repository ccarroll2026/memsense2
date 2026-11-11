
#include "structs.h"
extern int serialFd;
P_Checksum vecChecksum(std::vector<uint8_t> msg);
bool goodPacket(std::vector<uint8_t> packet);
void dataToPacket(std::vector<char> data);
bool vector2Struct(uint8_t type, std::vector<uint8_t> vec);
std::vector<uint8_t> parsePacket(std::vector<char> data);
bool sendMsg(std::vector<uint8_t> msg);
bool ping();
int getFoundGood();
int getFoundBad();
bool selectSensorData(uint8_t opt1, uint8_t opt2);
bool resetSensorData();
bool sendGetDevCmd(uint8_t cmd);
std::vector<std::string> getFlags();


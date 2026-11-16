
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

P_Error get_ack();
P_Device_Msg_Reply get_devComm();
P_Get_Device_Reply getDevMod();
P_Get_Device_Reply getDevSN();
P_Get_Device_Reply getDevFW();
P_Get_Device_Reply getDevCalDate();
P_Scaled_Vec getAccVec();
P_Scaled_Vec getAngVec();
P_Scaled_Vec getMagVec();
P_Scaled_Vec getDTVec();
P_Scaled_Vec getDVVec();
P_Scaled_Single getPressure();
P_Scaled_Single getTemperature();
P_Time_Data getTimeData();
P_Scaled_Vec getAuxAcc();

int getPacketsAccepted();
int getchecksumFailures();
int getBytesDiscarded();
}


#include "structs.h"
#include <vector>
#include <memory>

namespace mscip {

extern int serialFd;
P_Checksum Checksum(uint8_t buf[261], int end);
bool goodPacket(uint8_t packet[261], int end);
Status parsePacket(uint8_t buf[261], int end);
std::unique_ptr<uint8_t[]> makeMsg(uint8_t msg[], int end);
Status sendMsg(uint8_t msg[], int end);
std::pair<std::unique_ptr<uint8_t[]>, Status> ping();
std::pair<std::unique_ptr<uint8_t[]>, Status> selectSensorData(const uint8_t* sensorCodes, int count);
std::pair<std::unique_ptr<uint8_t[]>, Status> resetSensorData();
std::pair<std::unique_ptr<uint8_t[]>, Status> resetDevice();
std::pair<std::unique_ptr<uint8_t[]>, Status> sendGetDevCmd(uint8_t cmd);
std::pair<std::unique_ptr<uint8_t[]>, Status> sendCorrGpsTime(uint8_t WNmsb, uint8_t WNlsb, uint8_t newTimeMSB, uint8_t newTimeB2, uint8_t newTimeB1, uint8_t newTimeLSB);


int getPacketsAccepted();
int getchecksumFailures();
int getBytesDiscarded();

#ifndef P_Parser
#define P_Parser

class Parser {
private:
    uint8_t parseBuffer[261];
    size_t filled = 0;
    uint8_t current[261]; //copy of the last validated packet; PacketView points into it
    PacketView view = {};

public:
    // Returns true when a validated packet is ready; read it with packet().
    PacketView push(uint8_t bytes[], int n);
    PacketView parsing();
    PacketView packet() const;   // valid until the next push()
};
#endif

class FieldIterator {
public:
    // explicit FieldIterator(const PacketView& p);
    // bool next(Field& out);       // false at end of payload or on a malformed field
    // bool malformed() const;

    uint8_t FiType;
    const uint8_t* pl;
    uint8_t plSize;
    size_t next;

    bool malformed = false;

    explicit FieldIterator(const PacketView& p)
    {
        FiType = p.type;
        pl = p.payload;
        plSize = p.payloadLen;
    }

    // Field firstField();
    Field getField(size_t pos);
    
};

}

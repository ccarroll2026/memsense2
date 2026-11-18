
#include "structs.h"
#include <vector>
#include <memory>

namespace mscip {

extern int serialFd;
P_Checksum Checksum(uint8_t buf[261], int end);
bool goodPacket(uint8_t packet[261], int end);
void makeMsg(uint8_t* msg, int end);
Status sendMsg(uint8_t* msg, int end);
Status ping(uint8_t* res, size_t size);
Status selectSensorData(const uint8_t* sensorCodes, int count, uint8_t* res, size_t size);
Status resetSensorData(uint8_t* res, size_t size);
Status resetDevice(uint8_t* res, size_t size);
Status sendGetDevCmd(uint8_t cmd, uint8_t* res, size_t size);
Status sendCorrGpsTime(uint8_t* vals, size_t vSize, uint8_t* res, size_t size);


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
    PacketView parsing(); //internal function to handle packet parsing

public:
    // Returns PacketView when a full packet is found, returns an empty one if not
    PacketView push(uint8_t bytes[], int n);
    PacketView packet() const;   // valid until the next push(); returns view
};
#endif

class FieldIterator {
public:
    
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

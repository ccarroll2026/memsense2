#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <cstdint>
#include <iomanip>

#include "structs.h"
#include "MemsenseCommon.hpp"
#include "example.hpp"

namespace mscip{

int readSerial()
{
    int fd = serialFd;
    if (fd < 0) {
        std::cerr << "Serial port not open" << std::endl;
        return 1;
    }

    // uint8_t hold[261];
    uint8_t buf[256];
    int tries = 3;
    // int end = 0;
    Parser parser;

    while (tries > 0) {
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n > 0) {
            parser.push(buf, n);
        }
        tries--;
    }

    return 0;
}

std::string StatusToString(Status stat)
{
    switch (stat)
    {
    case Status::Ok:
        return "Ok";
    case Status::BufferTooSmall:
        return "BufferTooSmall";
    case Status::InvalidArgument:
        return "InvalidArgument";
    case Status::Malformed:
        return "Malformed";
    case Status::WrongMessage:
        return "WrongMessage";
    case Status::portNotOpened:
        return "portNotOpened";
    case Status::cannotWrite:
        return "cannotWrite";
    default:
        return "unknown status";
    }
}

void testCommands()
{
    std::cout << "Ping ";
    auto res = ping();
    std::cout << StatusToString(res.second) << std::endl;
    readSerial();
    
    std::cout << "selectSensors Acc Ang ";
    uint8_t msg[2] = {0x81, 0x82};
    auto res1 = selectSensorData(msg, 2);
    std::cout << StatusToString(res1.second) << std::endl;
    readSerial();
    

    std::cout << "selectSensors Temp Time ";
    uint8_t msg1[2] = {0x87, 0x88};
    auto res2 = selectSensorData(msg1, 2);
    std::cout << StatusToString(res2.second) << std::endl;
    readSerial();

    std::cout << "selectSensors Delta theta/vel ";
    uint8_t msg2[2] = {0x84, 0x85};
    auto res3 = selectSensorData(msg2, 2);
    std::cout << StatusToString(res3.second) << std::endl;
    readSerial();

    std::cout << "reset sensors data ";
    auto res4 = resetSensorData();
    std::cout << StatusToString(res4.second) << std::endl;
    readSerial();

    std::cout << "reset sensors data ";
    auto res5 = sendGetDevCmd(PE_Get_Dev_Cal);
    std::cout << StatusToString(res5.second) << std::endl;
    readSerial();

    std::cout << "packets accepted = " << getPacketsAccepted() << std::endl;
    std::cout << "checksum fails = " << getchecksumFailures() << std::endl;
    std::cout << "bytes discarded = " << getBytesDiscarded() << std::endl;
}

} // namespace mscip

int main()
{
    using namespace mscip;
    if (openSerial() != 0) return 1;
    
    testCommands();

    close(serialFd);
    serialFd = -1;

    Parser parser;

    uint8_t pingReply[] = {0xA5, 0xA5, 0x01, 0x04, 0x80, 0x02, 0x02, 0x00, 0xD3, 0xCF};
    auto pack = parser.push(pingReply, sizeof(pingReply));
    FieldIterator iterator(pack);

    Field firstField = iterator.getField(0);
    
    return 0;
}

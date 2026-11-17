#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <cstdint>
#include <iomanip>
#include <cstring>

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


void printFlags()
{
    std::cout << "flags: " ;
    std::vector<std::string> sFlags;
    auto flags = getFlags();
    if (flags[0] == 1){ sFlags.push_back("ack"); }
    if (flags[1] == 1){ sFlags.push_back("devComm"); }
    if (flags[2] == 1){ sFlags.push_back("devMod"); }
    if (flags[3] == 1){ sFlags.push_back("devSN"); }
    if (flags[4] == 1){ sFlags.push_back("devFW"); }
    if (flags[5] == 1){ sFlags.push_back("devCalDate"); }
    if (flags[6] == 1){ sFlags.push_back("acc"); }
    if (flags[7] == 1){ sFlags.push_back("ang"); }
    if (flags[8] == 1){ sFlags.push_back("mag"); }
    if (flags[9] == 1){ sFlags.push_back("deltaTheta"); }
    if (flags[10] == 1){ sFlags.push_back("deltaVel"); }
    if (flags[11] == 1){ sFlags.push_back("pressure"); }
    if (flags[12] == 1){ sFlags.push_back("temperature"); }
    if (flags[13] == 1){ sFlags.push_back("gpsCorrTime"); }
    if (flags[14] == 1){ sFlags.push_back("auxAcc"); }

    for (std::string flag : sFlags)
    {
        std::cout << flag << " ";
    }
    std::cout << std::endl;

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
    printFlags();
    
    std::cout << "selectSensors Acc Ang ";
    uint8_t msg[2] = {0x81, 0x82};
    auto res1 = selectSensorData(msg, 2);
    std::cout << StatusToString(res1.second) << std::endl;
    readSerial();
    printFlags();
    

    std::cout << "selectSensors Temp Time ";
    uint8_t msg1[2] = {0x87, 0x88};
    auto res2 = selectSensorData(msg1, 2);
    std::cout << StatusToString(res2.second) << std::endl;
    readSerial();
    printFlags();

    std::cout << "selectSensors Delta theta/vel ";
    uint8_t msg2[2] = {0x84, 0x85};
    auto res3 = selectSensorData(msg2, 2);
    std::cout << StatusToString(res3.second) << std::endl;
    readSerial();
    printFlags();

    std::cout << "reset sensors data ";
    auto res4 = resetSensorData();
    std::cout << StatusToString(res4.second) << std::endl;
    readSerial();
    printFlags();

    std::cout << "reset sensors data ";
    auto res5 = sendGetDevCmd(PE_Get_Dev_Cal);
    std::cout << StatusToString(res5.second) << std::endl;
    readSerial();
    printFlags();

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


    return 0;
}

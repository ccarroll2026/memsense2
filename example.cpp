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

    uint8_t hold[261];
    uint8_t buf[256];
    int tries = 3;
    int end = 0;

    while (tries > 0) {
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n > 0) {

            if (sizeof(hold) - end < n)
            {
                break;
            }
            std::memcpy(hold + end, buf, n);
            end += n;

            int pos = parseSerial(hold, end);
            if (pos != 0)
            {
                std::memcpy(hold, hold + pos, end - pos);
                end = end - pos;
            }
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

void testCommands()
{
    std::cout << "checksum gps" << std::endl;
    uint8_t buf[9] = {0xA5, 0xA5, 0x02, 0x03, 0x0C, 0x01, 0x05, 0x00, 0x00};


    P_Checksum cs = Checksum(buf, 8);
    std::cout << cs << std::endl;


    std::cout << "Ping ";
    ping();
    readSerial();
    printFlags();
    
    std::cout << "selectSensors Acc Ang ";
    selectSensorData(PE_Scaled_Acc, PE_Scaled_Ang);
    readSerial();
    printFlags();

    std::cout << "selectSensors Temp Time ";
    selectSensorData(PE_Scaled_Temp, PE_GPS_Correlated_Time);
    readSerial();
    printFlags();

    std::cout << "selectSensors Delta theta/vel ";
    selectSensorData(PE_Delta_Theta, PE_Delta_Vel);
    readSerial();
    printFlags();

    std::cout << "reset sensors data";
    resetSensorData();
    readSerial();
    printFlags();

    std::cout << "reset sensors data";
    sendGetDevCmd(PE_Get_Dev_Cal);
    readSerial();
    printFlags();
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

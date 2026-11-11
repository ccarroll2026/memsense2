#include <vector>
#include <cstdint>
#include <iostream>
#include <variant>
#include <algorithm>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdexcept>
#include <cstdint>
#include <iomanip>

#include "structs.h"
#include "MemsenseCommon.hpp"

int openSerial()
{
    int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    if (fd < 0) {
        std::cerr << "Error opening serial port" << std::endl;
        std::cout << errno << std::endl;
        return 1;
    }

    struct termios tty;
    tcgetattr(fd, &tty);
    cfmakeraw(&tty); // disable ICRNL/IXON/ICANON etc. so binary bytes pass through untouched
    cfsetspeed(&tty, B115200);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    tty.c_cflag &= ~CRTSCTS; // no hardware flow control
    tcsetattr(fd, TCSANOW, &tty);

    // The device only answers commands with DTR and RTS asserted
    int modem = TIOCM_DTR | TIOCM_RTS;
    ioctl(fd, TIOCMBIS, &modem);
    usleep(200000); // let the lines settle
    tcflush(fd, TCIFLUSH);

    serialFd = fd;
    return 0;
}

int readSerial()
{
    // std::cout << "read serial" << std::endl;
    int fd = serialFd;
    if (fd < 0) {
        std::cerr << "Serial port not open" << std::endl;
        return 1;
    }

    std::vector<uint8_t> holdThis;
    uint8_t buf[256];
    int tries = 100;

    while (tries > 0) {
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n > 0) {
            holdThis.insert(holdThis.end(), buf, buf + n);

            // Extract every complete, checksum-valid packet from the buffer.
            // Packet layout: A5 A5 type len payload[len] cs1 cs2  (len + 6 bytes total)
            size_t pos = 0;
            while (true)
            {
                // find the next sync pair
                size_t sync = pos;
                while (sync + 1 < holdThis.size() && !(holdThis[sync] == 0xA5 && holdThis[sync + 1] == 0xA5))
                    sync++;
                if (sync + 1 >= holdThis.size())
                {
                    // no sync pair; a trailing 0xA5 might be the start of one
                    pos = (!holdThis.empty() && holdThis.back() == 0xA5) ? holdThis.size() - 1 : holdThis.size();
                    break;
                }
                pos = sync;
                if (holdThis.size() - pos < 4) break; // need the length byte

                size_t total = holdThis[pos + 3] + 6;
                if (holdThis.size() - pos < total) break; // wait for the rest

                std::vector<uint8_t> pkt(holdThis.begin() + pos, holdThis.begin() + pos + total);
                P_Checksum cs = vecChecksum(pkt);
                if (cs.f1 == pkt[total - 2] && cs.f2 == pkt[total - 1])
                {
                    parsePacket(std::vector<char>(pkt.begin(), pkt.end()));
                    pos += total;
                }
                else
                {
                    pos++; // false sync or corrupt packet: skip this byte and rescan
                }
            }
            holdThis.erase(holdThis.begin(), holdThis.begin() + pos);
        }
        tries--;
    }

    return 0;
}

void printFlags()
{
    std::cout << "flags: " ;
    for (std::string flag : getFlags())
    {
        std::cout << flag << " ";
    }
    std::cout << "\n" << std::endl;
}

int main()
{
    if (openSerial() != 0) return 1;
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

    close(serialFd);
    serialFd = -1;
    return 0;
}
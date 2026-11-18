#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <cstdint>
#include <iomanip>
#include <cerrno>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <vector>

#include "mscip/structs.h"
#include "mscip/MemsenseCommon.hpp"

namespace mscip{

int serialFd;

std::vector<PacketView> packets;

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

void* receivePacket(PacketView pack)
{
    packets.push_back(pack);
    return nullptr;
}

int readSerial()
{
    int fd = serialFd;
    if (fd < 0) {
        std::cerr << "Serial port not open" << std::endl;
        return 1;
    }

    uint8_t buf[256];
    int tries = 3;
    Parser parser;

    while (tries > 0) {
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n > 0) {
            parser.push(buf, n, &receivePacket);
        }
        tries--;
    }

    return 0;
}

Status writeSerial(uint8_t* msg, size_t size)
{
    if (serialFd < 0)
    {
        return Status::portNotOpened;
    } 

    size_t sent = 0;
    while (sent < static_cast<size_t>(size))
    {
        ssize_t n = write(serialFd, msg + sent, size - sent);
        if (n < 0)
        {
            if (errno == EINTR) continue;
            return Status::cannotWrite;
        }
        sent += n;
    }
    tcdrain(serialFd); //wait until all bytes are transmitted
    
    return Status::Ok;

}

std::string StatusToString(Status stat)
{
    switch (stat)
    {
    case Status::Ok:
        return "Ok";
    case Status::BufferTooSmall:
        return "BufferTooSmall";
    case Status::BufferTooBig:
        return "BufferTooBig";
    case Status::InvalidArgument:
        return "InvalidArgument";
    case Status::Malformed:
        return "Malformed";
    case Status::portNotOpened:
        return "portNotOpened";
    case Status::cannotWrite:
        return "cannotWrite";
    case Status::incompleteMessage:
        return "incompleteMessage";
    default:
        return "unknown status";
    }
}


} // namespace mscip

int main()
{
    using namespace mscip;
    if (openSerial() != 0) return 1;
    
    //example use of library
    //make message, writeSerial, readSerial, parse serial, 

    close(serialFd);
    serialFd = -1;

    
    return 0;
}

#include <iostream>
#include <cerrno>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "MemsenseCommon.hpp"
#include "example.hpp"

namespace mscip {

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

} // namespace mscip

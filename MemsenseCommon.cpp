#include <vector>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <iomanip>
#include <unistd.h>
#include <termios.h>
#include <cerrno>
#include <cstring>
#include <memory>


#include "structs.h"
#include "MemsenseCommon.hpp"

namespace mscip {

int serialFd = -1; //set by the caller after opening the port

namespace {
    uint8_t echo; //last command sent, for verification

    P_Error ack;
    P_Device_Msg_Reply devComm;
    P_Get_Device_Reply devMod;
    P_Get_Device_Reply devSN;
    P_Get_Device_Reply devFW;
    P_Get_Device_Reply devCalDate;
    P_Scaled_Vec accVec;
    P_Scaled_Vec angVec;
    P_Scaled_Vec magVec;
    P_Scaled_Vec deltaThetaVec;
    P_Scaled_Vec deltaVelVec;
    P_Scaled_Single pressure;
    P_Scaled_Single temperature;
    P_Time_Data gpsCorrTime;
    P_Scaled_Vec auxAccVec;

    bool flag_ack = false;
    bool flag_devComm = false;
    bool flag_devMod = false;
    bool flag_devSN = false;
    bool flag_devFW = false;
    bool flag_devCalDate = false;
    bool flag_accVec = false;
    bool flag_angVec = false;
    bool flag_magVec = false;
    bool flag_deltaThetaVec = false;
    bool flag_deltaVelVec = false;
    bool flag_pressure = false;
    bool flag_temperature = false;
    bool flag_gpsCorrTime = false;
    bool flag_auxAccVec = false;

    int packetsAccepted = 0;
    int checksumFailures = 0;
    int bytesDiscarded = 0;

}

// P_Error get_ack() {return ack;}
// P_Device_Msg_Reply get_devComm(){return devComm;}
// P_Get_Device_Reply getDevMod(){return devMod;}
// P_Get_Device_Reply getDevSN(){return devSN;}
// P_Get_Device_Reply getDevFW(){return devFW;}
// P_Get_Device_Reply getDevCalDate(){return devCalDate;}
// P_Scaled_Vec getAccVec(){return accVec;}
// P_Scaled_Vec getAngVec(){return angVec;}
// P_Scaled_Vec getMagVec(){return magVec;}
// P_Scaled_Vec getDTVec(){return deltaThetaVec;}
// P_Scaled_Vec getDVVec(){return deltaVelVec;}
// P_Scaled_Single getPressure(){return pressure;}
// P_Scaled_Single getTemperature(){return temperature;}
// P_Time_Data getTimeData(){return gpsCorrTime;}
// P_Scaled_Vec getAuxAcc(){return auxAccVec;}

P_Checksum Checksum(uint8_t buf[261], int end)
{    
    //Fletcher-16 checksum
    uint8_t f1 = 0;
    uint8_t f2 = 0;
    for (int i = 0; i < end - 2; i++)
    {
        f1 = (f1 + buf[i]);
        f2 = (f2 + f1);
    }

    f1 = f1 % 256;
    f2 = f2 % 256;

    P_Checksum result = {f1, f2};
    return result;
}

//Checks if packet is valid
bool goodPacket(uint8_t packet[261], int end)
{
    bool headerGood = (packet[0] == 0xA5 && packet[1] == 0xA5); //correct start
    
    bool payloadSizeGood = packet[3] == (end - 6); //correct size

    P_Checksum expectedChecksum = Checksum(packet, end); //calc checksum
    auto f1 = packet[end -2];
    auto f2 = packet[end -1];
    bool csGood = (expectedChecksum.f1 == f1 && expectedChecksum.f2 == f2); //correct checksum

    if (!csGood)
    {
        checksumFailures++;
    }

    bool passed = headerGood && payloadSizeGood && csGood;
    return(passed);
}

bool toStruct(uint8_t type, uint8_t buf[261], int end)
{
    uint8_t msgCode = buf[0];
    if (type == 1)
    {
        switch (msgCode)
        {
        case PE_Ack:
            if (end == sizeof(ack))
            {
                ack.msg.msg_code = buf[0];
                ack.msg.msg_size = buf[1];
                ack.echo = buf[2];
                ack.errorCode = buf[3];

                flag_ack = true;
                if (buf[3] != 0)
                {
                    //ACK ERROR
                }
            }
            else
            {
                return false;
            }
            break;
        case PE_Dev_Commands:
            if (end == sizeof(devComm))
            {
                devComm.msg.msg_code = buf[0];
                devComm.msg.msg_size = buf[1];
                devComm.cmd1[0] = buf[3];
                devComm.cmd1[1] = buf[4];
                devComm.cmd1[2] = buf[5];
                devComm.cmd1[3] = buf[6];

                devComm.cmd2[0] = buf[7];
                devComm.cmd2[1] = buf[8];
                devComm.cmd2[2] = buf[9];
                devComm.cmd2[3] = buf[10];

                devComm.cmd3[0] = buf[11];
                devComm.cmd3[1] = buf[12];
                devComm.cmd3[2] = buf[13];
                devComm.cmd3[3] = buf[14];

                devComm.cmd4[0] = buf[15];
                devComm.cmd4[1] = buf[16];
                devComm.cmd4[2] = buf[17];
                devComm.cmd4[3] = buf[18];

                flag_devComm = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Dev_Model:
            if (end == sizeof(devMod))
            {
                devMod.msg.msg_code = buf[0];
                devMod.msg.msg_size = buf[1];
                for (int i = 0; i < 16; i++)
                {
                    devMod.s[i] = buf[i+2];
                }
                flag_devMod = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Dev_SN:
            if (end == sizeof(devSN))
            {
                devSN.msg.msg_code = buf[0];
                devSN.msg.msg_size = buf[1];
                for (int i = 0; i < 16; i++)
                {
                    devSN.s[i] = buf[i+2];
                }
                flag_devSN = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Dev_FW:
            if (end == sizeof(devFW))
            {
                devFW.msg.msg_code = buf[0];
                devFW.msg.msg_size = buf[1];
                for (int i = 0; i < 16; i++)
                {
                    devFW.s[i] = buf[i+2];
                }
                flag_devFW = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Dev_Cal_Date:
            if (end == sizeof(devCalDate))
            {
                devCalDate.msg.msg_code = buf[0];
                devCalDate.msg.msg_size = buf[1];
                for (int i = 0; i < 16; i++)
                {
                    devCalDate.s[i] = buf[i+2];
                }
                flag_devCalDate = true;
            }
            else
            {
                return false;
            }
            break;
        default:
            return false;
        }
    }
    else if (type == 2)
    {
        switch (msgCode)
        {
        case PE_Scaled_Acc_Vec_Reply:
            if (end == sizeof(accVec))
            {
                accVec.msg.msg_code = buf[0];
                accVec.msg.msg_size = buf[1];
                accVec.X[0] = buf[2];
                accVec.X[1] = buf[3];
                accVec.X[2] = buf[4];
                accVec.X[3] = buf[5];

                accVec.Y[0] = buf[6];
                accVec.Y[1] = buf[7];
                accVec.Y[2] = buf[8];
                accVec.Y[3] = buf[9];

                accVec.Z[0] = buf[10];
                accVec.Z[1] = buf[11];
                accVec.Z[2] = buf[12];
                accVec.Z[3] = buf[13];

                flag_accVec = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Scaled_Ang_Rate_Vec_Reply:
            if (end == sizeof(angVec))
            {
                angVec.msg.msg_code = buf[0];
                angVec.msg.msg_size = buf[1];
                angVec.X[0] = buf[2];
                angVec.X[1] = buf[3];
                angVec.X[2] = buf[4];
                angVec.X[3] = buf[5];

                angVec.Y[0] = buf[6];
                angVec.Y[1] = buf[7];
                angVec.Y[2] = buf[8];
                angVec.Y[3] = buf[9];

                angVec.Z[0] = buf[10];
                angVec.Z[1] = buf[11];
                angVec.Z[2] = buf[12];
                angVec.Z[3] = buf[13];

                flag_angVec = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Ack:
            if (end == sizeof(ack))
            {
                ack.msg.msg_code = buf[0];
                ack.msg.msg_size = buf[1];
                ack.echo = buf[2];
                ack.errorCode = buf[3];

                flag_ack = true;
                if (buf[3] != 0)
                {
                    //ACK ERROR
                }
            }
            else
            {
                return false;
            }
            break;
        default:
            return false;
        }
    }
    else if (type == 0xA2)
    {
        switch (msgCode)
        {
        case PE_Scaled_Acc_Vec_Reply:
            if (end == sizeof(accVec))
            {
                accVec.msg.msg_code = buf[0];
                accVec.msg.msg_size = buf[1];
                accVec.X[0] = buf[2];
                accVec.X[1] = buf[3];
                accVec.X[2] = buf[4];
                accVec.X[3] = buf[5];

                accVec.Y[0] = buf[6];
                accVec.Y[1] = buf[7];
                accVec.Y[2] = buf[8];
                accVec.Y[3] = buf[9];

                accVec.Z[0] = buf[10];
                accVec.Z[1] = buf[11];
                accVec.Z[2] = buf[12];
                accVec.Z[3] = buf[13];

                flag_accVec = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Scaled_Ang_Rate_Vec_Reply:
            if (end == sizeof(angVec))
            {
                angVec.msg.msg_code = buf[0];
                angVec.msg.msg_size = buf[1];
                angVec.X[0] = buf[2];
                angVec.X[1] = buf[3];
                angVec.X[2] = buf[4];
                angVec.X[3] = buf[5];

                angVec.Y[0] = buf[6];
                angVec.Y[1] = buf[7];
                angVec.Y[2] = buf[8];
                angVec.Y[3] = buf[9];

                angVec.Z[0] = buf[10];
                angVec.Z[1] = buf[11];
                angVec.Z[2] = buf[12];
                angVec.Z[3] = buf[13];

                flag_angVec = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Scaled_Mag_Vec_Reply:
            if (end == sizeof(magVec))
            {
                magVec.msg.msg_code = buf[0];
                magVec.msg.msg_size = buf[1];
                magVec.X[0] = buf[2];
                magVec.X[1] = buf[3];
                magVec.X[2] = buf[4];
                magVec.X[3] = buf[5];

                magVec.Y[0] = buf[6];
                magVec.Y[1] = buf[7];
                magVec.Y[2] = buf[8];
                magVec.Y[3] = buf[9];

                magVec.Z[0] = buf[10];
                magVec.Z[1] = buf[11];
                magVec.Z[2] = buf[12];
                magVec.Z[3] = buf[13];

                flag_magVec = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Delta_Theta_Vec:
            if (end == sizeof(deltaThetaVec))
            {
                deltaThetaVec.msg.msg_code = buf[0];
                deltaThetaVec.msg.msg_size = buf[1];
                deltaThetaVec.X[0] = buf[2];
                deltaThetaVec.X[1] = buf[3];
                deltaThetaVec.X[2] = buf[4];
                deltaThetaVec.X[3] = buf[5];

                deltaThetaVec.Y[0] = buf[6];
                deltaThetaVec.Y[1] = buf[7];
                deltaThetaVec.Y[2] = buf[8];
                deltaThetaVec.Y[3] = buf[9];

                deltaThetaVec.Z[0] = buf[10];
                deltaThetaVec.Z[1] = buf[11];
                deltaThetaVec.Z[2] = buf[12];
                deltaThetaVec.Z[3] = buf[13];

                flag_deltaThetaVec = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Delta_Vel_Vec:
            if (end == sizeof(deltaVelVec))
            {
                deltaVelVec.msg.msg_code = buf[0];
                deltaVelVec.msg.msg_size = buf[1];
                deltaVelVec.X[0] = buf[2];
                deltaVelVec.X[1] = buf[3];
                deltaVelVec.X[2] = buf[4];
                deltaVelVec.X[3] = buf[5];

                deltaVelVec.Y[0] = buf[6];
                deltaVelVec.Y[1] = buf[7];
                deltaVelVec.Y[2] = buf[8];
                deltaVelVec.Y[3] = buf[9];

                deltaVelVec.Z[0] = buf[10];
                deltaVelVec.Z[1] = buf[11];
                deltaVelVec.Z[2] = buf[12];
                deltaVelVec.Z[3] = buf[13];

                flag_deltaVelVec = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Scaled_Pressure_Reply:
            if (end == sizeof(pressure))
            {
                pressure.msg.msg_code = buf[0];
                pressure.msg.msg_size = buf[1];
                pressure.X[0] = buf[2];
                pressure.X[1] = buf[3];
                pressure.X[2] = buf[4];
                pressure.X[3] = buf[5];

                flag_pressure = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Scaled_Temperature_Reply:
            if (end == sizeof(temperature))
            {
                temperature.msg.msg_code = buf[0];
                temperature.msg.msg_size = buf[1];
                temperature.X[0] = buf[2];
                temperature.X[1] = buf[3];
                temperature.X[2] = buf[4];
                temperature.X[3] = buf[5];
                flag_temperature = true;

            }
            else
            {
                return false;
            }
            break;
        case PE_GPS_Correlated_Time_Reply:
            if (end == sizeof(gpsCorrTime))
            {
                gpsCorrTime.msg.msg_code = buf[0];
                gpsCorrTime.msg.msg_size = buf[1];

                for (int i = 0; i < 8; i++)
                {
                    gpsCorrTime.GPS_SOW[i] = buf[i+2];
                }
                gpsCorrTime.WN[0] = buf[10];
                gpsCorrTime.WN[1] = buf[11];
                gpsCorrTime.timestamp_flag[0] = buf[12];
                gpsCorrTime.timestamp_flag[1] = buf[13];
                flag_gpsCorrTime = true;
            }
            else
            {
                return false;
            }
            break;
        case PE_Scaled_Aux_Acc_Vec:
            if (end == sizeof(auxAccVec))
            {
                auxAccVec.msg.msg_code = buf[0];
                auxAccVec.msg.msg_size = buf[1];
                auxAccVec.X[0] = buf[2];
                auxAccVec.X[1] = buf[3];
                auxAccVec.X[2] = buf[4];
                auxAccVec.X[3] = buf[5];

                auxAccVec.Y[0] = buf[6];
                auxAccVec.Y[1] = buf[7];
                auxAccVec.Y[2] = buf[8];
                auxAccVec.Y[3] = buf[9];

                auxAccVec.Z[0] = buf[10];
                auxAccVec.Z[1] = buf[11];
                auxAccVec.Z[2] = buf[12];
                auxAccVec.Z[3] = buf[13];

                flag_auxAccVec = true;
            }
            else
            {
                return false;
            }
            break;        
        default:
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool parsePacket(uint8_t buf[261], int end)
{
    if (end != buf[3] + 6)
    {
        return {};
    }

    if(!goodPacket(buf, end))
    {
        return {};
    }
    
    uint8_t type = buf[2];

    uint8_t dataLen = buf[5];
    int dl = static_cast<int>(dataLen);

    //stop if message isnt whole
    if (end - 6 <= dl)
    {
        return false;
    }

    //for each distinct* portion of the message (*has its own message code and size)
    bool complete = false;
    int start = 0;

    while (!complete)
    {
        //grab first portion into msg
        dataLen = buf[start + 5];
        dl = static_cast<int>(dataLen);

        uint8_t msg[261];
        std::memcpy(msg, buf + start + 4, dl + 6);
        //load message into struct instance
        if(!toStruct(type, msg, dl + 2))
        {
            return false;
        }

        //get rid of processed data from front
        start += dl + 2;
        
        //if nothig left to process, exit while loop
        if (start >= end - 9){
            complete = true;
            packetsAccepted++;
        }

    }

    return true;
}

int parseSerial(uint8_t serial[261], int end)
{
    size_t pos = 0;
    while (true)
    {
        size_t sync = pos;
        while (sync + 1 < end && !(serial[sync] == 0xA5 && serial[sync +1] == 0xA5))
        {
            sync++;
            bytesDiscarded++;
        }
        if (sync + 1 >= end)
        {
            if (end > 0 && serial[end - 1] == 0xA5)
                {pos = end -1;}
            else
                {pos = end;}
            pos = 0;
            break;
        }
        pos = sync;
        if (end - pos < 4) { pos=0; break; }

        size_t total = serial[pos + 3] + 6;
        if (end - pos < total) { pos=0; break; }

        uint8_t pkt[261];
        std::memcpy(pkt, serial + pos, total);
        P_Checksum cs = Checksum(pkt, total);
        if (cs.f1 == pkt[total - 2] && cs.f2 == pkt[total - 1])
        {
            parsePacket(pkt, total);
            pos += total;
        }
        else
        {
            pos++; // false sync or corrupt packet: skip this byte and rescan
            bytesDiscarded++;
        }
    }
    return pos;
}

void setFlagsFalse()
{
    flag_ack = false;
    flag_devComm = false;
    flag_devMod = false;
    flag_devSN = false;
    flag_devFW = false;
    flag_devCalDate = false;
    flag_accVec = false;
    flag_angVec = false;
    flag_magVec = false;
    flag_deltaThetaVec = false;
    flag_deltaVelVec = false;
    flag_pressure = false;
    flag_temperature = false;
    flag_gpsCorrTime = false;
    flag_auxAccVec = false;
}

std::unique_ptr<uint8_t[]> makeMsg(uint8_t msg[], int end)
{
    auto buffer = std::make_unique<uint8_t[]>(end);
    P_Checksum cs = Checksum(msg, end);
    std::memcpy(buffer.get(), msg, end);
    buffer[end - 2] = cs.f1;
    buffer[end - 1] = cs.f2;
    int payloadSize = end - 6;
    buffer[3] = payloadSize;

    return buffer;
}

bool sendMsg(uint8_t msg[], int end)
{

    if (serialFd < 0)
    {
        return false;
    } 
    setFlagsFalse();

    size_t sent = 0;
    while (sent < end)
    {
        ssize_t n = write(serialFd, msg + sent, end - sent);
        if (n < 0)
        {
            if (errno == EINTR) continue;
            return false;
        }
        sent += n;
    }
    tcdrain(serialFd); //wait until all bytes are transmitted
    echo = msg[4];
    
    return true;
}

//ptr good until ??
std::unique_ptr<uint8_t[]> ping()
{
    uint8_t message[8] = {0xA5, 0xA5, 0x01, 0x02, 0x02, 0x00, 0x00, 0x00};
    auto msg = makeMsg(message, 8);
    bool succeeded = sendMsg(msg.get(), 8);
    if (!succeeded)
    {
        return nullptr;
    }
    return msg;
}

//ptr good until ??
std::unique_ptr<uint8_t[]> selectSensorData(uint8_t opt1, uint8_t opt2)
{
    uint8_t message[11] = {0xA5, 0xA5, 0x02, 0x05, 0x0C, 0x03, 0x01, opt1, opt2, 0x00, 0x00};
    auto msg = makeMsg(message, 11);
    bool succeeded = sendMsg(msg.get(), 11);
    if (!succeeded)
    {
        return nullptr;
    }
    return msg;
}

//ptr good until ??
std::unique_ptr<uint8_t[]> resetSensorData()
{
    uint8_t message[9] = {0xA5, 0xA5, 0x02, 0x03, 0x0C, 0x01, 0x05, 0x00, 0x00};
    auto msg = makeMsg(message, 9);

    bool succeeded = sendMsg(msg.get(), 9);
    if (!succeeded)
    {
        return nullptr;
    }
    return msg;
}

//ptr good until ??
std::unique_ptr<uint8_t[]> resetDevice()
{
    uint8_t message[8] = { 0xA5, 0xA5, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00};
    auto msg = makeMsg(message, 8);
    bool succeeded = sendMsg(msg.get(), 8);
    if (!succeeded)
    {
        return nullptr;
    }
    return msg;
}

//ptr good until ??
std::unique_ptr<uint8_t[]> sendGetDevCmd(uint8_t cmd)
{
    uint8_t message[8] = { 0xA5, 0xA5, 0x01, 0x02, cmd, 0x00, 0x00, 0x00};
    auto msg = makeMsg(message, 8);
    bool succeeded = sendMsg(msg.get(), 8);
    if (!succeeded)
    {
        return nullptr;
    }
    return msg;
}

//ptr good until ??
std::unique_ptr<uint8_t[]> sendCorrGpsTime(uint8_t WNmsb, uint8_t WNlsb, uint8_t newTimeMSB, uint8_t newTimeB2, uint8_t newTimeB1, uint8_t newTimeLSB)
{
    uint8_t message[14] = { 0xA5, 0xA5, 0x01, 0x08, 0x09, 0x06, WNmsb, WNlsb, newTimeMSB, newTimeB2, newTimeB1, newTimeLSB, 0x00, 0x00};
    auto msg = makeMsg(message, 14);
    bool succeeded = sendMsg(msg.get(), 14);
    if (!succeeded)
    {
        return nullptr;
    }
    return msg;
}

std::unique_ptr<uint8_t[]> getFlags()
{
    auto setFlags = std::make_unique<uint8_t[]>(14);
    
    if (flag_ack){ setFlags[0] = 1; } //ack
    if (flag_devComm){ setFlags[1] = 1; } //"devComm"
    if (flag_devMod){ setFlags[2] = 1; } //"devMod"
    if (flag_devSN){ setFlags[3] = 1; } //"devSN"
    if (flag_devFW){ setFlags[4] = 1; } //"devFW"
    if (flag_devCalDate){ setFlags[5] = 1; } //"devCalDate"
    if (flag_accVec){ setFlags[6] = 1; } //"acc"
    if (flag_angVec){ setFlags[7] = 1; } //"ang"
    if (flag_magVec){ setFlags[8] = 1; } //"mag"
    if (flag_deltaThetaVec){ setFlags[9] = 1; } //"deltaTheta"
    if (flag_deltaVelVec){ setFlags[10] = 1; } //"deltaVel"
    if (flag_pressure){ setFlags[11] = 1; } //"pressure"
    if (flag_temperature){ setFlags[12] = 1; } //"temperature"
    if (flag_gpsCorrTime){ setFlags[13] = 1; } //"gpsCorrTime"
    if (flag_auxAccVec){ setFlags[14] = 1; } //"auxAcc"

    return setFlags;
}

} //namespace mscip
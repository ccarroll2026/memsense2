#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <unistd.h>
#include <termios.h>
#include <cerrno>

#include "structs.h"
#include "MemsenseCommon.hpp"

uint8_t echo; //last command sent
int serialFd = -1; //set by the caller after opening the port

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

P_Error get_ack() {return ack;}
P_Device_Msg_Reply get_devComm(){return devComm;}
P_Get_Device_Reply getDevMod(){return devMod;}
P_Get_Device_Reply getDevSN(){return devSN;}
P_Get_Device_Reply getDevFW(){return devFW;}
P_Get_Device_Reply getDevCalDate(){return devCalDate;}
P_Scaled_Vec getAccVec(){return accVec;}
P_Scaled_Vec getAngVec(){return angVec;}
P_Scaled_Vec getMagVec(){return magVec;}
P_Scaled_Vec getDTVec(){return deltaThetaVec;}
P_Scaled_Vec getDVVec(){return deltaVelVec;}
P_Scaled_Single getPressure(){return pressure;}
P_Scaled_Single getTemperature(){return temperature;}
P_Time_Data getTimeData(){return gpsCorrTime;}
P_Scaled_Vec getAuxAcc(){return auxAccVec;}

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



//Calculate the checksum from a vector
P_Checksum vecChecksum(std::vector<uint8_t> msg)
{
    //Don't include the empty checksum values
    std::vector<uint8_t> body(msg.begin(), msg.end() - 2);

    //Fletcher-16 checksum
    uint8_t f1 = 0;
    uint8_t f2 = 0;
    for (uint8_t b : body)
    {
        f1 = (f1 + b);
        f2 = (f2 + f1);
    }
    f1 = f1 % 256;
    f2 = f2 % 256;

    P_Checksum result = {f1, f2};
    return result;
}

//Checks if packet is valid
bool goodPacket(std::vector<uint8_t> packet)
{
    bool headerGood = (packet[0] == 0xA5 && packet[1] == 0xA5); //correct start
    
    bool payloadSizeGood = packet[3] == (packet.size() - 6); //correct size

    P_Checksum expectedChecksum = vecChecksum(packet); //calc checksum
    auto f1 = packet[packet.size()-2];
    auto f2 = packet[packet.size()-1];
    bool csGood = (expectedChecksum.f1 == f1 && expectedChecksum.f2 == f2); //correct checksum

    bool passed = headerGood && payloadSizeGood && csGood;
    return(passed);
}

// void dataToPacket(std::vector<char> data)
// {
//     parsePacket(data);
// }

bool vector2Struct(uint8_t type, std::vector<uint8_t> vec)
{
    uint8_t msgCode = vec[0];
    if (type == 1)
    {
        switch (msgCode)
        {
        case PE_Ack:
            if (vec.size() == sizeof(ack))
            {
                ack.msg.msg_code = vec[0];
                ack.msg.msg_size = vec[1];
                ack.echo = vec[2];
                ack.errorCode = vec[3];

                flag_ack = true;
                if (vec[3] != 0)
                {
                    std::cout << "ACK ERROR" << ack << std::endl;
                }
            }
            else
            {
                std::cout << "Invalid message size (PE_Ack)" << std::endl;
                return false;
            }
            break;
        case PE_Dev_Commands:
            if (vec.size() == sizeof(devComm))
            {
                devComm.msg.msg_code = vec[0];
                devComm.msg.msg_size = vec[1];
                devComm.cmd1[0] = vec[3];
                devComm.cmd1[1] = vec[4];
                devComm.cmd1[2] = vec[5];
                devComm.cmd1[3] = vec[6];

                devComm.cmd2[0] = vec[7];
                devComm.cmd2[1] = vec[8];
                devComm.cmd2[2] = vec[9];
                devComm.cmd2[3] = vec[10];

                devComm.cmd3[0] = vec[11];
                devComm.cmd3[1] = vec[12];
                devComm.cmd3[2] = vec[13];
                devComm.cmd3[3] = vec[14];

                devComm.cmd4[0] = vec[15];
                devComm.cmd4[1] = vec[16];
                devComm.cmd4[2] = vec[17];
                devComm.cmd4[3] = vec[18];

                flag_devComm = true;
            }
            else
            {
                std::cout << "Invalid message size (devComm)" << std::endl;
                return false;
            }
            break;
        case PE_Dev_Model:
            if (vec.size() == sizeof(devMod))
            {
                devMod.msg.msg_code = vec[0];
                devMod.msg.msg_size = vec[1];
                for (int i = 0; i < 16; i++)
                {
                    devMod.s[i] = vec[i+2];
                }
                flag_devMod = true;
            }
            else
            {
                std::cout << "Invalid message size (devMod)" << std::endl;
                return false;
            }
            break;
        case PE_Dev_SN:
            if (vec.size() == sizeof(devSN))
            {
                devSN.msg.msg_code = vec[0];
                devSN.msg.msg_size = vec[1];
                for (int i = 0; i < 16; i++)
                {
                    devSN.s[i] = vec[i+2];
                }
                flag_devSN = true;
            }
            else
            {
                std::cout << "Invalid message size (devSN)" << std::endl;
                return false;
            }
            break;
        case PE_Dev_FW:
            if (vec.size() == sizeof(devFW))
            {
                devFW.msg.msg_code = vec[0];
                devFW.msg.msg_size = vec[1];
                for (int i = 0; i < 16; i++)
                {
                    devFW.s[i] = vec[i+2];
                }
                flag_devFW = true;
            }
            else
            {
                std::cout << "Invalid message size (devFW)" << std::endl;
                return false;
            }
            break;
        case PE_Dev_Cal_Date:
            if (vec.size() == sizeof(devCalDate))
            {
                devCalDate.msg.msg_code = vec[0];
                devCalDate.msg.msg_size = vec[1];
                for (int i = 0; i < 16; i++)
                {
                    devCalDate.s[i] = vec[i+2];
                }
                flag_devCalDate = true;
            }
            else
            {
                std::cout << "Invalid message size (devCalDate)" << std::endl;
                return false;
            }
            break;
        default:
            std::cout << "Invalid message size (default type 1)" << std::endl;
            return false;
        }
    }
    else if (type == 2)
    {
        switch (msgCode)
        {
        case PE_Scaled_Acc_Vec_Reply:
            if (vec.size() == sizeof(accVec))
            {
                accVec.msg.msg_code = vec[0];
                accVec.msg.msg_size = vec[1];
                accVec.X[0] = vec[2];
                accVec.X[1] = vec[3];
                accVec.X[2] = vec[4];
                accVec.X[3] = vec[5];

                accVec.Y[0] = vec[6];
                accVec.Y[1] = vec[7];
                accVec.Y[2] = vec[8];
                accVec.Y[3] = vec[9];

                accVec.Z[0] = vec[10];
                accVec.Z[1] = vec[11];
                accVec.Z[2] = vec[12];
                accVec.Z[3] = vec[13];

                flag_accVec = true;
            }
            else
            {
                std::cout << "Invalid message size (accVec)" << std::endl;
                return false;
            }
            break;
        case PE_Scaled_Ang_Rate_Vec_Reply:
            if (vec.size() == sizeof(angVec))
            {
                angVec.msg.msg_code = vec[0];
                angVec.msg.msg_size = vec[1];
                angVec.X[0] = vec[2];
                angVec.X[1] = vec[3];
                angVec.X[2] = vec[4];
                angVec.X[3] = vec[5];

                angVec.Y[0] = vec[6];
                angVec.Y[1] = vec[7];
                angVec.Y[2] = vec[8];
                angVec.Y[3] = vec[9];

                angVec.Z[0] = vec[10];
                angVec.Z[1] = vec[11];
                angVec.Z[2] = vec[12];
                angVec.Z[3] = vec[13];

                flag_angVec = true;
            }
            else
            {
                std::cout << "Invalid message size (angVec)" << std::endl;
                return false;
            }
            break;
        case PE_Ack:
            if (vec.size() == sizeof(ack))
            {
                ack.msg.msg_code = vec[0];
                ack.msg.msg_size = vec[1];
                ack.echo = vec[2];
                ack.errorCode = vec[3];

                flag_ack = true;
                if (vec[3] != 0)
                {
                    std::cout << "ACK ERROR" << ack << std::endl;
                }
            }
            else
            {
                std::cout << "Invalid message size (PE_Ack)" << std::endl;
                return false;
            }
            break;
        default:
            std::cout << "invalid msg code" << std::endl;
            return false;
        }
    }
    else if (type == 0xA2)
    {
        switch (msgCode)
        {
        case PE_Scaled_Acc_Vec_Reply:
            if (vec.size() == sizeof(accVec))
            {
                accVec.msg.msg_code = vec[0];
                accVec.msg.msg_size = vec[1];
                accVec.X[0] = vec[2];
                accVec.X[1] = vec[3];
                accVec.X[2] = vec[4];
                accVec.X[3] = vec[5];

                accVec.Y[0] = vec[6];
                accVec.Y[1] = vec[7];
                accVec.Y[2] = vec[8];
                accVec.Y[3] = vec[9];

                accVec.Z[0] = vec[10];
                accVec.Z[1] = vec[11];
                accVec.Z[2] = vec[12];
                accVec.Z[3] = vec[13];

                flag_accVec = true;
            }
            else
            {
                std::cout << "Invalid message size (accVec type A2)" << std::endl;
                return false;
            }
            break;
        case PE_Scaled_Ang_Rate_Vec_Reply:
            if (vec.size() == sizeof(angVec))
            {
                angVec.msg.msg_code = vec[0];
                angVec.msg.msg_size = vec[1];
                angVec.X[0] = vec[2];
                angVec.X[1] = vec[3];
                angVec.X[2] = vec[4];
                angVec.X[3] = vec[5];

                angVec.Y[0] = vec[6];
                angVec.Y[1] = vec[7];
                angVec.Y[2] = vec[8];
                angVec.Y[3] = vec[9];

                angVec.Z[0] = vec[10];
                angVec.Z[1] = vec[11];
                angVec.Z[2] = vec[12];
                angVec.Z[3] = vec[13];

                flag_angVec = true;
            }
            else
            {
                std::cout << "Invalid message size (angVec type A2)" << std::endl;
                return false;
            }
            break;
        case PE_Scaled_Mag_Vec_Reply:
            if (vec.size() == sizeof(magVec))
            {
                magVec.msg.msg_code = vec[0];
                magVec.msg.msg_size = vec[1];
                magVec.X[0] = vec[2];
                magVec.X[1] = vec[3];
                magVec.X[2] = vec[4];
                magVec.X[3] = vec[5];

                magVec.Y[0] = vec[6];
                magVec.Y[1] = vec[7];
                magVec.Y[2] = vec[8];
                magVec.Y[3] = vec[9];

                magVec.Z[0] = vec[10];
                magVec.Z[1] = vec[11];
                magVec.Z[2] = vec[12];
                magVec.Z[3] = vec[13];

                flag_magVec = true;
            }
            else
            {
                std::cout << "Invalid message size (magVec)" << std::endl;
                return false;
            }
            break;
        case PE_Delta_Theta_Vec:
            if (vec.size() == sizeof(deltaThetaVec))
            {
                deltaThetaVec.msg.msg_code = vec[0];
                deltaThetaVec.msg.msg_size = vec[1];
                deltaThetaVec.X[0] = vec[2];
                deltaThetaVec.X[1] = vec[3];
                deltaThetaVec.X[2] = vec[4];
                deltaThetaVec.X[3] = vec[5];

                deltaThetaVec.Y[0] = vec[6];
                deltaThetaVec.Y[1] = vec[7];
                deltaThetaVec.Y[2] = vec[8];
                deltaThetaVec.Y[3] = vec[9];

                deltaThetaVec.Z[0] = vec[10];
                deltaThetaVec.Z[1] = vec[11];
                deltaThetaVec.Z[2] = vec[12];
                deltaThetaVec.Z[3] = vec[13];

                flag_deltaThetaVec = true;
            }
            else
            {
                std::cout << "Invalid message size (deltaTheraVec)" << std::endl;
                return false;
            }
            break;
        case PE_Delta_Vel_Vec:
            if (vec.size() == sizeof(deltaVelVec))
            {
                deltaVelVec.msg.msg_code = vec[0];
                deltaVelVec.msg.msg_size = vec[1];
                deltaVelVec.X[0] = vec[2];
                deltaVelVec.X[1] = vec[3];
                deltaVelVec.X[2] = vec[4];
                deltaVelVec.X[3] = vec[5];

                deltaVelVec.Y[0] = vec[6];
                deltaVelVec.Y[1] = vec[7];
                deltaVelVec.Y[2] = vec[8];
                deltaVelVec.Y[3] = vec[9];

                deltaVelVec.Z[0] = vec[10];
                deltaVelVec.Z[1] = vec[11];
                deltaVelVec.Z[2] = vec[12];
                deltaVelVec.Z[3] = vec[13];

                flag_deltaVelVec = true;
            }
            else
            {
                std::cout << "Invalid message size (deltaVelVec)" << std::endl;
                return false;
            }
            break;
        case PE_Scaled_Pressure_Reply:
            if (vec.size() == sizeof(pressure))
            {
                pressure.msg.msg_code = vec[0];
                pressure.msg.msg_size = vec[1];
                pressure.X[0] = vec[2];
                pressure.X[1] = vec[3];
                pressure.X[2] = vec[4];
                pressure.X[3] = vec[5];

                flag_pressure = true;
            }
            else
            {
                std::cout << "Invalid message size (pressure)" << std::endl;
                return false;
            }
            break;
        case PE_Scaled_Temperature_Reply:
            if (vec.size() == sizeof(temperature))
            {
                temperature.msg.msg_code = vec[0];
                temperature.msg.msg_size = vec[1];
                temperature.X[0] = vec[2];
                temperature.X[1] = vec[3];
                temperature.X[2] = vec[4];
                temperature.X[3] = vec[5];
                flag_temperature = true;

            }
            else
            {
                std::cout << "Invalid message size (temperature)" << std::endl;
                return false;
            }
            break;
        case PE_GPS_Correlated_Time_Reply:
            if (vec.size() == sizeof(gpsCorrTime))
            {
                gpsCorrTime.msg.msg_code = vec[0];
                gpsCorrTime.msg.msg_size = vec[1];

                for (int i = 0; i < 8; i++)
                {
                    gpsCorrTime.GPS_SOW[i] = vec[i+2];
                }
                gpsCorrTime.WN[0] = vec[10];
                gpsCorrTime.WN[1] = vec[11];
                gpsCorrTime.timestamp_flag[0] = vec[12];
                gpsCorrTime.timestamp_flag[1] = vec[13];
                flag_gpsCorrTime = true;
            }
            else
            {
                std::cout << "Invalid message size (gpsCorrTime)" << std::endl;
                return false;
            }
            break;
        case PE_Scaled_Aux_Acc_Vec:
            if (vec.size() == sizeof(auxAccVec))
            {
                auxAccVec.msg.msg_code = vec[0];
                auxAccVec.msg.msg_size = vec[1];
                auxAccVec.X[0] = vec[2];
                auxAccVec.X[1] = vec[3];
                auxAccVec.X[2] = vec[4];
                auxAccVec.X[3] = vec[5];

                auxAccVec.Y[0] = vec[6];
                auxAccVec.Y[1] = vec[7];
                auxAccVec.Y[2] = vec[8];
                auxAccVec.Y[3] = vec[9];

                auxAccVec.Z[0] = vec[10];
                auxAccVec.Z[1] = vec[11];
                auxAccVec.Z[2] = vec[12];
                auxAccVec.Z[3] = vec[13];

                flag_auxAccVec = true;
            }
            else
            {
                std::cout << "Invalid message size (auxAccVec)" << std::endl;
                return false;
            }
            break;        
        default:
            std::cout << "Invalid message size (default type A2)" << std::endl;
            return false;
        }
    }
    else
    {
        std::cout << "Invalid type" << std::endl;
        return false;
    }
    return true;
}

std::vector<uint8_t> parsePacket(std::vector<char> data)
{
    std::vector<uint8_t> packet(data.begin(), data.end());

    if (packet.size() != packet[3] + 6)
    {
        return {};
    }

    if(!goodPacket(packet))
    {
        std::cout << "packet not good" << std::endl;
    }
    
    //seperate out header
    std::vector<uint8_t> header(packet.begin(), packet.begin() + 4);
    uint8_t type = header[2];

    //seperate out checksum
    std::vector<uint8_t> cs(packet.end() -2, packet.end());

    //seperate out content
    std::vector<uint8_t> content(packet.begin() + 4, packet.end() -2);
    uint8_t dataLen = content[1];
    int dl = static_cast<int>(dataLen);

    //stop if message isnt whole
    if (content.size() <= dl)
    {
        std::cout << "invalid message length" << std::endl;
        return {};
    }

    //for each distinct* portion of the message (*has its own message code and size)
    bool complete = false;
    while (!complete)
    {
        //grab first portion into msg
        dataLen = content[1];
        dl = static_cast<int>(dataLen);
        std::vector<uint8_t> msg(content.begin(), content.begin() + dl + 2); //this contains just the one struct

        //load message into struct instance
        if(!vector2Struct(type, msg))
        {
            std::cout << "failed" << std::endl;
            return {};
        }

        //get rid of processed data from front
        content.assign(content.begin() + dl + 2, content.end());

        //if nothig left to process, exit while loop
        if (content.size() == 0 ){
            complete = true;
        }

    }

    return packet;
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

bool sendMsg(std::vector<uint8_t> msg)
{
    P_Checksum cs = vecChecksum(msg);
    msg[msg.size() - 2] = cs.f1;
    msg[msg.size() - 1] = cs.f2;
    int payloadSize = msg.size() - 6;
    msg[3] = payloadSize;

    if (serialFd < 0)
    {
        std::cout << "Error: could not open FD" << std::endl;
        return false;
    } 
    setFlagsFalse();

    size_t sent = 0;
    while (sent < msg.size())
    {
        ssize_t n = write(serialFd, msg.data() + sent, msg.size() - sent);
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

bool ping()
{
    std::vector<uint8_t> message = {0xA5, 0xA5, 0x01, 0x02, 0x02, 0x00, 0x00, 0x00};
    bool succeeded = sendMsg(message);
    return succeeded;
}

bool selectSensorData(uint8_t opt1, uint8_t opt2)
{
    std::vector<uint8_t> message = { 0xA5, 0xA5, 0x02, 0x05, 0x0C, 0x03, 0x01, opt1, opt2, 0x00, 0x00};
    bool succeeded = sendMsg(message);
    return succeeded;
}

bool resetSensorData()
{
    std::vector<uint8_t> message = { 0xA5, 0xA5, 0x02, 0x03, 0x0C, 0x01, 0x05, 0x00, 0x00};
    bool succeeded = sendMsg(message);
    return succeeded;
}

bool resetDevice()
{
    std::vector<uint8_t> message = { 0xA5, 0xA5, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00};
    bool succeeded = sendMsg(message);
    return succeeded;
}

bool sendGetDevCmd(uint8_t cmd)
{
    std::vector<uint8_t> message = { 0xA5, 0xA5, 0x01, 0x02, cmd, 0x00, 0x00, 0x00};
    bool succeeded = sendMsg(message);
    return succeeded;
}

bool sendCorrGpsTime(uint8_t WNmsb, uint8_t WNlsb, uint8_t newTimeMSB, uint8_t newTimeB2, uint8_t newTimeB1, uint8_t newTimeLSB)
{
    std::vector<uint8_t> message = { 0xA5, 0xA5, 0x01, 0x08, WNmsb, WNlsb, newTimeMSB, newTimeB2, newTimeB1, newTimeLSB, 0x00, 0x00};
    bool succeeded = sendMsg(message);
    return succeeded;
}

std::vector<std::string> getFlags()
{
    std::vector<std::string> setFlags{};
    
    if (flag_ack){ setFlags.push_back("ack"); }
    if (flag_devComm){ setFlags.push_back("devComm"); }
    if (flag_devMod){ setFlags.push_back("devMod"); }
    if (flag_devSN){ setFlags.push_back("devSN"); }
    if (flag_devFW){ setFlags.push_back("devFW"); }
    if (flag_devCalDate){ setFlags.push_back("devCalDate"); }
    if (flag_accVec){ setFlags.push_back("acc"); }
    if (flag_angVec){ setFlags.push_back("ang"); }
    if (flag_magVec){ setFlags.push_back("mag"); }
    if (flag_deltaThetaVec){ setFlags.push_back("deltaTheta"); }
    if (flag_deltaVelVec){ setFlags.push_back("deltaVel"); }
    if (flag_pressure){ setFlags.push_back("pressure"); }
    if (flag_temperature){ setFlags.push_back("temperature"); }
    if (flag_gpsCorrTime){ setFlags.push_back("gpsCorrTime"); }
    if (flag_auxAccVec){ setFlags.push_back("auxAcc"); }

    return setFlags;
}

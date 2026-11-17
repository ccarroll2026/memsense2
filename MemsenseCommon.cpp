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

int getPacketsAccepted(){return packetsAccepted;}
int getchecksumFailures(){return checksumFailures;}
int getBytesDiscarded(){return bytesDiscarded;}

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

Status parsePacket(uint8_t buf[261], int end)
{
    if (end != buf[3] + 6)
    {
        return Status::Malformed;
    }

    if(!goodPacket(buf, end))
    {
        return Status::Malformed;
    }
    
    uint8_t type = buf[2];

    uint8_t dataLen = buf[5];
    int dl = static_cast<int>(dataLen);

    //stop if message isnt whole
    if (end - 6 <= dl)
    {
        return Status::incompleteMessage;
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
        // if(!toStruct(type, msg, dl + 2))
        // {
        //     return Status::InvalidArgument;
        // }

        //field iterator ??

        //get rid of processed data from front
        start += dl + 2;
        
        //if nothig left to process, exit while loop
        if (start >= end - 9){
            complete = true;
            packetsAccepted++;
        }

    }

    return Status::Ok;
}

// Returns true when a validated packet is ready; read it with packet().
PacketView Parser::push(uint8_t bytes[], int n)
{
    if (filled + n > sizeof(parseBuffer))
    {
        size_t drop = filled + n - sizeof(parseBuffer); //discard oldest bytes to make room
        if (drop > filled) drop = filled;
        std::memmove(parseBuffer, parseBuffer + drop, filled - drop);
        filled -= drop;
        bytesDiscarded += drop;
    }
    if (static_cast<size_t>(n) > sizeof(parseBuffer)) return {};
    std::memcpy(parseBuffer + filled, bytes, n);
    filled += n;
    //drain every complete packet in the buffer; return the last one found
    PacketView last = {};
    for (PacketView p = parsing(); p.payload != nullptr; p = parsing())
    {
        last = p;
    }

    return last;
}

PacketView Parser::parsing()
{
    size_t pos = 0;
    
    while (true)
    {
        size_t sync = pos;
        while (sync + 1 < filled && !(parseBuffer[sync] == 0xA5 && parseBuffer[sync + 1] == 0xA5))
        {
            sync++;
            bytesDiscarded++;
        }
        if (sync >= filled){
            break;}
        pos = sync;
        if (filled - pos < 4) {
            break;}

        size_t total = parseBuffer[pos + 3] + 6;
        if (filled - pos < total)
        {
            //Candidate isn't complete yet. Its length byte may be false, so before waiting for more
            //bytes check whether a complete, valid packet starts later inside the buffer.
            size_t q = pos + 1;
            bool found = false;
            for (; q + 4 <= filled; q++)
            {
                if (parseBuffer[q] != 0xA5 || parseBuffer[q + 1] != 0xA5) continue;
                size_t t2 = parseBuffer[q + 3] + 6;
                if (filled - q < t2) continue;
                P_Checksum c2 = Checksum(parseBuffer + q, t2);
                if (c2.f1 == parseBuffer[q + t2 - 2] && c2.f2 == parseBuffer[q + t2 - 1])
                {
                    found = true;
                    break;
                }
            }
            if (!found) break;
            bytesDiscarded += q - pos; //skip the false candidate
            pos = q;
            continue;
        }

        uint8_t pkt[261];
        std::memcpy(pkt, parseBuffer + pos, total);
        P_Checksum cs = Checksum(pkt, total);
    
        if (cs.f1 == pkt[total - 2] && cs.f2 == pkt[total - 1])
        {
            PacketView view = {};
            view.type = pkt[2];
            view.payloadLen = total - 6;
            //copy out: the view must stay valid after the buffer is shifted below
            std::memcpy(current, pkt, total);
            view.payload = current + 4;

            pos += total; //consume the packet and any garbage before it

            std::memmove(parseBuffer, parseBuffer + pos, filled - pos);
            filled -= pos;

            parsePacket(pkt, total);
            return view;
        }
        pos++; // false sync or corrupt packet: skip this byte and rescan
        bytesDiscarded++;
    }
    return {};
}

PacketView Parser::packet() const   // valid until the next push()
{
    return view;
}

std::unique_ptr<uint8_t[]> makeMsg(uint8_t msg[], int end)
{
    auto buffer = std::make_unique<uint8_t[]>(end);
    
    std::memcpy(buffer.get(), msg, end);

    int payloadSize = end - 6;
    buffer[3] = payloadSize;

    P_Checksum cs = Checksum(buffer.get(), end);

    buffer[end - 2] = cs.f1;
    buffer[end - 1] = cs.f2;

    return buffer;
}

Status sendMsg(uint8_t msg[], int end)
{

    if (serialFd < 0)
    {
        return Status::portNotOpened;
    } 

    size_t sent = 0;
    while (sent < end)
    {
        ssize_t n = write(serialFd, msg + sent, end - sent);
        if (n < 0)
        {
            if (errno == EINTR) continue;
            return Status::cannotWrite;
        }
        sent += n;
    }
    tcdrain(serialFd); //wait until all bytes are transmitted
    echo = msg[4];
    
    return Status::Ok;
}

//ptr good until ??
std::pair<std::unique_ptr<uint8_t[]>, Status> ping()
{
    uint8_t message[8] = {0xA5, 0xA5, 0x01, 0x02, 0x02, 0x00, 0x00, 0x00};
    auto msg = makeMsg(message, 8);
    Status sts = sendMsg(msg.get(), 8);
    bool succeeded = sts == Status::Ok;
    if (!succeeded)
    {
        return {nullptr, sts};
    }
    return {std::move(msg), sts};
}

//ptr good until ??
std::pair<std::unique_ptr<uint8_t[]>, Status>selectSensorData(const uint8_t* sensorCodes, int count)
{
    uint8_t startMessage[7] = {0xA5, 0xA5, 0x02, 0x05, 0x0C, 0x03, 0x01}; //, opt1, opt2, 0x00, 0x00};

    if (count <= 0 || count > 9)
    {
        return {nullptr, Status::InvalidArgument};
    }

    int max = count + 9; //7 header bytes + count codes + 2 checksum bytes
    auto message = std::make_unique<uint8_t[]>(max);

    for (int i = 0; i < 7; i++)
    {
        message[i] = startMessage[i];
    }
    message[5] = count + 1; //descriptor length: function byte + sensor codes
    
    for (int i = 0; i < count; i++)
    {
        switch (sensorCodes[i])
        {
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
            break;
        default:
            return {nullptr, Status::InvalidArgument};

        }
        message[7 + i] = sensorCodes[i];
    }
    auto msg = makeMsg(message.get(), max);
    Status sts = sendMsg(msg.get(), max);
    bool succeeded = sts == Status::Ok;
    if (!succeeded)
    {
        return {nullptr, sts};
    }
    return {std::move(msg), sts};
}

//ptr good until ??
std::pair<std::unique_ptr<uint8_t[]>, Status> resetSensorData()
{
    uint8_t message[9] = {0xA5, 0xA5, 0x02, 0x03, 0x0C, 0x01, 0x05, 0x00, 0x00};
    auto msg = makeMsg(message, 9);
    Status sts = sendMsg(msg.get(), 9);
    bool succeeded = sts == Status::Ok;
    if (!succeeded)
    {
        return {nullptr, sts};
    }
    return {std::move(msg), sts};
}

//ptr good until ??
std::pair<std::unique_ptr<uint8_t[]>, Status> resetDevice()
{
    uint8_t message[8] = { 0xA5, 0xA5, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00};
    auto msg = makeMsg(message, 8);
    Status sts = sendMsg(msg.get(), 8);
    bool succeeded = sts == Status::Ok;
    if (!succeeded)
    {
        return {nullptr, sts};
    }
    return {std::move(msg), sts};
}

//ptr good until ??
std::pair<std::unique_ptr<uint8_t[]>, Status> sendGetDevCmd(uint8_t cmd)
{
    uint8_t message[8] = { 0xA5, 0xA5, 0x01, 0x02, cmd, 0x00, 0x00, 0x00};
    auto msg = makeMsg(message, 8);
    Status sts = sendMsg(msg.get(), 8);
    bool succeeded = sts == Status::Ok;
    if (!succeeded)
    {
        return {nullptr, sts};
    }
    return {std::move(msg), sts};
}

//ptr good until ??
std::pair<std::unique_ptr<uint8_t[]>, Status> sendCorrGpsTime(uint8_t WNmsb, uint8_t WNlsb, uint8_t newTimeMSB, uint8_t newTimeB2, uint8_t newTimeB1, uint8_t newTimeLSB)
{
    uint8_t message[14] = { 0xA5, 0xA5, 0x01, 0x08, 0x09, 0x06, WNmsb, WNlsb, newTimeMSB, newTimeB2, newTimeB1, newTimeLSB, 0x00, 0x00};
    auto msg = makeMsg(message, 14);
    Status sts = sendMsg(msg.get(), 14);
    bool succeeded = sts == Status::Ok;
    if (!succeeded)
    {
        return {nullptr, sts};
    }
    return {std::move(msg), sts};
}


// explicit FieldIterator(const PacketView& p);
// bool next(Field& out);       // false at end of payload or on a malformed field
// bool malformed() const;

// Field FieldIterator::firstField()
// {
//     Field firstField = getField(0);
    
//     return firstField
// }

Field FieldIterator::getField(size_t pos)
{
    Field f;

    f.size = pl[pos + 1];

    if (pos + 2 + f.size > plSize)
    {
        malformed = true;
        next = 0;
        return {};
    }
    f.type = FiType;
    f.code = pl[pos];

    f.data = &pl[pos + 2];

    if (pos + f.size < plSize - 2)
    {
        //next = position in p where next field starts
        
        next = pos + f.size + 2;
    }
    else{
        next = 0;
    }

    return f;
}

} //namespace mscip
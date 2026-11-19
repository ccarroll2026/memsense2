#include <cstdint>
#include <iterator>
#include <cstring>

#include "mscip/structs.h"
#include "mscip/MemsenseCommon.hpp"

namespace mscip {

//Returns checksum value, buf[] should include the last two bytes where checksum will go
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

//Functions to view stats about process
int Parser::getPacketsAccepted(){return packetsAccepted;}
int Parser::getchecksumFailures(){return checksumFailures;}
int Parser::getBytesDiscarded(){return bytesDiscarded;}

// Returns number of valid packets found in bytes[]; n is size of bytes, callbackFunc recieves each valid packet
int Parser::push(uint8_t bytes[], int n, void (*callbackFunc(PacketView)))
{
    int numPackets = 0;
    //pushed bytes dont fit in parseBuffer, make room by dropping oldest bytes
    if (filled + n > sizeof(parseBuffer))
    {
        size_t drop = filled + n - sizeof(parseBuffer); //discard oldest bytes to make room
        if (drop > filled) drop = filled;
        std::memmove(parseBuffer, parseBuffer + drop, filled - drop);
        filled -= drop;
        bytesDiscarded += static_cast<int>(drop);
    }
    
    // if (static_cast<size_t>(n) > sizeof(parseBuffer) - filled) return {}; //ditch this line
    
    //pass bytes[] into parseBuffer for continuity
    std::memcpy(parseBuffer + filled, bytes, n);
    filled += n;

    //parse all packets in buffer and call callbackFunc on each
    for (PacketView p = parsing(); p.payload != nullptr; p = parsing())
    {
        numPackets++;
        callbackFunc(p);
    }

    return numPackets;
}

//Internal function 
PacketView Parser::parsing()
{
    size_t pos = 0;
    
    while (true)
    {
        //find first instance of sync bytes (0xa5, 0xa5)
        size_t sync = pos;
        while (sync + 1 < filled && !(parseBuffer[sync] == 0xA5 && parseBuffer[sync + 1] == 0xA5))
        {
            sync++;
            bytesDiscarded++;
        }
        if (sync >= filled) //sync not found
        {
            break;
        }
        pos = sync;
        if (filled - pos < 4) //sync found too close to end to contain full message
        {
            break;
        }

        size_t total = parseBuffer[pos + 3] + 6; //expected size of packet
        if (total > 255)
        {
            break;
        }

        //incomplete packet or bad header
        if (filled - pos < total)
        {
            //Its length byte may be false, so before waiting for more
            //bytes check whether a complete, valid packet starts later inside the buffer.
            size_t q = pos + 1;
            bool found = false;
            for (; q + 4 <= filled; q++)
            {
                if (parseBuffer[q] != 0xA5 || parseBuffer[q + 1] != 0xA5) continue;
                size_t t2 = parseBuffer[q + 3] + 6;
                if (filled - q < t2) continue;
                P_Checksum c2 = Checksum(parseBuffer + q, static_cast<int>(t2));
                if (c2.f1 == parseBuffer[q + t2 - 2] && c2.f2 == parseBuffer[q + t2 - 1])
                {
                    found = true;
                    break;
                }
            }
            if (!found) break;
            bytesDiscarded += static_cast<int>(q - pos); //skip the false candidate
            pos = q;
            continue;
        }

        //Check the checksum
        uint8_t pkt[261];
        std::memcpy(pkt, parseBuffer + pos, total);
        P_Checksum cs = Checksum(pkt, static_cast<int>(total));
    
        if (cs.f1 == pkt[total - 2] && cs.f2 == pkt[total - 1]) //good checksum, fill PacketView view
        {
            PacketView view = {};
            view.type = pkt[2];
            view.payloadLen = static_cast<uint8_t>(total - 6);
            //copy out: the view must stay valid after the buffer is shifted below
            std::memcpy(current, pkt, total);
            view.payload = current + 4;

            pos += total; //consume the packet and any garbage before it

            std::memmove(parseBuffer, parseBuffer + pos, filled - pos);
            filled -= pos;

            return view;
        }
        pos++; // false sync or corrupt packet: skip this byte and rescan
        bytesDiscarded++;
    }
    return {}; //no good checksum, return empty
}

//get most recent parsed packet
PacketView Parser::packet() const   // valid until the next push()
{
    return view;
}

//internal function, fills payload and checksum of provided message
void makeMsg(uint8_t* msg, int end)
{

    int payloadSize = end - 6;
    msg[3] = static_cast<uint8_t>(payloadSize);

    P_Checksum cs = Checksum(msg, end);

    msg[end - 2] = cs.f1;
    msg[end - 1] = cs.f2;
}

//fills res with ping message, size is size of res
Status ping(uint8_t* res, size_t size)
{
    uint8_t message[8] = {0xA5, 0xA5, 0x01, 0x02, 0x02, 0x00, 0x00, 0x00};
    makeMsg(message, 8);
    Status sts = Status::Ok;

    //check if res size is correct
    if (sizeof(message) > size)
    {
        sts = Status::BufferTooSmall;
        return sts;
    }
    else if (sizeof(message) < size)
    {
        sts = Status::BufferTooBig;
        return sts;
    }
    //copy message into res
    std::memcpy(res, message, size);

    return sts;
}

//fill res with select sensor message which calls provided sensorCodes
//count is size of sensorCodes buffer, size is size of res
Status selectSensorData(const uint8_t* sensorCodes, int count, uint8_t* res, size_t size)
{
    uint8_t startMessage[7] = {0xA5, 0xA5, 0x02, 0x05, 0x0C, 0x03, 0x01};

    if (count <= 0 || count > 9)
    {
        return Status::InvalidArgument;
    }

    int max = count + 9; //7 header bytes + count codes + 2 checksum bytes
    uint8_t message[18]; //count is 7 header + 9 possible codes + 2 checksum bytes

    for (int i = 0; i < 7; i++)
    {
        message[i] = startMessage[i];
    }
    message[5] = static_cast<uint8_t>(count + 1); //descriptor length: function byte + sensor codes
    
    //check if sensorCodes are valid
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
            return Status::InvalidArgument;

        }
        message[7 + i] = sensorCodes[i]; //use good sensor code
    }

    makeMsg(message, max);
    Status sts = Status::Ok;

    //check res size
    if (static_cast<size_t>(max) > size)
    {
        sts = Status::BufferTooSmall;
        return sts;
    }
    if (static_cast<size_t>(max) < size)
    {
        sts = Status::BufferTooBig;
        return sts;
    }
    //copy message into res
    std::memcpy(res, message, size);

    return sts;
}

//fills res with reset sensors message, size is size of res
Status resetSensorData(uint8_t* res, size_t size)
{
    uint8_t message[9] = {0xA5, 0xA5, 0x02, 0x03, 0x0C, 0x01, 0x05, 0x00, 0x00};
    makeMsg(message, 9);
    Status sts = Status::Ok;

    //check res size
    if (sizeof(message) > size)
    {
        sts = Status::BufferTooSmall;
        return sts;
    }
    else if (sizeof(message) < size)
    {
        sts = Status::BufferTooBig;
        return sts;
    }
    std::memcpy(res, message, size);

    return sts;
}

//fills res with reset device command
Status resetDevice(uint8_t* res, size_t size)
{
    uint8_t message[8] = { 0xA5, 0xA5, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00};
    makeMsg(message, 8);
    Status sts = Status::Ok;
    
    //check res size
    if (sizeof(message) > size)
    {
        sts = Status::BufferTooSmall;
        return sts;
    }
    else if (sizeof(message) < size)
    {
        sts = Status::BufferTooBig;
        return sts;
    }
    std::memcpy(res, message, size);

    return sts;
}

//fills res with get device message, using code cmd
Status sendGetDevCmd(uint8_t cmd, uint8_t* res, size_t size)
{
    uint8_t message[8] = { 0xA5, 0xA5, 0x01, 0x02, cmd, 0x00, 0x00, 0x00};
    makeMsg(message, 8);
    Status sts = Status::Ok;
    
    //check size of res
    if (sizeof(message) > size)
    {
        sts = Status::BufferTooSmall;
        return sts;
    }
    else if (sizeof(message) < size)
    {
        sts = Status::BufferTooBig;
        return sts;
    }
    std::memcpy(res, message, size);

    return sts;
}

// uint8_t WNmsb, uint8_t WNlsb, uint8_t newTimeMSB, uint8_t newTimeB2, uint8_t newTimeB1, uint8_t newTimeLSB,
//fills res with correlated GPS time message
//vals should be GPS WN (2 bytes) and new time value (4 bytes), both MSB first
Status sendCorrGpsTime(uint8_t* vals, size_t vSize, uint8_t* res, size_t size)
{
    if (vSize != 6) //must have 6 bytes in vals
    {
        return Status::InvalidArgument;
    }
    uint8_t message[14] = { 0xA5, 0xA5, 0x01, 0x08, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    std::memcpy(message + 7, vals, vSize);
    makeMsg(message, 14);
    Status sts = Status::Ok;
    
    //check size of res
    if (sizeof(message) > size)
    {
        sts = Status::BufferTooSmall;
        return sts;
    }
    else if (sizeof(message) < size)
    {
        sts = Status::BufferTooBig;
        return sts;
    }
    std::memcpy(res, message, size);

    return sts;
}

//get the field at pos from the full payload (field is message code + size and message data)
//sets variable next (belongs to FieldIterator) to pos of next field start
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

    if (pos + f.size + 2 < plSize)
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
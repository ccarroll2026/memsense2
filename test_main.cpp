#include "MemsenseCommon.hpp"
#include <gtest/gtest.h>
#include "example.hpp"
#include <cstring>
#include "structs.h"


static size_t g_allocs = 0;
void* operator new(size_t n) { ++g_allocs; return std::malloc(n); }
void  operator delete(void* p) noexcept { std::free(p); }
void  operator delete(void* p, size_t) noexcept { std::free(p); }

namespace mscip {

//Test suite: ChecksumTest
class ChecksumTest : public ::testing::Test {
protected:
    static void SetUpTestSuite()
    {
        if (openSerial() != 0) return;
    }

    static void TearDownTestSuite()
    {
        close(serialFd);
        serialFd = -1;
    }
};



TEST_F(ChecksumTest, csPing)
{
    uint8_t buf[8] = {0xA5, 0xA5, 0x01, 0x02, 0x02, 0x00, 0x4F, 0x25};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csPingReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x01, 0x04, 0x80, 0x02, 0x02, 0x00, 0xD3, 0xCF};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGetDevice)
{
    uint8_t buf[8] = {0xA5, 0xA5, 0x01, 0x02, 0x03, 0x00, 0x50, 0x27};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGetDeviceReply)
{
    uint8_t buf[20] = {0xA5, 0xA5, 0x01, 0x0F, 0x80, 0x03, 0x03, 0x00, 0x83, 0x08, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x7D, 0x73};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csDeviceReset)
{
    uint8_t buf[8] = {0xA5, 0xA5, 0x01, 0x02, 0x04, 0x00, 0x51, 0x29};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csDeviceResetReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x01, 0x04, 0x80, 0x02, 0x04, 0x00, 0xD5, 0xD3};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGetDeviceModel)
{
    uint8_t buf[8] = {0xA5, 0xA5, 0x01, 0x02, 0x05, 0x00, 0x52, 0x2B};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGetDeviceModelReply)
{
    uint8_t buf[28] = {0xA5, 0xA5, 0x01, 0x16, 0x80, 0x02, 0x05, 0x00, 0x85, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x4D, 0x53, 0x5F, 0x49, 0x4D, 0x55, 0x33, 0x30, 0x32, 0x30, 0xEC, 0x54};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGetDeviceSN)
{
    uint8_t buf[8] = {0xA5, 0xA5, 0x01, 0x02, 0x06, 0x00, 0x53, 0x2D};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGetDeviceSNReply)
{
    uint8_t buf[28] = {0xA5, 0xA5, 0x01, 0x16, 0x80, 0x02, 0x06, 0x00, 0x86, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x32, 0x30, 0x32, 0x36, 0x38, 0xE1, 0x23};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGetDeviceFW)
{
    uint8_t buf[8] = {0xA5, 0xA5, 0x01, 0x02, 0x07, 0x00, 0x54, 0x2F};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGetDeviceFWReply)
{
    uint8_t buf[28] = {0xA5, 0xA5, 0x01, 0x16, 0x80, 0x02, 0x07, 0x00, 0x87, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x52, 0x5F, 0x31, 0x5F, 0x32, 0x5F, 0x33, 0xA6, 0x25};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGetDeviceCal)
{
    uint8_t buf[8] = {0xA5, 0xA5, 0x01, 0x02, 0x08, 0x00, 0x55, 0x31};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGetDeviceCalReply)
{
    uint8_t buf[28] = {0xA5, 0xA5, 0x01, 0x16, 0x80, 0x02, 0x08, 0x00, 0x88, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x35, 0x2D, 0x30, 0x38, 0x2D, 0x32, 0x30, 0x31, 0x35, 0x32, 0x10};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGPSTime)
{
    uint8_t buf[14] = {0xA5, 0xA5, 0x01, 0x08, 0x09, 0x06, 0x07, 0x2F, 0x00, 0x00, 0x02, 0xFF, 0x99, 0xAF};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csGPSTimeReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x01, 0x04, 0x80, 0x02, 0x09, 0x00, 0xDA, 0xDD};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csBaudConfig)
{
    uint8_t buf[13] = {0xA5, 0xA5, 0x02, 0x07, 0x01, 0x05, 0x01, 0x00, 0x01, 0xC2, 0x00, 0x1D, 0x84};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); 
}
TEST_F(ChecksumTest, csBaudConfigReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x80, 0x02, 0x01, 0x00, 0xD3, 0xD3};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csFilterConfig)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x03, 0x02, 0x01, 0x02, 0x58, 0xE1};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csFilterConfigReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x80, 0x02, 0x03, 0x00, 0xD5, 0xD7};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csImuSampleRateConfig)
{
    uint8_t buf[11] = {0xA5, 0xA5, 0x02, 0x05, 0x04, 0x03, 0x01, 0x00, 0x12, 0x6B, 0x56};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csImuSampleRateConfigReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x80, 0x02, 0x04, 0x00, 0xD6, 0xD9};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csSelectSensors)
{
    uint8_t buf[12] = {0xA5, 0xA5, 0x02, 0x06, 0x05, 0x03, 0x01, 0x00, 0x81, 0x82, 0x5E, 0x2E};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csSelectSensorsReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x80, 0x02, 0x05, 0x00, 0xD7, 0xDB};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csGetSampleRate)
{
    uint8_t buf[8] = {0xA5, 0xA5, 0x02, 0x02, 0x06, 0x00, 0x54, 0x31};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csGetSampleRateReply)
{
    uint8_t buf[14] = {0xA5, 0xA5, 0x02, 0x08, 0x80, 0x02, 0x06, 0x00, 0x86, 0x02, 0x03, 0x20, 0x87, 0xA5};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csAccRanConfig)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x07, 0x02, 0x01, 0x02, 0x5C, 0xF1};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csAccRanConfigReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x80, 0x02, 0x07, 0x00, 0xD9, 0xDF};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(static_cast<int>(thing.f1), buf[sizeof(buf)-2]);
    EXPECT_EQ(static_cast<int>(thing.f2), buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csGyroRanConfig)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x08, 0x02, 0x01, 0x02, 0x5D, 0xF5};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(static_cast<int>(thing.f1), buf[sizeof(buf)-2]);
    EXPECT_EQ(static_cast<int>(thing.f2), buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csGyroRanConfigReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x80, 0x02, 0x08, 0x00, 0xDA, 0xE1};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(static_cast<int>(thing.f1), buf[sizeof(buf)-2]);
    EXPECT_EQ(static_cast<int>(thing.f2), buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csAllConfig)
{
    uint8_t buf[9] = {0xA5, 0xA5, 0x02, 0x03, 0x09, 0x01, 0x03, 0x5C, 0x97};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(static_cast<int>(thing.f1), buf[sizeof(buf)-2]);
    EXPECT_EQ(static_cast<int>(thing.f2), buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csAllConfigReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x80, 0x02, 0x09, 0x00, 0xDB, 0xE3};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csDataOn)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x0A, 0x02, 0x01, 0x01, 0x5E, 0xFC};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csDataOnReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x80, 0x02, 0x0A, 0x00, 0xDC, 0xE5};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csExtrigOn)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x0B, 0x02, 0x01, 0x01, 0x5F, 0x00};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csExtrigOnReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x80, 0x02, 0x0B, 0x00, 0xDD, 0xE7};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csSelectSensorsRevB)
{
    uint8_t buf[11] = {0xA5, 0xA5, 0x02, 0x05, 0x0C, 0x03, 0x01, 0x81, 0x82, 0x64, 0xF0};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csSelectSensorsRevBReply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x80, 0x02, 0x0C, 0x00, 0xDE, 0xE9};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csAccRanConfig2)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x0D, 0x02, 0x01, 0x05, 0x65, 0x0C};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csAccRanConfig2Reply)
{
    uint8_t buf[10] = {0xA5, 0xA5, 0x02, 0x04, 0x80, 0x02, 0x0D, 0x00, 0xDF, 0xEB};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csImuData)
{
    uint8_t buf[34] = {0xA5, 0xA5, 0xA2, 0x1C, 0x81, 0x0C, 0x37, 0xA7, 0xC5, 0xAC, 0x37, 0x7B, 0xA8, 0x82, 0x3F, 0x80, 0x00, 0x65, 0x82, 0x0C, 0x37, 0xA7, 0xC5, 0xAC, 0x37, 0x7B, 0xA8, 0x82, 0x37, 0x49, 0x53, 0x9C, 0x0C, 0x23};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csAccVec)
{
    uint8_t buf[20] = {0xA5, 0xA5, 0xA2, 0x0E, 0x81, 0x0C, 0x37, 0xA7, 0xC5, 0xAC, 0x37, 0x7B, 0xA8, 0x82, 0x3F, 0x80, 0x00, 0x65, 0xD6, 0x1A};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csAngVec)
{
    uint8_t buf[20] = {0xA5, 0xA5, 0xA2, 0x0E, 0x82, 0x0C, 0x37, 0xA7, 0xC5, 0xAC, 0x37, 0x7B, 0xA8, 0x82, 0x37, 0x49, 0x53, 0x9C, 0x22, 0x40};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csMagVec)
{
    uint8_t buf[20] = {0xA5, 0xA5, 0xA2, 0x0E, 0x83, 0x0C, 0x37, 0xA7, 0xC5, 0xAC, 0x37, 0x7B, 0xA8, 0x82, 0x37, 0x49, 0x53, 0x9C, 0x23, 0x4E};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csTheta)
{
    uint8_t buf[20] = {0xA5, 0xA5, 0xA2, 0x0E, 0x84, 0x0C, 0x37, 0xA7, 0xC5, 0xAC, 0x37, 0x7B, 0xA8, 0x82, 0x37, 0x49, 0x53, 0x9C, 0x24, 0x5C};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csDeltaVel)
{
    uint8_t buf[20] = {0xA5, 0xA5, 0xA2, 0x0E, 0x85, 0x0C, 0x37, 0xA7, 0xC5, 0xAC, 0x37, 0x7B, 0xA8, 0x82, 0x37, 0x49, 0x53, 0x9C, 0x25, 0x6A};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csPressure)
{
    uint8_t buf[12] = {0xA5, 0xA5, 0xA2, 0x06, 0x86, 0x04, 0x00, 0x00, 0x03, 0xFD, 0x7C, 0xB4};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csTemper)
{
    uint8_t buf[12] = {0xA5, 0xA5, 0xA2, 0x06, 0x87, 0x04, 0x00, 0x00, 0x00, 0x19, 0x96, 0xD0};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}
TEST_F(ChecksumTest, csGPS)
{
    uint8_t buf[20] = {0xA5, 0xA5, 0xA2, 0x0E, 0x88, 0x0C, 0x41, 0x09, 0x44, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x07, 0x2F, 0x00, 0x08, 0x1A, 0x32};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    // EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]); //last byte should be 0x15 not 32
}
TEST_F(ChecksumTest, csAuxAcc)
{
    uint8_t buf[20] = {0xA5, 0xA5, 0xA2, 0x0E, 0x89, 0x0C, 0x37, 0xA7, 0xC5, 0xAC, 0x37, 0x7B, 0xA8, 0x82, 0x3F, 0x80, 0x00, 0x65, 0xDE, 0x8A};
    P_Checksum thing = Checksum(buf, sizeof(buf));
    EXPECT_EQ(thing.f1, buf[sizeof(buf)-2]);
    EXPECT_EQ(thing.f2, buf[sizeof(buf)-1]);
}

class EncoderTest : public ::testing::Test {
protected:
    static void SetUpTestSuite()
    {
        if (openSerial() != 0) return;
    }

    static void TearDownTestSuite()
    {
        close(serialFd);
        serialFd = -1;
        std::cout << "close test" << std::endl;
    }
};

TEST_F(EncoderTest, encodePing)
{
    uint8_t pingMsg[] = {0xA5, 0xA5, 0x01, 0x02, 0x02, 0x00, 0x4F, 0x25};
    Status sts = ping(pingMsg, sizeof(pingMsg));
    EXPECT_EQ(sts, Status::Ok);
}
TEST_F(EncoderTest, encodeSelectSensorDataNewSettings)
{
    uint8_t selectMsg[] = {0xA5, 0xA5, 0x02, 0x05, 0x0C, 0x03, 0x01, 0x81, 0x82, 0x64, 0xF0};
    uint8_t cmd[] = {0x81, 0x82};
    uint8_t res[sizeof(selectMsg)];
    Status sts = selectSensorData(cmd, 2, res, sizeof(res));
    EXPECT_EQ(sts, Status::Ok);
    for (int i = 0; i < sizeof(selectMsg); i++)
    {
        EXPECT_EQ(res[i], selectMsg[i]);
    }
}
TEST_F(EncoderTest, encodeResetSensorData)
{
    uint8_t resetReply[] = {0xa5, 0xa5, 0x02, 0x03, 0x0c, 0x01, 0x05, 0x61, 0xa2};
    uint8_t res[sizeof(resetReply)];
    Status sts = resetSensorData(res, sizeof(resetReply));
    
    EXPECT_EQ(sts, Status::Ok);
    for (int i = 0; i < sizeof(resetReply); i++)
    {
        EXPECT_EQ(res[i], resetReply[i]);
    }
}
TEST_F(EncoderTest, encodeGetDevModel)
{
    uint8_t modelMsg[] = {0xA5, 0xA5, 0x01, 0x02, 0x05, 0x00, 0x52, 0x2B}; 
    uint8_t res[sizeof(modelMsg)];
    Status sts = sendGetDevCmd(0x05, res, sizeof(res));
    EXPECT_EQ(sts, Status::Ok);
    for (int i = 0; i < sizeof(modelMsg); i++)
    {
        EXPECT_EQ(res[i], modelMsg[i]);
    }
}
TEST_F(EncoderTest, encodeGetDevSN)
{
    uint8_t SNMsg[] = {0xA5, 0xA5, 0x01, 0x02, 0x06, 0x00, 0x53, 0x2D};
    uint8_t res[sizeof(SNMsg)];
    Status sts = sendGetDevCmd(0x06, res, sizeof(res));
    EXPECT_EQ(sts, Status::Ok);
    for (int i = 0; i < sizeof(SNMsg); i++)
    {
        EXPECT_EQ(res[i], SNMsg[i]);
    }
}
TEST_F(EncoderTest, encodeGetDevFW)
{
    uint8_t FWMsg[] = {0xA5, 0xA5, 0x01, 0x02, 0x07, 0x00, 0x54, 0x2F};
    uint8_t res[sizeof(FWMsg)];
    Status sts = sendGetDevCmd(0x07, res, sizeof(res));
    EXPECT_EQ(sts, Status::Ok);
    for (int i = 0; i < sizeof(FWMsg); i++)
    {
        EXPECT_EQ(res[i], FWMsg[i]);
    }
}
TEST_F(EncoderTest, encodeInvalidSelectSensors_zero)
{
    uint8_t res[9];
    Status sts = selectSensorData({}, 0, res, 9);
    EXPECT_EQ(sts, Status::InvalidArgument);
}
TEST_F(EncoderTest, encodeInvalidSelectSensors_ten) //redundant with below b/c to have 10 you need an invalid code
{
    uint8_t msg[] = {0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a};
    uint8_t res[sizeof(msg) + 9];
    Status sts = selectSensorData(msg, 10, res, sizeof(res));
    EXPECT_EQ(sts, Status::InvalidArgument);
}
TEST_F(EncoderTest, encodeInvalidSelectSensors_badCmd)
{
    uint8_t msg[] = {0x80, 0x81};
    uint8_t res[sizeof(msg) + 9];
    Status sts = selectSensorData(msg, 2, res, sizeof(res));
    EXPECT_EQ(sts, Status::InvalidArgument);
}

class ParserTest : public ::testing::Test {
protected:
    Parser parser;
    uint8_t type = 0x01;
    uint8_t expPL[22] = {0x80, 0x02, 0x07, 0x00, 0x87, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x52, 0x5F, 0x31, 0x5F, 0x32, 0x5F, 0x33};
    size_t payloadLen = 22;
    uint8_t toParse[28] = {0xA5, 0xA5, 0x01, 0x16, 0x80, 0x02, 0x07, 0x00, 0x87, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x52, 0x5F, 0x31, 0x5F, 0x32, 0x5F, 0x33, 0xA6, 0x25};

    static void SetUpTestSuite()
    {
        close(serialFd);
    }

    static void TearDownTestSuite()
    {
        
    }
};

TEST_F(ParserTest, parseControl)
{
    
    auto pack = parser.push(toParse, 28);
    EXPECT_EQ(pack.type, type) << "type = " << pack.type;
    for (int i = 0; i < payloadLen; i++)
    {
        EXPECT_EQ(pack.payload[i], expPL[i]) << "pl = " << pack.payload[i] << " exp = " << expPL[i];
    }
    EXPECT_EQ(pack.payloadLen, payloadLen) << "len = " << pack.payloadLen;
}
TEST_F(ParserTest, parseOneByOne)
{
    for (int i = 0; i < 27; i++)
    {
        uint8_t b[1] = {toParse[i]};
        parser.push(b, 1);
    }
    uint8_t b[1] = {toParse[27]};
    auto pack = parser.push(b, 1);
    EXPECT_EQ(pack.type, type) << "type = " << pack.type;
    for (int i = 0; i < payloadLen; i++)
    {
        EXPECT_EQ(pack.payload[i], expPL[i]) << "pl = " << pack.payload[i] << " exp = " << expPL[i];
    }
    EXPECT_EQ(pack.payloadLen, payloadLen) << "len = " << pack.payloadLen;
}
TEST_F(ParserTest, parseAllSplits)
{
    for (int i = 1; i < 27; i++)
    {
        uint8_t firstPart[i];
        std::memcpy(firstPart, toParse, i);
        parser.push(firstPart, i);

        uint8_t secondPart[sizeof(toParse) - i];
        std::memcpy(secondPart, toParse + i, sizeof(toParse) - i);
        auto pack = parser.push(secondPart, sizeof(toParse) - i);

        EXPECT_EQ(pack.payloadLen, payloadLen) << "len = " << pack.payloadLen << " i = " << i;
        for (int i = 0; i < payloadLen; i++)
        {
            EXPECT_EQ(pack.payload[i], expPL[i]) << "pl = " << pack.payload[i] << " exp = " << expPL[i];
        }
        
        EXPECT_EQ(pack.type, type) << "type = " << pack.type;

    }  
}

TEST_F(ParserTest, parseStartGarbage)
{
    uint8_t garbage[7] = {0x56, 0x55, 0x5a, 0xa5, 0x87, 0x99, 0xb2};

    uint8_t startsWithGarbage[sizeof(toParse) + sizeof(garbage)];
    std::memcpy(startsWithGarbage, garbage, sizeof(garbage));
    std::memcpy(startsWithGarbage + sizeof(garbage), toParse, sizeof(toParse));

    auto pack = parser.push(startsWithGarbage, sizeof(startsWithGarbage));

    EXPECT_EQ(pack.type, type) << "type = " << pack.type;
    for (int i = 0; i < payloadLen; i++)
    {
        EXPECT_EQ(pack.payload[i], expPL[i]) << "pl = " << pack.payload[i] << " exp = " << expPL[i];
    }
    EXPECT_EQ(pack.payloadLen, payloadLen) << "len = " << pack.payloadLen;
}
// TEST_F(ParserTest, parseBetweenGarbage)
// {
//     auto pack0 = parser.push(toParse, 28);

//     uint8_t garbage[7] = {0x56, 0x55, 0x5a, 0xa5, 0x87, 0x99, 0xb2};
//     parser.push(garbage, 7);

//     auto pack = parser.push(toParse, 28);
//     EXPECT_EQ(pack.type, type) << "type = " << pack.type;
//     for (int i = 0; i < payloadLen; i++)
//     {
//         EXPECT_EQ(pack.payload[i], expPL[i]) << "pl = " << pack.payload[i] << " exp = " << expPL[i];
//     }
//     EXPECT_EQ(pack.payloadLen, payloadLen) << "len = " << pack.payloadLen;
// }
TEST_F(ParserTest, parseEndGarbage)
{
    uint8_t garbage[7] = {0x56, 0x55, 0x5a, 0xa5, 0x87, 0x99, 0xb2};

    uint8_t endsWithGarbage[sizeof(toParse) + sizeof(garbage)];
    std::memcpy(endsWithGarbage, toParse, sizeof(toParse));
    std::memcpy(endsWithGarbage + sizeof(toParse), garbage, sizeof(garbage));

    auto pack = parser.push(endsWithGarbage, sizeof(endsWithGarbage));

    EXPECT_EQ(pack.type, type) << "type = " << pack.type;
    for (int i = 0; i < payloadLen; i++)
    {
        EXPECT_EQ(pack.payload[i], expPL[i]) << "pl = " << pack.payload[i] << " exp = " << expPL[i] << " i = " << i << " payloadLen = " << payloadLen;
    }
    EXPECT_EQ(pack.payloadLen, payloadLen) << "len = " << pack.payloadLen;
}
TEST_F(ParserTest, parseBadChecksum)
{
    size_t size = sizeof(toParse);
    uint8_t badThenGood[2 * size];
    std::memcpy(badThenGood, toParse, size);
    badThenGood[size - 1] = 0x00; //corrupt the checksum
    std::memcpy(badThenGood + size, toParse, size); //good packet
    auto pack = parser.push(badThenGood, sizeof(badThenGood));
    EXPECT_EQ(pack.type, type) << "type = " << pack.type;
    for (int i = 0; i < payloadLen; i++)
    {
        EXPECT_EQ(pack.payload[i], expPL[i]) << "pl = " << pack.payload[i] << " exp = " << expPL[i];
    }
    EXPECT_EQ(pack.payloadLen, payloadLen) << "len = " << pack.payloadLen;
}
TEST_F(ParserTest, parseFalseLong)
{
    uint8_t falseLong[] = {0x88, 0xa5, 0xa5, 0x01, 0x80, 0x81, 0x0c, 0x55, 0x54, 0x7b, 0x98};
    uint8_t toSend[sizeof(falseLong) + sizeof(toParse)];
    std::memcpy(toSend, falseLong, sizeof(falseLong));
    std::memcpy(toSend + sizeof(falseLong), toParse, sizeof(toParse));
    auto pack = parser.push(toSend, sizeof(toSend));
    EXPECT_EQ(pack.type, type) << "type = " << pack.type;
    for (int i = 0; i < payloadLen; i++)
    {
        EXPECT_EQ(pack.payload[i], expPL[i]) << "pl = " << pack.payload[i] << " exp = " << expPL[i];
    }
    EXPECT_EQ(pack.payloadLen, payloadLen) << "len = " << pack.payloadLen;
}
TEST_F(ParserTest, parseMax)
{
    // uint8_t bufMax[255];
    // std::memset(bufMax, 0, sizeof(bufMax));
    // bufMax[0] = 0xa5; bufMax[1] = 0xa5; bufMax[2] = 0xa2;
    // auto msg = makeMsg(bufMax, sizeof(bufMax));
    // auto pack = parser.push(bufMax, sizeof(bufMax));

    uint8_t bufMax[255];
    std::memset(bufMax, 0, sizeof(bufMax));
    bufMax[0] = 0xa5; bufMax[1] = 0xa5; bufMax[2] = 0xa2;
    makeMsg(bufMax, sizeof(bufMax));
    // auto msg = makeMsg(bufMax, sizeof(bufMax));
    
    auto pack = parser.push(bufMax, sizeof(bufMax));

    EXPECT_NE(pack.type, 0x00);
    EXPECT_NE(pack.payloadLen, 0x00) << "len = " << pack.payloadLen;
}

class FieldIteratorTest : public ::testing::Test {
protected:
    Parser parser;
    static void SetUpTestSuite()
    {
    }

    static void TearDownTestSuite()
    {
        
    }
};
TEST_F(FieldIteratorTest, parseSingleField)
{
    uint8_t pingReply[] = {0xA5, 0xA5, 0x01, 0x04, 0x80, 0x02, 0x02, 0x00, 0xD3, 0xCF};
    auto pack = parser.push(pingReply, sizeof(pingReply));
    FieldIterator iterator(pack);

    Field firstField = iterator.getField(0);
    EXPECT_EQ(firstField.type, 0x01) << "type = " << firstField.type;
    EXPECT_EQ(firstField.code, 0x80) << "code = " << firstField.code;
    EXPECT_EQ(firstField.size, 0x02) << "size = " << firstField.size;
    EXPECT_EQ(firstField.data, pack.payload + 2);
}
TEST_F(FieldIteratorTest, parseDoubleField)
{
    
    uint8_t ImuReply[] = {0xA5, 0xA5, 0xA2, 0x1C, 0x81, 0x0C, 0x37, 0xA7, 0xC5, 0xAC, 0x37, 0x7B, 0xA8, 0x82, 0x3F, 0x80, 0x00, 0x65, 0x82, 0x0C, 0x37, 0xA7, 0xC5, 0xAC, 0x37, 0x7B, 0xA8, 0x82, 0x37, 0x49, 0x53, 0x9C, 0x0C, 0x23};
    auto pack = parser.push(ImuReply, sizeof(ImuReply));
    FieldIterator iterator(pack);

    Field firstField = iterator.getField(0);
    EXPECT_EQ(firstField.type, 0xA2) << "type = " << firstField.type;
    EXPECT_EQ(firstField.code, 0x81) << "code = " << firstField.code;
    EXPECT_EQ(firstField.size, 0x0C) << "size = " << firstField.size;
    EXPECT_EQ(firstField.data, pack.payload + 2);

    Field secondField = iterator.getField(iterator.next);
    EXPECT_EQ(secondField.type, 0xA2) << "type = " << secondField.type;
    EXPECT_EQ(secondField.code, 0x82) << "code = " << secondField.code;
    EXPECT_EQ(secondField.size, 0x0C) << "size = " << secondField.size;
    EXPECT_EQ(secondField.data, pack.payload + 2 + firstField.size + 2);
}
TEST_F(FieldIteratorTest, parseBadSize)
{
    
    uint8_t pingReply[10] = {0xA5, 0xA5, 0x01, 0x04, 0x80, 0x07, 0x02, 0x00, 0xD3, 0xCF};
    auto cs = Checksum(pingReply, 10);
    pingReply[8] = cs.f1;
    pingReply[9] = cs.f2;
    auto pack = parser.push(pingReply, sizeof(pingReply));
    FieldIterator iterator(pack);

    iterator.getField(0);
    
    EXPECT_EQ(iterator.malformed, true);
}

TEST(NoAlloc, parseAndEncode) {
    Parser parser;                 // set up before measuring
    size_t before = g_allocs;

    /* push bytes, iterate fields, encode ping and selectSensors */
    uint8_t pingMsg[] = {0xA5, 0xA5, 0x01, 0x02, 0x02, 0x00, 0x4F, 0x25};
    ping(pingMsg, sizeof(pingMsg));

    uint8_t pingReply[] = {0xA5, 0xA5, 0x01, 0x04, 0x80, 0x02, 0x02, 0x00, 0xD3, 0xCF};
    auto pack = parser.push(pingReply, sizeof(pingReply));
    FieldIterator iterator(pack);

    iterator.getField(0);

    uint8_t cmd[] = {0x81, 0x82};
    uint8_t res[11];
    selectSensorData(cmd, 2, res, sizeof(res));
    
    EXPECT_EQ(g_allocs, before);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

}
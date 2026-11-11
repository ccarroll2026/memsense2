#ifndef P_structs
#define P_structs

#include <cstdint>

enum P_Msg_Type {
    PE_Basic_Msg = 0x01,
    PE_IMU_Msg = 0x02,
    PE_Data_Msg = 0xA2
};

enum P_Error_Codes {
    PE_Err_OK = 0x00,
    PE_Err_CS = 0x01,
    PE_Err_Msg_Type = 0x02,
    PE_Err_Msg_Code = 0x03,
    PE_Err_Param = 0x04
};

enum P_Filter_Functions {
    PE_New = 0x01,
    PE_Req = 0x02,
    PE_Save = 0x03,
    PE_Load = 0x04,
    PE_Reset = 0x05
};

enum P_Config_All_Codes {
    PE_All_Save = 0x03,
    PE_All_Load = 0x04,
    PE_All_Reset = 0x05
};

enum P_UART_Baud_Rates {
    PE_Baud_9600 = 0x00002580,
    PE_Baud_19200 = 0x00004B00,
    PE_Baud_115200 = 0x0001C200,
    PE_Baud_230400 = 0x00038400,
    PE_Baud_460800 = 0x00070800,
    PE_Baud_921600 = 0x000E1000
};

enum P_Filter_Bandwidth_Control_Codes {
    PE_Disable_Filter = 0x00,
    PE_IRF_25 = 0x01,
    PE_IRF_50 = 0x02,
    PE_IRF_75 = 0x03,
    PE_IRF_100 = 0x04,
    PE_IRF_10 = 0x05,
    PE_IRF_150 = 0x06,
    PE_IRF_200 = 0x07
};

enum P_Select_Sensors_Options {
    PE_Scaled_Acc = 0x81,
    PE_Scaled_Ang = 0x82,
    PE_Scaled_Mag = 0x83, //Doesn't work
    PE_Delta_Theta = 0x84,
    PE_Delta_Vel = 0x85,
    PE_Scaled_Pressure = 0x86, //Doesn't work
    PE_Scaled_Temp = 0x87,
    PE_GPS_Correlated_Time = 0x88,
    PE_Scaled_Aux_Acc = 0x89  //Doesn't work
};

enum P_Acc_Range_Codes {
    PE_Acc_2 = 0x00,
    PE_Acc_4 = 0x01,
    PE_Acc_8 = 0x02,
    PE_Acc_10 = 0x03,
    PE_Acc_15 = 0x04,
    PE_Acc_20 = 0x05,
    PE_Acc_40 = 0x06
};

enum P_Gyro_Range_Codes {
    PE_Gyro_120 = 0x01,
    PE_Gyro_240 = 0x02,
    PE_Gyro_480 = 0x03,
    PE_Gyro_960 = 0x04,
    PE_Gyro_1920 = 0x05,
    PE_Gyro_75 = 0x06,
    PE_Gyro_200 = 0x07
};

enum P_On_Off {
    PE_Off = 0x00,
    PE_On = 0x01
};

enum P_Type_1_Cmd_Codes {
    PE_Ping = 0x02,
    PE_Get_Dev = 0x03,
    PE_Dev_Reset = 0x04,
    PE_Get_Dev_Model = 0x05,
    PE_Get_Dev_SN = 0x06,
    PE_Get_Dev_FW = 0x07,
    PE_Get_Dev_Cal = 0x08,
    PE_Corr_GPS_Time = 0x09
};

enum P_Type_2_Cmd_Codes {
    PE_UART_Baud_Rate_Cmd = 0x01,
    PE_Config_Filter_Cmd = 0x03,
    PE_Sample_Rate_Config_Cmd = 0x04,
    PE_Select_Sensor_Cmd = 0x05,
    PE_Get_IMU_Internal_Sample_Rate = 0x06,
    PE_Config_Acc_Range = 0x07,
    PE_Config_Gyro_Range = 0x08,
    PE_Config_All = 0x09,
    PE_Data_On_Off = 0x0A,
    PE_XTRIG_On_Off = 0x0B,
    PE_Select_Sensors_Rev_B = 0x0C,
    PE_Config_Aux_Accel_Range = 0x0D
};

enum P_Type_1_Reply_Codes {
    PE_Ack = 0x80,
    PE_Dev_Commands = 0x83,
    PE_Dev_Model = 0x85,
    PE_Dev_SN = 0x86,
    PE_Dev_FW = 0x87,
    PE_Dev_Cal_Date = 0x88
};

enum P_Type_2_Reply_Codes {
    PE_IMU_Internal_Sample_Rate = 0x86
};

enum P_A2_Reply_Codes {
    PE_Scaled_Acc_Vec_Reply = 0x81,
    PE_Scaled_Ang_Rate_Vec_Reply = 0x82,
    PE_Scaled_Mag_Vec_Reply = 0x83,
    PE_Delta_Theta_Vec = 0x84,
    PE_Delta_Vel_Vec = 0x85,
    PE_Scaled_Pressure_Reply = 0x86,
    PE_Scaled_Temperature_Reply = 0x87,
    PE_GPS_Correlated_Time_Reply = 0x88,
    PE_Scaled_Aux_Acc_Vec = 0x89
};

//Internal use for keeping track of what structure size to expect
enum P_Expect_Struct { 
    PE_Cmd_Reply = 0x80,
    PE_Device_Msg_Reply = 0x83,
    PE_Get_Device_Reply = 0x85,
    PE_Sample_Rate_Reply = 0x86,
    PE_IMU_Data = 0xA2,
    PE_Scaled_Vec = 0x81,
    PE_Scaled_Single = 0x87,
    PE_Time_Data = 0x88
};

//Message Structs

struct P_Msg {
    uint8_t msg_code;
    uint8_t msg_size;
};

struct P_Checksum {
    uint8_t f1;
    uint8_t f2;

    friend std::ostream& operator<<(std::ostream& os, const P_Checksum& u)
    {
        os << std::hex <<
        static_cast<int>(u.f1) << ", " << 
        static_cast<int>(u.f2) << "\n" <<
        std::endl;
        return os;
    }
};

/*--------------------------------------*/
struct P_Error {
    P_Msg msg;
    uint8_t echo;
    uint8_t errorCode;
    friend std::ostream& operator<<(std::ostream& os, const P_Error u)
    {
        os << std::hex <<
        static_cast<int>(u.msg.msg_code) << ", " << 
        static_cast<int>(u.msg.msg_size) << ", " << 
        static_cast<int>(u.echo) << ", " << 
        static_cast<int>(u.errorCode) << "\n" <<
        std::endl;
        return os;
    }
};

//TODO: can be more commands than 4...
struct P_Device_Msg_Reply {
    P_Msg msg;
    uint8_t cmd1[2];
    uint8_t cmd2[2];
    uint8_t cmd3[2];
    uint8_t cmd4[2];
};

struct P_Get_Device_Reply {
    P_Msg msg;
    uint8_t s[16];
};

struct P_Correlate_Time_Cmd {
    P_Msg msg;
    uint8_t GPS_WN[2];
    uint8_t NTV[4];
};

/*--------------------------------------*/

struct P_Scaled_Vec {
    P_Msg msg;
    uint8_t X[4];
    uint8_t Y[4];
    uint8_t Z[4];
};

struct P_Scaled_Single {
    P_Msg msg;
    uint8_t X[4];
};

struct P_Time_Data {
    P_Msg msg;
    uint8_t GPS_SOW[8];
    uint8_t WN[2];
    uint8_t timestamp_flag[2];
};


#endif //P_structs
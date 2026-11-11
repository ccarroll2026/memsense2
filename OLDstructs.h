// #include <cstdint>

// enum P_Msg_Type {
//     PE_Basic_Msg = 0x01,
//     PE_IMU_Msg = 0x02,
//     PE_Data_Msg = 0xA2
// };

// enum P_Error_Codes {
//     PE_Err_OK = 0x00,
//     PE_Err_CS = 0x01,
//     PE_Err_Msg_Type = 0x02,
//     PE_Err_Msg_Code = 0x03,
//     PE_Err_Param = 0x04
// };

// enum P_Filter_Functions {
//     PE_New = 0x01,
//     PE_Req = 0x02,
//     PE_Save = 0x03,
//     PE_Load = 0x04,
//     PE_Reset = 0x05
// };

// enum P_Config_All_Codes {
//     PE_All_Save = 0x03,
//     PE_All_Load = 0x04,
//     PE_All_Reset = 0x05
// };

// enum P_UART_Baud_Rates {
//     PE_Baud_9600 = 0x00002580,
//     PE_Baud_19200 = 0x00004B00,
//     PE_Baud_115200 = 0x0001C200,
//     PE_Baud_230400 = 0x00038400,
//     PE_Baud_460800 = 0x00070800,
//     PE_Baud_921600 = 0x000E1000
// };

// enum P_Filter_Bandwidth_Control_Codes {
//     PE_Disable_Filter = 0x00,
//     PE_IRF_25 = 0x01,
//     PE_IRF_50 = 0x02,
//     PE_IRF_75 = 0x03,
//     PE_IRF_100 = 0x04,
//     PE_IRF_10 = 0x05,
//     PE_IRF_150 = 0x06,
//     PE_IRF_200 = 0x07
// };

// enum P_Select_Sensors_Options {
//     PE_Scaled_Acc = 0x81,
//     PE_Scaled_Ang = 0x82,
//     PE_Scaled_Mag = 0x83,
//     PE_Delta_Theta = 0x84,
//     PE_Delta_Vel = 0x85,
//     PE_Scaled_Pressure = 0x86,
//     PE_Scaled_Temp = 0x87,
//     PE_GPS_Correlated_Time = 0x88,
//     PE_Scaled_Aux_Acc = 0x89
// };

// enum P_Acc_Range_Codes {
//     PE_Acc_2 = 0x00,
//     PE_Acc_4 = 0x01,
//     PE_Acc_8 = 0x02,
//     PE_Acc_10 = 0x03,
//     PE_Acc_15 = 0x04,
//     PE_Acc_20 = 0x05,
//     PE_Acc_40 = 0x06
// };

// enum P_Gyro_Range_Codes {
//     PE_Gyro_120 = 0x01,
//     PE_Gyro_240 = 0x02,
//     PE_Gyro_480 = 0x03,
//     PE_Gyro_960 = 0x04,
//     PE_Gyro_1920 = 0x05,
//     PE_Gyro_75 = 0x06,
//     PE_Gyro_200 = 0x07
// };

// enum P_On_Off {
//     PE_Off = 0x00,
//     PE_On = 0x01
// };

// enum P_Type_1_Cmd_Codes {
//     PE_Ping = 0x02,
//     PE_Get_Dev = 0x03,
//     PE_Dev_Reset = 0x04,
//     PE_Get_Dev_Model = 0x05,
//     PE_Get_Dev_SN = 0x06,
//     PE_Get_Dev_FW = 0x07,
//     PE_Get_Dev_Cal = 0x08,
//     PE_Corr_GPS_Time = 0x09
// };

// enum P_Type_2_Cmd_Codes {
//     PE_UART_Baud_Rate_Cmd = 0x01,
//     PE_Config_Filter_Cmd = 0x03,
//     PE_Sample_Rate_Config_Cmd = 0x04,
//     PE_Select_Sensor_Cmd = 0x05,
//     PE_Get_IMU_Internal_Sample_Rate = 0x06,
//     PE_Config_Acc_Range = 0x07,
//     PE_Config_Gyro_Range = 0x08,
//     PE_Config_All = 0x09,
//     PE_Data_On_Off = 0x0A,
//     PE_XTRIG_On_Off = 0x0B,
//     PE_Select_Sensors_Rev_B = 0x0C,
//     PE_Config_Aux_Accel_Range = 0x0D
// };

// enum P_Type_1_Reply_Codes {
//     PE_Ack = 0x80,
//     PE_Dev_Commands = 0x83,
//     PE_Dev_Model = 0x85,
//     PE_Dev_SN = 0x86,
//     PE_Dev_FW = 0x87,
//     PE_Dev_Cal_Date = 0x88
// };

// enum P_Type_2_Reply_Codes {
//     PE_IMU_Internal_Sample_Rate = 0x86
// };

// enum P_A2_Reply_Codes {
//     PE_Scaled_Acc_Vec_Reply = 0x81,
//     PE_Scaled_Ang_Rate_Vec_Reply = 0x82,
//     PE_Scaled_Mag_Vec_Reply = 0x83,
//     PE_Delta_Theta_Vec = 0x84,
//     PE_Delta_Vel_Vec = 0x85,
//     PE_Scaled_Pressure_Reply = 0x86,
//     PE_Scaled_Temperature_Reply = 0x87,
//     PE_GPS_Correlated_Time_Reply = 0x88,
//     PE_Scaled_Aux_Acc_Vec = 0x89
// };

// //Internal use for keeping track of what structure size to expect
// enum P_Expect_Struct { 
//     PE_Cmd_Reply = 0x80,
//     PE_Device_Msg_Reply = 0x83,
//     PE_Get_Device_Reply = 0x85,
//     PE_Sample_Rate_Reply = 0x86,
//     PE_IMU_Data = 0xA2,
//     PE_Scaled_Vec = 0x81,
//     PE_Scaled_Single = 0x87,
//     PE_Time_Data = 0x88
// };

// //Message Structs

// struct P_Header {
//     uint8_t sync1 = 0xA5;
//     uint8_t sync2 = 0xA5;
//     uint8_t msg_type;
//     uint8_t payload_size;

//     friend std::ostream& operator<<(std::ostream& os, const P_Header& u)
//     {
//         os << std::hex <<
//         static_cast<int>(u.sync1) << ", " << 
//         static_cast<int>(u.sync2) << ", " << 
//         static_cast<int>(u.msg_type) << ", " << 
//         static_cast<int>(u.payload_size) << "\n" <<
//         std::endl;
//         return os;
//     }
// };

// struct P_Msg {
//     uint8_t msg_code;
//     uint8_t msg_size;

//     friend std::ostream& operator<<(std::ostream& os, const P_Msg& u)
//     {
//         os << std::hex <<
//         static_cast<int>(u.msg_code) << ", " << 
//         static_cast<int>(u.msg_size) << "\n" <<
//         std::endl;
//         return os;
//     }
// };

// struct P_Checksum {
//     uint8_t f1;
//     uint8_t f2;

//     friend std::ostream& operator<<(std::ostream& os, const P_Checksum& u)
//     {
//         os << std::hex <<
//         static_cast<int>(u.f1) << ", " << 
//         static_cast<int>(u.f2) << "\n" <<
//         std::endl;
//         return os;
//     }
// };

// // struct P_Error {
// //     uint8_t echo;
// //     uint8_t errorCode;

// //     friend std::ostream& operator<<(std::ostream& os, const P_Error& u)
// //     {
// //         os << std::hex <<
// //         static_cast<int>(u.echo) << ", " << 
// //         static_cast<int>(u.errorCode) << "\n" <<
// //         std::endl;
// //         return os;
// //     }
// // };

// /*--------------------------------------*/
// struct P_Error {
//     P_Msg msg;
//     uint8_t echo;
//     uint8_t errorCode;
// };

// struct P_Cmd {
//     P_Header header;
//     P_Msg msg;
//     P_Checksum cs;
// };

// struct P_Cmd_Reply {
//     P_Header header;
//     P_Msg msg;
//     P_Error err;
//     P_Checksum cs;

//     // friend std::ostream& operator<<(std::ostream& os, const P_Cmd_Reply& u)
//     // {
//     //     os << 
//     //     "P_Cmd_Reply" <<
//     //     std::hex <<
//     //     u.header << u.msg << u.err << u.cs
//     //     << std::endl;
//     //     return os;
//     // }
// };

// struct P_Device_Msg_Reply {
//     // P_Header header;
//     P_Msg msg;
//     // P_Error err;
//     // P_Msg msg2;
//     uint8_t cmd1[2];
//     uint8_t cmd2[2];
//     uint8_t cmd3[2];
//     uint8_t cmd4[2];
//     // P_Checksum cs;

//     // friend std::ostream& operator<<(std::ostream& os, const P_Device_Msg_Reply& u)
//     // {
//     //     os << std::hex <<
//     //     u.header << u.msg << u.err << u.msg <<
//     //     static_cast<int>(u.cmd1[0]) << ", " << static_cast<int>(u.cmd1[1]) <<
//     //     static_cast<int>(u.cmd2[0]) << ", " << static_cast<int>(u.cmd2[1]) <<
//     //     static_cast<int>(u.cmd3[0]) << ", " << static_cast<int>(u.cmd3[1]) <<
//     //     static_cast<int>(u.cmd4[0]) << ", " << static_cast<int>(u.cmd4[1]) <<
//     //     u.cs
//     //     << std::endl;
//     //     return os;
//     // }
// };

// struct P_Get_Device_Reply {
//     // P_Header header;
//     P_Msg msg;
//     // P_Error err;
//     // P_Msg msg2;
//     uint8_t s[16];
//     // P_Checksum cs;

//     // friend std::ostream& operator<<(std::ostream& os, const P_Get_Device_Reply& u)
//     // {
//     //     os << std::hex <<
//     //     u.header << u.msg << u.err <<
//     //     static_cast<int>(u.s[0]) << ", " << static_cast<int>(u.s[1]) <<
//     //     static_cast<int>(u.s[2]) << ", " << static_cast<int>(u.s[3]) <<
//     //     static_cast<int>(u.s[4]) << ", " << static_cast<int>(u.s[5]) <<
//     //     static_cast<int>(u.s[6]) << ", " << static_cast<int>(u.s[7]) <<
//     //     static_cast<int>(u.s[8]) << ", " << static_cast<int>(u.s[9]) <<
//     //     static_cast<int>(u.s[10]) << ", " << static_cast<int>(u.s[11]) <<
//     //     static_cast<int>(u.s[12]) << ", " << static_cast<int>(u.s[13]) <<
//     //     static_cast<int>(u.s[14]) << ", " << static_cast<int>(u.s[15]) <<
//     //     u.cs
//     //     << std::endl;
//     //     return os;
//     // }
// };

// struct P_Correlate_Time_Cmd {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t GPS_WN[2];
//     uint8_t NTV[4];
//     // P_Checksum cs;
// };

// struct P_Baud_Config {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t newSettings;
//     uint8_t baud_rate[4];
//     // P_Checksum cs;
// };

// struct P_Config_Filter {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t filt;
//     uint8_t bandwidth;
//     // P_Checksum cs;
// };

// struct P_Config_IMU_Sample_Rate {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t func;
//     uint8_t msb;
//     uint8_t lsb;
//     // P_Checksum cs;
// };

// struct P_Select_Sensors {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t func;
//     uint8_t reserved;
//     uint8_t acc_vec;
//     uint8_t ang_vec;
//     // P_Checksum cs;
// };

// struct P_Sample_Rate_Reply {
//     // P_Header header;
//     P_Msg msg;
//     // P_Error err;
//     // P_Msg msg2;
//     uint8_t MSB;
//     uint8_t LSB;
//     // P_Checksum cs;
//     // friend std::ostream& operator<<(std::ostream& os, const P_Sample_Rate_Reply& u)
//     // {
//     //     os << std::hex <<
//     //     u.header << u.msg << u.err << u.msg2 <<
//     //     static_cast<int>(u.MSB) << ", " << static_cast<int>(u.LSB) <<
//     //     u.cs
//     //     << std::endl;
//     //     return os;
//     // }
// };

// struct P_Config_Range {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t func;
//     uint8_t control;
//     // P_Checksum cs;
// };

// struct P_Config_All {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t all;
//     // P_Checksum cs;
// };

// struct P_Select_Sensor_Cmd {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t func;
//     uint8_t sensor_select;
//     uint8_t sensor_select_2;
//     // P_Checksum cs;
// };


// /*--------------------------------------*/

// struct P_IMU_Data {
//     P_Header header;
//     P_Msg msg;
//     uint8_t X_Accel[4];
//     uint8_t Y_Accel[4];
//     uint8_t Z_Accel[4];
//     P_Msg msg2;
//     uint8_t X_Gyro[4];
//     uint8_t Y_Gyro[4];
//     uint8_t Z_Gyro[4];
//     P_Checksum cs;

//     // friend std::ostream& operator<<(std::ostream& os, const P_IMU_Data& u)
//     // {
//     //     os << std::hex <<
//     //     u.header << u.msg << 
//     //     static_cast<int>(u.X_Accel[0]) << ", " << static_cast<int>(u.X_Accel[1]) << ", " << static_cast<int>(u.X_Accel[2]) << ", " << static_cast<int>(u.X_Accel[3]) <<
//     //     static_cast<int>(u.Y_Accel[0]) << ", " << static_cast<int>(u.Y_Accel[1]) << ", " << static_cast<int>(u.Y_Accel[2]) << ", " << static_cast<int>(u.Y_Accel[3]) <<
//     //     static_cast<int>(u.Z_Accel[0]) << ", " << static_cast<int>(u.Z_Accel[1]) << ", " << static_cast<int>(u.Z_Accel[2]) << ", " << static_cast<int>(u.Z_Accel[3]) <<
//     //     u.msg <<
//     //     static_cast<int>(u.X_Gyro[0]) << ", " << static_cast<int>(u.X_Gyro[1]) << ", " << static_cast<int>(u.X_Gyro[2]) << ", " << static_cast<int>(u.X_Gyro[3]) <<
//     //     static_cast<int>(u.Y_Gyro[0]) << ", " << static_cast<int>(u.Y_Gyro[1]) << ", " << static_cast<int>(u.Y_Gyro[2]) << ", " << static_cast<int>(u.Y_Gyro[3]) <<
//     //     static_cast<int>(u.Z_Gyro[0]) << ", " << static_cast<int>(u.Z_Gyro[1]) << ", " << static_cast<int>(u.Z_Gyro[2]) << ", " << static_cast<int>(u.Z_Gyro[3]) <<
//     //     u.cs
//     //     << std::endl;
//     //     return os;
//     // }
// };

// struct P_Scaled_Acc_Vec {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t X_Accel[4];
//     uint8_t Y_Accel[4];
//     uint8_t Z_Accel[4];
//     // P_Checksum cs;
// };

// struct P_Scaled_Vec {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t X[4];
//     uint8_t Y[4];
//     uint8_t Z[4];
//     // P_Checksum cs;

//     // friend std::ostream& operator<<(std::ostream& os, const P_Scaled_Vec& u)
//     // {
//     //     os << std::hex <<
//     //     u.header << u.msg << 
//     //     static_cast<int>(u.X[0]) << ", " << static_cast<int>(u.X[1]) << ", " << static_cast<int>(u.X[2]) << ", " << static_cast<int>(u.X[3]) <<
//     //     static_cast<int>(u.Y[0]) << ", " << static_cast<int>(u.Y[1]) << ", " << static_cast<int>(u.Y[2]) << ", " << static_cast<int>(u.Y[3]) <<
//     //     static_cast<int>(u.Z[0]) << ", " << static_cast<int>(u.Z[1]) << ", " << static_cast<int>(u.Z[2]) << ", " << static_cast<int>(u.Z[3]) <<
//     //     u.cs
//     //     << std::endl;
//     //     return os;
//     // }
// };

// struct P_Scaled_Single {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t X[4];
//     // P_Checksum cs;
//     // friend std::ostream& operator<<(std::ostream& os, const P_Scaled_Single& u)
//     // {
//     //     os << std::hex <<
//     //     u.header << u.msg << 
//     //     static_cast<int>(u.X[0]) << ", " << static_cast<int>(u.X[1]) << ", " << static_cast<int>(u.X[2]) << ", " << static_cast<int>(u.X[3]) <<
//     //     u.cs
//     //     << std::endl;
//     //     return os;
//     // }
// };

// struct P_Time_Data {
//     // P_Header header;
//     P_Msg msg;
//     uint8_t GPS_SOW[8];
//     uint8_t WN[2];
//     uint8_t timestamp_flag[2];
//     // P_Checksum cs;

//     // friend std::ostream& operator<<(std::ostream& os, const P_Time_Data& u)
//     // {
//     //     os << std::hex <<
//     //     u.header << u.msg << 
//     //     static_cast<int>(u.GPS_SOW[0]) << ", " << static_cast<int>(u.GPS_SOW[1]) << ", " << static_cast<int>(u.GPS_SOW[2]) << ", " << static_cast<int>(u.GPS_SOW[3]) <<
//     //     static_cast<int>(u.GPS_SOW[4]) << ", " << static_cast<int>(u.GPS_SOW[5]) << ", " << static_cast<int>(u.GPS_SOW[6]) << ", " << static_cast<int>(u.GPS_SOW[7]) <<
        
//     //     static_cast<int>(u.WN[0]) << ", " << static_cast<int>(u.WN[1]) <<
//     //     static_cast<int>(u.timestamp_flag[0]) << ", " << static_cast<int>(u.timestamp_flag[1]) << 
//     //     u.cs
//     //     << std::endl;
//     //     return os;
//     // }
// };


// P_Error ack;
// P_Device_Msg_Reply devComm;
// P_Get_Device_Reply devMod;
// P_Get_Device_Reply devSN;
// P_Get_Device_Reply devFW;
// P_Get_Device_Reply devCalDate;
// P_IMU_Data imuData;
// P_Scaled_Vec accVec;
// P_Scaled_Vec angVec;
// P_Scaled_Vec magVec;
// P_Scaled_Vec deltaThetaVec;
// P_Scaled_Vec deltaVelVec;
// P_Scaled_Single pressure;
// P_Scaled_Single temperature;
// P_Time_Data gpsCorrTime;
// P_Scaled_Vec auxAccVec;


/*

a5

a5a5a22282c3d6e547a3be3336a3c2ca89b81cbbb6b286bf6bce23f5981d387427d3dafd46081cbbb692fcbf6ff993f599c62874242fffa4fb

a5a5a22282c3e2ebdd93cbc4bc8bcc51e981cbbbc7e95bf76cb33f59c42b874242fffa9f5d

a5a5a22282c3e858dd33b43a16f3d23efa81cbbc5ae3cbf7a1913f5a3ff7874242fffa6c54

a5a5a22282c3dcb9ebb6c2e423cc8fb8381cbbc6b318bf7bfc3f5a63fb874242fffac8fa

a5a5a22282cba3640953d9294e43dad7b9a81cbbc45054bf78db93f5a486874242fffa52a2fac81cbbc2cf32bf736b63f5ab86874242fffaa7eb

a5a5a22282cbe36aff5bcccb4fd3dbefe7081cbbd73f42bf729793f5aca874242fffa5ffa

a5a5a22282cbe1be6483d656183d28bf2e81cbbdeaccabf7451f3f5a3218874242fffa5744

a5a5a22282cbd8faa5abd996e523d9a9e81cbbe8e3c5bf78f193f5a475d8781cbbf5c488bf73f5a77e7874242fffa1d

a5a5a22282c3deeb9bbcfee0bd1e64b581cbbe1c29fbf7da653f5a6b22873d657de81cbbcf51ebf78c7a3f5a1d62874242fffa92a6


*/
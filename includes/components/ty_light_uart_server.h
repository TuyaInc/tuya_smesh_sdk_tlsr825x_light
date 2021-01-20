/*************************************************************************
	> File Name: ty_uart_cmd_server.h
	> Author: 
	> Mail: 
	> Created Time: Wed 12 Jun 2019 17:08:28 CST
 ************************************************************************/

#ifndef __TY_LIGHT_UART_SERVER_H__
#define __TY_LIGHT_UART_SERVER_H__

#include "ty_light_basis_types.h"

//ONE FRAME
#define     F_TY_UART_CMD_SERVER_MIN_LEN                   0x08
#define     F_TY_UART_CMD_SERVER_MAX_LEN                   0xFF

#define     CMD_TY_UART_CMD_SERVER_WRITE                   0x01
#define     CMD_TY_UART_CMD_SERVER_WRITE_NOACK             0x02
#define     CMD_TY_UART_CMD_SERVER_READ                    0x03
#define     CMD_TY_UART_CMD_SERVER_STATUS                  0x04
#define     CMD_TY_UART_CMD_SERVER_DATA                    0x05
#define     CMD_TY_UART_CMD_SERVER_REV_CONTROL             0x06
#define     CMD_TY_UART_CMD_SERVER_REV_CONTROL_NOACK       0x07
#define     CMD_TY_UART_CMD_SERVER_REV_READ                0x08
#define     CMD_TY_UART_CMD_SERVER_REV_STATUS              0x09

#define     CMD_TY_UART_FEA_SERVER_ACTIVE_BT               0x00
#define     CMD_TY_UART_FEA_SERVER_RESET_BT                0x01
#define     CMD_TY_UART_FEA_SERVER_MESH_STA                0x02

#define     CMD_TY_UART_FEA_SERVER_LIGHT_CONTROL           0x10
#define     CMD_TY_UART_FEA_SERVER_SCENE_ID                0x11
#define     CMD_TY_UART_FEA_SERVER_COMPLETE_SCENE          0x12
#define     CMD_TY_UART_FEA_SERVER_COUNTDOWN               0x13

#define     CMD_TY_UART_FEA_SERVER_TRANS_DP                0x20

//初始化结构体
#pragma pack(1)
typedef struct{
}ty_uart_cmd_server_params_s;
#pragma pack()
//对外功能函数集合结构体
#pragma pack(1)
typedef struct{
    u8 (*init)(ty_uart_cmd_server_params_s *param);//初始化
    void (*receive_cmd)(u8 cmd, u8 fur, u8 *para, u8 len);//接收处理命令
    // void (*send_cmd)(u8 cmd, u8 fur, u8 *para, u8 len);//发送命令

    // void (*set_uart_tx_callback)(void (*cb)( u8 *params, u8 len, u8 is_group));//MCU回复或上报数据
    // void (*set_uart_read_callback)(void (*cb)(u8 fur, u8 *params, u8 len));//MCU读取灯的状态
    void (*set_uart_ctl_callback)(void (*cb)(u8 fur, u8 *params, u8 len));//MCU控制灯
}ty_uart_cmd_server_s;
#pragma pack()

extern ty_uart_cmd_server_s ty_uart_cmd_server;



/**
* @description:send data to MCU
* @param[in] {u8 cmd} command, view general serial port light protocol document
* @param[in] {u8 fur} feature, view general serial port light protocol document
* @param[in] {u8 *para} data buf
* @param[in] {u8 len} data length
* @return: none 
**/
void ty_uart_cmd_server_send_cmd_process(u8 cmd, u8 fur, u8 *para, u8 len);


/**
* @description: set if send data to mcu flag
* @param {bool flag} 1 -> send; 0 -> not send
* @return: none 
**/
void ty_uart_cmd_server_set_txtomcu_flag(bool flag);

/**
* @description: get if send data to mcu flag
* @param {none}
* @return: flag: 1 -> send; 0 -> not send
**/
u8 ty_uart_cmd_server_get_txtomcu_flag(void);

#endif

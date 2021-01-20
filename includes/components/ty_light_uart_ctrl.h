/*************************************************************************
	> File Name: app_uart.h
	> Author: 
	> Mail: 
	> Created Time: Tue 26 Feb 2019 22:29:40 CST
 ************************************************************************/

#ifndef __TY_LIGHT_UART_CTRL_H__
#define __TY_LIGHT_UART_CTRL_H__

#include "ty_light_basis_types.h"
#include "ty_light_uart_server.h"




#pragma pack(1)
typedef struct{
	void (*get_ctrl_data_switch_state_callback)(u8 (*cb)(void));
	void (*get_ctrl_data_mode_state_callback)(u8 (*cb)(void));
	void (*get_ctrl_data_colororigin_hsv_callback)(u16 (*cb)(u16 *out_hue, u16 *out_sat, u16 *out_value));
	void (*get_ctrl_data_colororigin_bright_callback)(u16 (*cb)(void));
	void (*get_ctrl_data_colororigin_temper_callback)(u16 (*cb)(void));
	void (*get_uart_reply_addr_callback)(u16 (*cb)(void));
	void (*get_ctrl_data_countdown_data_callback)(void (*cb)(u8 *out_countdown_data));
}ty_light_uart_ctrl_for_mcu_uart_common_s;
#pragma pack()
extern ty_light_uart_ctrl_for_mcu_uart_common_s ty_light_uart_ctrl_for_mcu_uart_common;



/**
* @description: get vendor dps
* @param {u8 *buf} the data after transform
* @param {u8 buf_left_size} 
* @param {u8 *buf_used_size} the length after transform
* @param {u8 *para} input data
* @param {u8 len} input data length
* @param {u8 is_std2nstd} standard to not standard or not standard to standard
* @return: 1 -> success ; 0 -> fail
**/
int app_mesh_vendor_get_dps(u8 *buf, u8 buf_left_size, u8 *buf_used_size, u8 *para, u8 len, u8 is_std2nstd);

/**
* @description: The control information of the lamp is sent to MCU,
             whether it is sent to MCU is decided by ty_uart_cmd_server_get_txtomcu_flag()
* @param {u8 fur} feature
* @param {u8 *params} input data
* @param {u8 len} input data length
* @return: 0
**/
int app_mesh_uart_write(u8 fur, u8 *params, u8 len);

/**
* @description: send uart dps to mesh for upload
* @param {u8 *params} input data
* @param {u8 len} input data length
* @param {u8 is_group}
* @return: none
**/
void app_mesh_uart_upload(u8 *params, u8 len, u8 is_group);

/**
* @description: MCU reads the relevant state of the lamp
* @param {u8 fur} feature
* @param {u8 *params} input data
* @param {u8 len} input data length
* @return: none
**/
void app_mesh_uart_read(u8 fur, u8 *params, u8 len);

#endif

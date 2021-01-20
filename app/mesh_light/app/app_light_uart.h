/*************************************************************************
	> File Name: app_uart.h
	> Author: 
	> Mail: 
	> Created Time: Tue 26 Feb 2019 22:29:40 CST
 ************************************************************************/

#ifndef __APP_LIGHT_UART_H__
#define __APP_LIGHT_UART_H__

#include "ty_light_basis_types.h"
#include "ty_light_uart_ctrl.h"


/**
* @description: MCU controls the lamp through uart
* @param {u8 fur} feature
* @param {u8 *params} input data
* @param {u8 len} input data length
* @return: none
**/
void app_mesh_uart_ctl(u8 fur, u8 *params, u8 len);

/**
* @description: The control information of the lamp is sent to MCU,
             whether it is sent to MCU is decided by ty_uart_cmd_server_get_txtomcu_flag()
* @param {uint32_t opcode} feature
* @param {u8 *params} input data
* @param {u8 len} input data length
* @return: 0
**/
void app_mesh_data_to_uart_send(uint32_t opcode, uint8_t *data,uint16_t data_len);
#endif

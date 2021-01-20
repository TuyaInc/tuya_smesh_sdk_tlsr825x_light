/*************************************************************************
	> File Name: app_common.h
	> Author: 
	> Mail: 
	> Created Time: Mon 25 Mar 2019 13:55:08 CST
 ************************************************************************/

#ifndef __APP_COMMON_H__
#define __APP_COMMON_H__

#include "ty_light_basis_types.h"

enum{
	VD_CMD_WORK_MODE = 0x02,
    VD_CMD_SCENE_DATA = 0x06,
    VD_CMD_COUNTDOWN = 0x07,
	VD_CMD_MUSIC_DATA = 0x08,
	VD_CMD_POWER_MEMORY = 0x21,
	VD_CMD_DONT_DISTURB = 0x22,
};

/**
* @description: response light state after control in on-off model 
* @param[in] {IN bool Onoff} control command: 1->on,0->off
* @return: none
**/
void app_light_ctrl_data_onoff_response(IN bool Onoff);

/**
* @description: light data of vendor model send without ACK
* @param[in] {IN u16 DstAddr} destination address
* @param[in] {IN u8 VendorData} data of vendor model
* @param[in] {IN u8 DataLen} data length
* @return: none 
**/
void app_light_vendor_data_publish(IN u16 DstAddr,IN u8 *VendorData,IN u8 DataLen);

void tuya_mesh_data_recv_callback(uint16_t src_addr, uint16_t dst_addr, uint32_t opcode, uint8_t *data, uint16_t data_len, uint8_t ttl);

u16 tuya_get_uart_reply_addr(void);

#endif


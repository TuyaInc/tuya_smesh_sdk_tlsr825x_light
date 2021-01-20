/*************************************************************************
	> File Name: tuya_node_init.h
	> Author: 
	> Mail: 
	> Created Time: Mon 25 Mar 2019 13:55:08 CST
 ************************************************************************/

#ifndef _APP_UNPROV_BEACON_H
#define _APP_UNPROV_BEACON_H

#include "board.h"

/**
* @description: mesh information init
* @param {none}
* @return: none 
**/
extern void mesh_para_init(void);

/**
* @description: get mesh category of prov info: uuid + fast prov from json
* @param {none}
* @return: none
**/
uint16_t tuya_get_mesh_category(void);

/**
* @description: get pid from json
* @param {none}
* @return: tuya_pid
**/
uint8_t* tuya_get_pid(void);

/**
* @description: query get number is key or pid from json
* @param {none}
* @return: CONFIG_OEM_ON -> 1:key; 0:pid
**/
uint8_t tuya_get_is_key_or_pid(void);

/**
* @description: get if need pub addr from json
* @param {none}
* @return: LOCAL_CONTROL_ON
**/
uint8_t app_tuya_get_if_need_pub_addr(void);

/**
* @description: get firmware version from json
* @param {none}
* @return: FW_VERSION_HEX 
**/
uint16_t tuya_get_firmware_version(void);

/**
* @description: get power reset count from json
* @param {none}
* @return: count 
**/
uint8_t tuya_get_power_reset_cnt(void);

/**
* @description: get power reset time from json
* @param {none}
* @return: POWER_RESET_TIME 
**/
uint8_t tuya_get_power_reset_time(void);

/**
* @description: get power reset recover time from json
* @param {none}
* @return: POWER_RESET_RECOVER_TIME 
**/
uint16_t tuya_get_power_reset_recover_time(void);

/**
* @description: set pid
* @param {uint8_t *pid} pid data
* @return: none 
**/
void app_tuya_pid_set(uint8_t *pid);

/**
* @description: set category
* @param {uint16_t mesh_category}
* @return: none 
**/
void app_tuya_mesh_category_set(uint16_t mesh_category);


    
#endif

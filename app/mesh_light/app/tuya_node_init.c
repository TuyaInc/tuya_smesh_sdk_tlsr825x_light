/*************************************************************************
	> File Name: tuya_node_init.c
	> Author: 
	> Mail: 
	> Created Time: Mon 25 Mar 2019 13:55:04 CST
 ************************************************************************/
#include "tuya_node_init.h"
#include "ty_nv.h"
#include "ty_string_op.h"
#include "ty_fast_provision.h"
#include "tuya_sigmesh_hal.h"
#include "ty_uart_cmd_server_for_factory_test.h"
#include "base_oem_config.h"

static node_info_t node_info = {0};

/**
* @description: get mesh category of prov info: uuid + fast prov from json
* @param {none}
* @return: none
**/
uint16_t tuya_get_mesh_category(void){
    return ty_light_json_config_get_category();
}

/**
* @description: get pid from json
* @param {none}
* @return: tuya_pid
**/
uint8_t* tuya_get_pid(void){
    static uint8_t tuya_pid[8] = {MESH_PID};
    return tuya_pid;
}

/**
* @description: query get number is key or pid from json
* @param {none}
* @return: CONFIG_OEM_ON -> 1:key; 0:pid
**/
uint8_t tuya_get_is_key_or_pid(void){
    return CONFIG_OEM_ON;
}

/**
* @description: get if need pub addr from json
* @param {none}
* @return: LOCAL_CONTROL_ON
**/
uint8_t app_tuya_get_if_need_pub_addr(void){
    return LOCAL_CONTROL_ON;
}

/**
* @description: get firmware version from json
* @param {none}
* @return: FW_VERSION_HEX 
**/
uint16_t tuya_get_firmware_version(void){
    return FW_VERSION_HEX;
}

/**
* @description: get power reset count from json
* @param {none}
* @return: count 
**/
uint8_t tuya_get_power_reset_cnt(void){
    return ty_light_json_config_get_resetnum();
}

/**
* @description: get power reset time from json
* @param {none}
* @return: POWER_RESET_TIME 
**/
uint8_t tuya_get_power_reset_time(void){
    return POWER_RESET_TIME;
}

/**
* @description: get power reset recover time from json
* @param {none}
* @return: POWER_RESET_RECOVER_TIME 
**/
uint16_t tuya_get_power_reset_recover_time(void){
    return POWER_RESET_RECOVER_TIME;
}


//--------

/**
* @description: update uuid (mac+Mesh category+PID/ProductKey)
* @param {none}
* @return: none 
**/
void tuya_uuid_update(void){
    memcpy(&(node_info.uuid[8]), tuya_get_pid(), 8);
    node_info.uuid[6] = tuya_get_mesh_category() >> 8;
    node_info.uuid[7] = tuya_get_mesh_category() & 0xFF;
    if(get_fast_prov_en_state()){
        node_info.uuid[6] = node_info.uuid[6] | 0x40;
    }
    else{
        node_info.uuid[6] = node_info.uuid[6] & (~0x40);
    }
    tuya_mesh_uuid_set(node_info.uuid);
}

/**
* @description: set category
* @param {uint16_t mesh_category}
* @return: none 
**/
void app_tuya_mesh_category_set(uint16_t mesh_category){
    //tuya_mesh_category = mesh_category;
    tuya_uuid_update();
}

/**
* @description: set pid
* @param {uint8_t *pid} pid data
* @return: none 
**/
void app_tuya_pid_set(uint8_t *pid){
    memcpy(tuya_get_pid(), pid, 8);
    tuya_uuid_update();
}

const u8  my_fwRevision_value [9] = {
    0,0,
    (FW_VERSION_HEX & 0xFF),((FW_VERSION_HEX >> 8) & 0xFF),
    (FW_VERSION_HEX & 0xFF),((FW_VERSION_HEX >> 8) & 0xFF),
    0x00, 0x00, 0x00
};

/**
* @description: mesh information init
* @param {none}
* @return: none 
**/
void mesh_para_init(void){
 

    node_info.ttl = 8;
    node_info.relay = 1;
    node_info.proxy = 1;
    node_info.fn = 1;
    node_info.lpn = 0;
    node_info.net_trans_count = 2;
    node_info.net_trans_steps = 2;

    node_info.company_id = VENDOR_ID;

    node_info.product_id.pid.is_fmkey = tuya_get_is_key_or_pid()? 1 : 0;

    node_info.product_id.pid.key_share = 1;

    node_info.product_id.pid.pub_addr_need = app_tuya_get_if_need_pub_addr()? 1 : 0;

    node_info.version_id = tuya_get_firmware_version();

    u8 mac[12],tmp;
    u8 ret = NV_USER_ITEM_LOAD(NV_USER_ITEM_MAC, mac, 12);
    ty_string_op_hexstr2hex(mac, 12, node_info.uuid);
    for(s8 i=0;i<3;i++){
        tmp = node_info.uuid[i];
        node_info.uuid[i] = node_info.uuid[5-i];
        node_info.uuid[5-i] = tmp;
    }
    node_info.uuid[6] = tuya_get_mesh_category() >> 8;
    node_info.uuid[7] = tuya_get_mesh_category() & 0xFF;
    memcpy(&(node_info.uuid[8]), tuya_get_pid(), 8);

    if(get_fast_prov_en_state()){
        node_info.uuid[6] = node_info.uuid[6] | 0x40;
    }
    else{
        node_info.uuid[6] = node_info.uuid[6] & (~0x40);
    }

    tuya_mesh_node_init(&node_info);

    u8 tuya_country_code = 0;
	u8 tuya_nv_sts = 0;
    tuya_nv_sts = NV_USER_ITEM_LOAD(NV_USER_ITEM_COUNTRY_CODE, &tuya_country_code, 1);
	if(0 != tuya_nv_sts)
	{
		tuya_country_code = 0;
	}
    if(COUNTRY_CODE_EU == tuya_country_code){
        tuya_mesh_rf_power_set(TUYA_RF_POWER_P6p14dBm);
    }
    else{
        tuya_mesh_rf_power_set(TUYA_RF_POWER_P10p46dBm);
    }
}


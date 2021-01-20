/*************************************************************************
	> File Name: app_factory_test.c
	> Author: 
	> Mail: 
	> Created Time: Wed 12 Aug 2020 11:57:09 CST
 ************************************************************************/
#include "ty_uart_cmd_server_for_factory_test.h"
#include "ty_gpio_base_test.h"
#include "base_oem_config.h"

static u8 if_enter_factory_test = 0;
ty_uart_cmd_server_for_factory_test_params_s ty_uart_cmd_server_for_factory_test_params = {
    .is_need_fingerprint_check = 1,
    .is_need_get_pid_from_cloud = 0,
    .if_support_country_code = 1,
    .if_support_gpio_test_num = 1,

    .is_need_update_mac_in_flash = 1,
    .mac_address = 0x76000, 

    .firmname = BUILD_FIRMNAME,
    .firmversion = FW_VERSION,
};


/**
* @description: enter factory test callback
* @param[in] none
* @return: none 
**/
void app_factory_test_enter_cb(void){
    PR_DEBUG("ENTER\n");   
    if_enter_factory_test = 1;
}


/**
* @description: start factory rssi test callback
* @param[in] {para} data
* @param[in] {len} length
* @return: 0 
**/
u8 app_factory_test_rssi_test_start_cb(u8 *para, u8 len){
    app_rssi_update_test_name(para, len);
    app_rssi_init();
    return 0;
}


/**
* @description: rssi test callback
* @param[in] none
* @return: rssi or 0
**/
u8 app_factory_test_rssi_test_cb(void){
    int rssi = 0;
    ty_rssi_base_test_stop();
    if(ty_rssi_base_test_get_rssi_avg(&rssi) != -1){
       if(rssi > 0 && rssi < 75){
            return rssi;
       }  
    }
    return 0;
}


/**
* @description: rssi test reset callback
* @param[in] none
* @return: none
**/
void app_factory_test_reset_cb(void){

}

//------------------------------------------------------------------------


/**
* @description: factory test init
* @param[in] none
* @return: none
**/
void app_factory_test_init(void){
    ty_uart_cmd_server_for_factory_test.init(&ty_uart_cmd_server_for_factory_test_params);
    ty_uart_cmd_server_for_factory_test.set_enter_callback(&app_factory_test_enter_cb);
    ty_uart_cmd_server_for_factory_test.set_gpio_test_callback(&ty_gpio_base_test_auto);
    ty_uart_cmd_server_for_factory_test.set_rssi_test_callback(&app_factory_test_rssi_test_cb);
    ty_uart_cmd_server_for_factory_test.set_rssi_test_start_callback(&app_factory_test_rssi_test_start_cb);
    ty_uart_cmd_server_for_factory_test.set_reset_callback(&app_factory_test_reset_cb);
}


/**
* @description: factory test command handle
* @param[in] {cmd} command
* @param[in] {data} data
* @param[in] {len} data length
* @return: none
**/
int app_factory_test_cmd(u8 cmd, u8 *data, u8 len){
    ty_uart_cmd_server_for_factory_test.receive_cmd(cmd,data,len);
}


/**
* @description: factory test run in a loop
* @param[in] none
* @return: none
**/
int app_factory_test_run(void){
    
}


/**
* @description: query if enter factory test
* @param[in] none
* @return: if_enter_factory_test: 1->yes, 0->no
**/
int app_factory_test_if_enter(void){
    return if_enter_factory_test;
}

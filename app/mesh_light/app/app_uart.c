/*************************************************************************
	> File Name: app_uart.c
	> Author: 
	> Mail: 
	> Created Time: Tue 26 Feb 2019 22:29:34 CST
 ************************************************************************/
#include "app_uart.h"
#include "ty_fifo.h"
#include "ty_nv.h"
#include "ty_timer_event.h"
#include "base_oem_config.h"
#include "hal_uart.h"
#include "tuya_sigmesh_hal.h"

#include "app_light_uart.h"
#include "app_light_cmd.h"
#include "app_common.h"


#define     F_HEAD1                     0x00
#define     F_HEAD2                     0x01
#define     F_HEAD3                     0x02
#define     F_CMD                       0x03
#define     F_LEN1                      0x04
#define     UART_F_LEN2                 0x06
#define     F_LEN2                      0x05
#define     F_DATA                      0x06

#define     UART_F_MIN_LEN              0x08
#define     F_MIN_LEN                   0x07
#define     F_MAX_LEN                   0xFF

static u8 uart_init_flg = 0;

ty_uart_cmd_server_params_s ty_uart_cmd_server_params = {
};


static void app_uart_server_run(void);


/**
* @description: Additive data for check
* @param {u8 *data} data
* @param {u8 len} length
* @return: sum -> 8 bits reserved
**/
inline static u8 app_uart_check_sum(u8 *data,u8 len){
    u8 i,sum = 0;
    for(i=0;i<len;i++){
        sum+=data[i];
    }
    return sum;
}


////////////////////////////////////////////////////////////////////////////////////
//app_uart
////////////////////////////////////////////////////////////////////////////////////
//uint8_t get_if_uart_init(void){
//    return uart_init_flg;
//}

/**
* @description: uart not init
* @param {none}
* @return: none
**/
void app_uart_deinit(void){
//    uart_init_flg = 0;
}

/**
* @description: uart init
* @param {none}
* @return: none
**/
void app_uart_init(void){
//    uart_init_flg = 1;

    hal_flash_init();
#if CONFIG_LOG_ON
    #if (TY_BT_MODULE == TYBT8C)
        hal_uart_init(BAUD_RATE_115200, GPIO_UART1_TX, GPIO_UART1_RX);
    #else
        hal_uart_init(BAUD_RATE_115200, GPIO_UART0_TX, GPIO_UART0_RX);
    #endif
#else
    #if (TY_BT_MODULE == TYBT8C)
        hal_uart_init(BAUD_RATE_9600, GPIO_UART1_TX, GPIO_UART1_RX);
    #else
        hal_uart_init(BAUD_RATE_9600, GPIO_UART0_TX, GPIO_UART0_RX);
    #endif
#endif
    ty_fifo_init();

    app_factory_test_init();

#if LIGHT_CFG_UART_ENABLE
    // //初始化MESH 灯串口对接
    ty_uart_cmd_server.init(&ty_uart_cmd_server_params);
    ty_uart_cmd_server.set_uart_ctl_callback(&app_mesh_uart_ctl);

    ty_light_uart_ctrl_for_mcu_uart_common.get_ctrl_data_switch_state_callback(&app_light_ctrl_data_switch_get);
    ty_light_uart_ctrl_for_mcu_uart_common.get_ctrl_data_mode_state_callback(&app_light_ctrl_data_mode_get_value);
    ty_light_uart_ctrl_for_mcu_uart_common.get_ctrl_data_colororigin_hsv_callback(&app_light_ctrl_data_hsv_get);
    ty_light_uart_ctrl_for_mcu_uart_common.get_ctrl_data_colororigin_bright_callback(&app_light_ctrl_data_bright_get);
    ty_light_uart_ctrl_for_mcu_uart_common.get_ctrl_data_colororigin_temper_callback(&app_light_ctrl_data_temperature_get);
    ty_light_uart_ctrl_for_mcu_uart_common.get_uart_reply_addr_callback(&tuya_get_uart_reply_addr);
    ty_light_uart_ctrl_for_mcu_uart_common.get_ctrl_data_countdown_data_callback(&app_tuya_vendor_light_countdown_data);


#endif
     
}

/**
* @description: uart run in a loop, handle uart data
* @param {none}
* @return: none
**/
void app_uart_run(void){
    u8 buf[254];
    int len = hal_uart_read(buf,254);

    if(len > 0){
        ty_fifo_add(buf,len);
        PR_DEBUG("%-30s","RECEIVE RAW DATA:");
        for(u8 i=0;i<len;i++){
            PR_DEBUG_RAW("%02X ",buf[i]&0xFF); 
        }
        PR_DEBUG_RAW("\n");
    }

    app_factory_test_run();
#if LIGHT_CFG_UART_ENABLE
    static u8 work_state = 0;
    //灯串口上报在离网状态
    if(work_state != is_provision_success()){//mesh状态改变
        work_state = is_provision_success();
        app_mesh_uart_write(CMD_TY_UART_FEA_SERVER_MESH_STA,&work_state,1); 
    }
#endif
    app_uart_server_run();//解析FIFO中的数据，并调用相应的处理函数
}


/**
* @description: run in a loop, handle uart data
* @param {none}
* @return: none
**/
static void app_uart_server_run(void){
    static u8 is_factory = 1;
    u8 i,num = ty_fifo_get_size();
    if(num < F_MIN_LEN)return;
    
    //1.judge head
    u8 buf[256];

    ty_fifo_read(buf,F_MIN_LEN);//get head length datas,judge if it's head
    if(((buf[F_HEAD1] != 0x66) || (buf[F_HEAD2] != 0xaa) || (buf[F_HEAD3] != 0x00)) &&//factory 
            ((buf[F_HEAD1] != 0xa5) || (buf[F_HEAD2] != 0x5a) || (buf[F_HEAD3] != 0x00))){//mesh light
        ty_fifo_pop(1);
        return;
    }


    u8 head = buf[F_HEAD1];
    u8 cmd = buf[F_CMD];
    u8 len = 0, total_len = 0;
    if(head == 0xa5){                    //mesh light
        len = buf[UART_F_LEN2];
        total_len = len + UART_F_MIN_LEN;
    }else if(head == 0x66){             //2.judge if it's a whole frame
        len = buf[F_LEN2];
        total_len = len + F_MIN_LEN;

        if(len > F_MAX_LEN){                    //error frame,the max frame data len is pid+version=39
            ty_fifo_pop(3);
            return;
        }
    }
    if(num < total_len)return;

    //3.judge check sum 
    ty_fifo_read(buf,total_len);//read all frame data
    u8 ck_sum = app_uart_check_sum(buf,total_len-1);
        
    if(ck_sum == buf[total_len-1]){
        //LOG-FOR-DEBUG
        PR_DEBUG("%-30s","RECEIVE RAW DATA:");
        for(i=0;i<total_len;i++){
            PR_DEBUG_RAW("%02X ",buf[i]&0xFF); 
        }
        PR_DEBUG_RAW("\n");

        ty_fifo_pop(total_len);//correct frame,pop this frame length data
        if(head == 0x66){
            if(get_if_factory_test_close()){
                return;
            }
            app_factory_test_cmd(cmd,&buf[F_DATA],len);
        }else if(head == 0xa5){
            
            ty_uart_cmd_server.receive_cmd(cmd,buf[0x04],&buf[0x07],len);
        }
    }else{//check sum not correct,pop the head length data
        ty_fifo_pop(3);
        return;     
    }
}

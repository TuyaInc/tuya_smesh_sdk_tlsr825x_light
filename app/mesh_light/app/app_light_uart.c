/*************************************************************************
	> File Name: app_uart.c
	> Author: 
	> Mail: 
	> Created Time: Tue 26 Feb 2019 22:29:34 CST
 ************************************************************************/
#include "app_light_uart.h"
#include "tuya_sigmesh_hal.h"
#include "app_common.h"
#include "base_oem_config.h"
#include "tuya_light_model.h"
#include "ty_light_save_user_flash.h"
#include "app_light_cmd.h"

/**
* @description: MCU controls the lamp through uart
* @param {u8 fur} feature
* @param {u8 *params} input data
* @param {u8 len} input data length
* @return: none
**/
void app_mesh_uart_ctl(u8 fur, u8 *params, u8 len){
    switch(fur){
        case CMD_TY_UART_FEA_SERVER_LIGHT_CONTROL:{ //灯功能:开关,模式,亮度,冷暖,彩光,音乐律动
                if(params[0]&0x01)
                {
                    app_light_ctrl_data_switch_set(params[1]);
                    app_light_ctrl_data_countdown_set(0);
                }
                if(params[0]&0x02)
                {
                    app_light_ctrl_data_mode_set(params[2]);
                }
                if((params[0]&0x04)||(params[0]&0x08)||(params[0]&0x10))
                {
                    u16 tuya_H,tuya_S,tuya_V;
                    // COLOR_RGB_T       ColorData;
                    // COLOR_ORIGIN_T    ColorOrigin;

                    if(params[0]&0x04)
                    {
                        tuya_H = params[3];
                        tuya_H = (tuya_H<<8) + params[4];
                        // ColorOrigin.usHue = tuya_H;
                    }
                    if(params[0]&0x08)
                    {
                        tuya_S = params[5];
                        tuya_S = (tuya_S<<8) + params[6];
                        // ColorOrigin.usSat = tuya_S;
                    }
                    if(params[0]&0x10)
                    {
                        tuya_V = params[7];
                        tuya_V = (tuya_V<<8) + params[8];
                        // ColorOrigin.usValue = tuya_V;

                        u16 usLimitMin = 10;
                        u16 usLimitMax = 1000;
                        
                        usLimitMax = 1000 * ( (float) ty_light_json_config_get_colormax() / 100.0 );
                        usLimitMin = 1000 * ( (float) ty_light_json_config_get_colormin() / 100.0 );

                        tuya_V = ( tuya_V - 10 ) * ( usLimitMax - usLimitMin ) / ( 1000 - 10 ) + usLimitMin;
                    }
                    app_light_ctrl_data_hsv_set(tuya_H, tuya_S, tuya_V);
                }
                if(params[0]&0x20)
                {
                    u16 tuya_B = params[9];
                    tuya_B = (tuya_B<<8) + params[10];
                    app_light_ctrl_data_bright_set(tuya_B);
                }
                if(params[0]&0x40)
                {
                    u16 tuya_T = params[11];
                    tuya_T = (tuya_T<<8) + params[12];
                    app_light_ctrl_data_temperature_set(tuya_T);
                }
                app_light_ctrl_data_auto_save_start(5000);
                app_light_ctrl_proc();
            }
            break;
        case CMD_TY_UART_FEA_SERVER_COUNTDOWN:{  //倒计时 
                u32 Countdown_time=0;
                Countdown_time = params[0];
                Countdown_time = (Countdown_time<<8) + params[1];
                Countdown_time = (Countdown_time<<8) + params[2];
                Countdown_time = (Countdown_time<<8) + params[3];
                app_light_ctrl_data_countdown_set(Countdown_time);
            }
            break;
        case CMD_TY_UART_FEA_SERVER_RESET_BT:{  //重置 BT 
               if(0x01 == params[0])
               {
                   kick_out();
               }
            }
            break;
        case CMD_TY_UART_FEA_SERVER_COMPLETE_SCENE:
        case CMD_TY_UART_FEA_SERVER_SCENE_ID:{  //场景号控制 
                if(SCENE_MODE!=app_light_ctrl_data_mode_get_value()){
                    app_light_ctrl_data_mode_set(SCENE_MODE);
                }

                u8 par[LIGHT_SCENE_MAX_LENGTH] = {0};
                OPERATE_LIGHT opRet = 1;
                if(CMD_TY_UART_FEA_SERVER_SCENE_ID == fur){
                  par[3]= 2;
                  par[4]= params[0] << 4;
                  opRet = 0;
                }
                else{
                    u32 scene_len = 0;
                    u8 par_temp[LIGHT_SCENE_MAX_LENGTH] = {0};
                    memcpy(par_temp, &params[1], len-1);
                    opRet = ty_light_basis_tools_scene_data_compress(par_temp, &par[4], &scene_len);
                    par[3]= (u8)scene_len;                       
                }
                             
                if(opRet == 0)
                {
                    u8 temp_flg = ty_uart_cmd_server_get_txtomcu_flag();
                    ty_uart_cmd_server_set_txtomcu_flag(0);//MCU控制场景，不需要交互下发
                    ty_light_scene_cmd_data_set(&par[4],par[3]);
                    ty_uart_cmd_server_set_txtomcu_flag(temp_flg);//还原之前的交互方式
                    app_light_ctrl_data_auto_save_start(APP_DATA_AUTO_SAVE_DELAY_TIME); //APP_DATA_AUTO_SAVE_DELAY_TIME 5000    
                }           
            }
            break;            
        default:break;
    }        
}



/**
* @description: The control information of the lamp is sent to MCU,
             whether it is sent to MCU is decided by ty_uart_cmd_server_get_txtomcu_flag()
* @param {uint32_t opcode} feature
* @param {u8 *params} input data
* @param {u8 len} input data length
* @return: 0
**/
void app_mesh_data_to_uart_send(uint32_t opcode, uint8_t *data,uint16_t data_len){      
    if(ty_uart_cmd_server_get_txtomcu_flag()){
        switch(opcode){
            case TUYA_G_ONOFF_SET:
            case TUYA_G_ONOFF_SET_NOACK:{
                    mesh_cmd_g_onoff_set_t *dev_msg_onoff = (mesh_cmd_g_onoff_set_t *)data;
                    u8 params[13]={0};
                    u8 params_len = 13;
                    params[0] = 0x01;
                    params[1] = dev_msg_onoff->onoff;
                    app_mesh_uart_write(CMD_TY_UART_FEA_SERVER_LIGHT_CONTROL, params, params_len);
                }
                break;
            case TUYA_LIGHTNESS_SET:
            case TUYA_LIGHTNESS_SET_NOACK:{
                    ty_mesh_cmd_lightness_set_t *dev_msg_lightness = (ty_mesh_cmd_lightness_set_t *)data;
                    if((dev_msg_lightness->lightness)<656)
                    {
                        dev_msg_lightness->lightness = 656;
                    }
                    u16 tuya_B = (u16)(((float)(dev_msg_lightness->lightness))/65535.0f*1000);
                    u8 params[13]={0};
                    u8 params_len = 13;
                    params[0] = 0x20;
                    params[9] = (tuya_B>>8)&0xFF;
                    params[10] = tuya_B&0xFF;
                    app_mesh_uart_write(CMD_TY_UART_FEA_SERVER_LIGHT_CONTROL, params, params_len);
                }
                break;
            case TUYA_LIGHT_CTL_TEMP_SET:
            case TUYA_LIGHT_CTL_TEMP_SET_NOACK:{
                    ty_mesh_cmd_light_ctl_temp_set_t *dev_msg_ctl = (ty_mesh_cmd_light_ctl_temp_set_t *)data;        
                    u16 tuya_T = (u16)(((float)(dev_msg_ctl->temp-800))/19200.0f*1000);
                    u8 params[13]={0};
                    u8 params_len = 13;
                    params[0] = 0x40;
                    params[11] = (tuya_T>>8)&0xFF;
                    params[12] = tuya_T&0xFF;
                    app_mesh_uart_write(CMD_TY_UART_FEA_SERVER_LIGHT_CONTROL, params, params_len);
                }
                break;
            case TUYA_LIGHT_HSL_SET:
            case TUYA_LIGHT_HSL_SET_NOACK:{
                    ty_mesh_cmd_light_hsl_set_t *dev_msg_hsl = (ty_mesh_cmd_light_hsl_set_t *)data;
                    //whether this function is available
                    if((dev_msg_hsl->lightness)<330)
                    {
                        dev_msg_hsl->lightness = 330;
                    }           
                    // opRet = app_light_ctrl_data_hsl_set(dev_msg_hsl->hue, dev_msg_hsl->sat, dev_msg_hsl->lightness);
                    u16 h,s,v;
                    ty_light_basis_tools_hsl2hsv(dev_msg_hsl->hue, dev_msg_hsl->sat, dev_msg_hsl->lightness, &h, &s, &v);
                    u8 params[13]={0};
                    u8 params_len = 13;
                    params[0] = 0x1C;
                    params[3] = (h>>8)&0xFF;
                    params[4] = h&0xFF;
                    params[5] = (s>>8)&0xFF;
                    params[6] = s&0xFF;
                    params[7] = (v>>8)&0xFF;
                    params[8] = v&0xFF;
                    app_mesh_uart_write(0x10, params, params_len);//CMD_TY_UART_FEA_SERVER_LIGHT_CONTROL 0x10
                }
                break;
    // vendor model cmd-----------------
            case TUYA_VD_TUYA_WTITE:
            case TUYA_VD_TUYA_WRITE_NOACK:{
                    if(data_len < 2){
                        u8 params[13]={0};
                        u8 params_len = 13;
                        params[0] = 0x02;
                        params[2] = *data;
                        app_mesh_uart_write(CMD_TY_UART_FEA_SERVER_LIGHT_CONTROL, params, params_len);
                    }
                    else{
                        if(data[0] == 1){
                            switch(data[1]){
                                case VD_CMD_SCENE_DATA:
                                case VD_CMD_MUSIC_DATA:
                                case VD_CMD_POWER_MEMORY:
                                case VD_CMD_DONT_DISTURB:
                                    break;
                                case VD_CMD_COUNTDOWN:{
                                        u8 params[4];
                                        u8 params_len = 4;
                                        params[0] = data[3];
                                        params[1] = data[4];
                                        params[2] = data[5];
                                        params[3] = data[6];
                                        app_mesh_uart_write(CMD_TY_UART_FEA_SERVER_COUNTDOWN, params, params_len); 
                                    }
                                    break;
                                default:
                                        app_mesh_uart_write(CMD_TY_UART_FEA_SERVER_TRANS_DP, &data[1], data_len-1);
                                    break;
                            }
                        }  
                    } 
                }
            break;       
            default:
            break;
        }
    }
}














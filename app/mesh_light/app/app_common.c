/*************************************************************************
	> File Name: app_common.c
	> Author: 
	> Mail: 
	> Created Time: Tue 26 Feb 2019 22:29:34 CST
 ************************************************************************/

#include "app_common.h"
#include "hal_log.h"
#include "tuya_sigmesh_hal.h"
#include "base_oem_config.h"
#include "ty_timer_event.h"

#include "tuya_light_model.h"

#include "app_light_cmd.h"
#include "ty_light_json_config.h"
#include "ty_light_cfg_init.h"
#include "app_light_control.h"
#include "ty_light_save_user_flash.h"
#include "ty_light_mdev_std.h"
#include "app_light_prompt.h"
#include "ty_light_basis_tools.h"
#include "ty_light_remote.h"
#include "ty_light_scene.h"

#include "ty_light_uart_ctrl.h"


u16 inquire_src = 0;
u16 inquire_dst = 0;

/**
* @description: response light state after control in on-off model 
* @param[in] {IN bool Onoff} control command: 1->on,0->off
* @return: none
**/
void app_light_ctrl_data_onoff_response(IN bool Onoff)
{
    ty_mesh_cmd_g_onoff_st_t up_msg_onoff;
    up_msg_onoff.present = Onoff;
    up_msg_onoff.target = up_msg_onoff.present;
    tuya_mesh_data_send(get_primary_ele_addr(), TUYA_PUB_ADDR, TUYA_G_ONOFF_STATUS, &up_msg_onoff, sizeof(ty_mesh_cmd_g_onoff_st_t), 0, 0);
}


/**
* @description: light data of vendor model send without ACK
* @param[in] {IN u16 DstAddr} destination address
* @param[in] {IN u8 VendorData} data of vendor model
* @param[in] {IN u8 DataLen} data length
* @return: none 
**/
void app_light_vendor_data_publish(IN u16 DstAddr,IN u8 *VendorData,IN u8 DataLen)
{
    tuya_mesh_data_send(get_primary_ele_addr(), DstAddr, TUYA_VD_TUYA_WRITE_NOACK, VendorData, DataLen, 0, 0);
}

void app_light_vendor_data_send(IN u16 DstAddr,IN u8 *VendorData,IN u8 DataLen)
{
    tuya_mesh_data_send(get_primary_ele_addr(), DstAddr, TUYA_VD_TUYA_DATA, VendorData, DataLen, 0, 0);
}

static int app_light_group_addr_inquire_callback(void){
    u8 buf[4];
	buf[0]=VD_GROUP_ADDR_INQUIRE;
	buf[1]=0x02;
	buf[2]=inquire_dst>>8;
	buf[3]=inquire_dst;
	APP_LOG("!!!ty_light_remote_group_addr_inquire_callback\n");
    tuya_mesh_data_send(get_primary_ele_addr(), inquire_src, TUYA_VD_TUYA_DATA, buf, 4, 0, 0);

    return -1;
}

/**
 * @brief: add inquire vendor remote group address to timer
 * @param {none}
 * @return: none
 */
static void app_light_vendor_group_addr_inquire(uint16_t src_addr, uint16_t dst_addr){
	u8 dealy;
	dealy = get_primary_ele_addr()%100;
    inquire_src = src_addr;
    inquire_dst = dst_addr;
    ty_timer_event_add(app_light_group_addr_inquire_callback,dealy*30*1000);//window 3s / 100device 30ms
}

int mesh_app_close_config_network_callback(){
    static u8 cnt = 1;
    TY_LOG_DEBUG("mesh_app_close_config_network_callback:%d\r\n",cnt);

    if(1 == get_if_prov_success()){
        return -1;
    }

    if(cnt >= 10){
        tuya_gatt_adv_beacon_enable(0);
        return -1;
    }
    else{
        cnt++;
        return 0;
   }
}

void mesh_app_close_config_network_init(void){

    if(0 == get_if_prov_success()){
        ty_timer_event_add(mesh_app_close_config_network_callback, 60*1000*1000);
    }
}

/**
* @description: mesh oem config init, and light params and light ctrldata init,
                and enable timer for light ctrl gradual 
* @param[in] none
* @return: none 
**/
void mesh_oem_config_init(void ){
    APP_LOG("%s\r\n",__FUNCTION__);
    OPERATE_LIGHT opRet = 1;
#if (JSON_CONFIG_ON == 1)
    /* get oem get json!! */
    /* if read failure,return */
    opRet = ty_light_json_config_load();
    if(opRet != LIGHT_OK) {
        TY_LOG_NOTICE("oem cfg load error!");
    }
#else 
    opRet = ty_light_json_config_data_set(strlen(DEFAULT_CONFIG), DEFAULT_CONFIG);
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("Default oem config error!");
    }
#endif
    
    opRet = ty_light_cfg_init_light_params_init();
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("Light params cfg init error!");
    }

    opRet = app_ty_light_ctrldata_init();
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("Light cfg init error!");
    }

    app_light_ctrl_gradual_enable();
}


/**
* @description: application init, perform a related operation on the light.
                you can add some other application init at there.
* @param[in] none
* @return: none 
**/
void mesh_app_init(void){
    APP_LOG("%s\r\n",__FUNCTION__);

    tuya_mesh_data_recv_cb_init(&tuya_mesh_data_recv_callback);

    mesh_app_close_config_network_init();
    
    #if LIGHT_CFG_REMOTE_ENABLE
    ty_light_remote_init();
    #endif

    OPERATE_LIGHT opRet = 1;

    TY_LOG_DEBUG("tuya_hal_get_power_cnt %d", tuya_hal_get_power_cnt());

    //Perform a related operation on the light
    if(TRUE == app_light_ctrl_data_switch_get()){
        if(1 == get_if_prov_success()){
            opRet = app_light_ctrl_proc();
            if(opRet != LIGHT_OK) {
                TY_LOG_ERR("Pre device init set RGBCW error!");
                return;
            }
            app_light_ctrl_data_onoff_response(1);
        }
    }else{
        if((FALSE == app_light_ctrl_data_not_disturb_get())||(tuya_hal_get_power_cnt()>1)){
            if(1 == get_if_prov_success()){
                app_light_ctrl_data_switch_set(TRUE);
                app_light_ctrl_data_auto_save_start(APP_DATA_AUTO_SAVE_DELAY_TIME);
                opRet = app_light_ctrl_proc();
                if(opRet != LIGHT_OK) {
                    TY_LOG_ERR("Pre device init set RGBCW error!");
                    return;
                }
                app_light_ctrl_data_onoff_response(1);
            }else{
                app_light_ctrl_data_switch_set(TRUE);
            }
        }else{
            if(0 == get_if_prov_success()){
                app_light_ctrl_data_switch_set(TRUE);
            }
        }
    }
	//产品测试初始化
    opRet = ty_light_mdev_light_prod_init();  /* prodution init */
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("Prod init error!");
        return;
    }
}

/**
* @description: main application run, you can do something need loop at there
* @param[in] none
* @return: none 
**/
void mesh_main_run(void){
    app_light_ctrl_loop();
}


/**
* @description: Restore initialization settings
* @param[in] none
* @return: none 
**/
void mesh_factory_reset(void){
    APP_LOG("%s\r\n",__FUNCTION__);
#if (LIGHT_CFG_3IN1_SAVE == 1)
    ty_light_save_user_flash_3in1_erase_all();
#else
    ty_light_save_user_flash_erase_all();
#endif
    ty_light_cfg_init_ctrl_data_reset();

	ty_light_save_user_flash_offset_init();

    app_light_ctrl_data_auto_save(TYPE_APP_DATA);
	
}


/**
* @description: tuya mdev test
* @param[in] {uint8_t is_Authorized} if Authorized by tuya
* @param[in] {uint8_t rssi} 
* @return: none 
**/
void app_tuya_mdev_test(uint8_t is_Authorized, uint8_t rssi){
    APP_LOG("%s\r\n",__FUNCTION__);
    tuya_gatt_adv_beacon_enable(0); // when mdev test close prov, if do not need can delete this callback
    
    OPERATE_LIGHT opRet = 1;
	ty_light_mdev_prod_test_callback(is_Authorized,rssi);
}


/**
* @description: mesh state change callback
* @param[in] {mesh_state_t stat} mesh state:
                   typedef enum{
                    }mesh_state_t;
* @return: none 
**/

void mesh_state_callback(mesh_state_t stat){
    OPERATE_LIGHT opRet = 1;
    static char LastMeshStat = 0xFF;
    TY_LOG_NOTICE("last mesh stat:%d, mesh stat %d",LastMeshStat,stat);
    if(LastMeshStat != stat) {
        switch(stat) {
            case NODE_POWER_ON_UNPROVISION:
                    app_light_ctrl_prompt_start();
                break;
            case NODE_POWER_ON_IN_MESH:
                break;
            case NODE_PROVISION_SUCCESS:
                    app_light_ctrl_prompt_stop();
                    ty_light_join_network_success();
                break;
            case NODE_KICK_OUT:
                    app_light_ctrl_prompt_start();
                break;
            case NODE_MESH_RESET:
                    app_light_ctrl_prompt_start();
                break;
            case NODE_RECOVER_IN_MESH:
                break;
            case TY_OTA_START:
                app_entry_tuya_ota_mode();
                break;
            case TY_OTA_SUCCESS:
            case TY_OTA_FAIL:
                app_show_tuya_ota_result(1);
                break;
            case TY_GROUP_SUB_ADD:
                break;
            case TY_GROUP_SUB_DEL:
                break;
            default:
                break;
        }
        LastMeshStat = stat;
    }
}

/*--------------------------------
opcode  
--------------------------------*/

/**
* @description: tuya vendor light dp-data handle
* @param[in] {u8 *par} data
* @param[in] {int par_len} data length
* @return: none 
**/
void app_tuya_vendor_light_dp_data(u8 *par, int par_len){
    switch(par[1]){
        case VD_CMD_SCENE_DATA:{
                if(SCENE_MODE!=app_light_ctrl_data_mode_get_value()){
                    app_light_ctrl_data_mode_set(SCENE_MODE);
                }
                ty_light_scene_cmd_data_set(&par[4],par[3]);
                app_light_ctrl_data_auto_save_start(APP_DATA_AUTO_SAVE_DELAY_TIME);
            }
            break;
        case VD_CMD_COUNTDOWN:{
                u32 Countdown_time=0;
                Countdown_time = par[3];
                Countdown_time = (Countdown_time<<8) + par[4];
                Countdown_time = (Countdown_time<<8) + par[5];
                Countdown_time = (Countdown_time<<8) + par[6];
                app_light_ctrl_data_countdown_set(Countdown_time);              
        }break;
        case VD_CMD_MUSIC_DATA:{
                char music_buffer[22] = {0};
                u32 len = par[3];
                if(len!= 4)
                {
                    break;
                }

                OPERATE_LIGHT opRet = 1;
                
                opRet = ty_light_basis_tools_music_data_decompress(&par[4], len, music_buffer);
                if(LIGHT_OK != opRet)
                {
                    break;
                }
                opRet = app_light_ctrl_data_realtime_adjust_set(TRUE,music_buffer);
                if(LIGHT_OK != opRet)
                {
                    break;
                }
                app_light_real_time_ctrl_proc();
            }
            break;
        case VD_CMD_POWER_MEMORY:{//power_memory
                u32 len = par[3];
                tLightMemCfgData.usFlag = 0x01;
                tLightMemCfgData.usMemory = par[5];
                if(USER_MODE == tLightMemCfgData.usMemory)
                {
                    tLightMemCfgData.usHue = ((u16)par[6]<<8) + par[7];
                    tLightMemCfgData.usSat = ((u16)par[8]<<8) + par[9];
                    tLightMemCfgData.usValue = ((u16)par[10]<<8) + par[11];
                    tLightMemCfgData.usBright = ((u16)par[12]<<8) + par[13];
                    tLightMemCfgData.usTemper = ((u16)par[14]<<8) + par[15];
                }
                app_light_ctrl_data_auto_save_start(APP_DATA_AUTO_SAVE_DELAY_TIME);
            }
            break;
        case VD_CMD_DONT_DISTURB:{//do_not_disturb  value = int
                app_light_ctrl_data_not_disturb_set(par[3]);
                app_light_ctrl_data_auto_save_start(APP_DATA_AUTO_SAVE_DELAY_TIME);
            }
            break;
        default:
            break;
    }
}


/**
* @description: tuya vendor light set-data handle
* @param[in] {u8 *par} data
* @param[in] {int par_len} data length
* @return: none 
**/
void app_tuya_vendor_set_light_data(uint16_t src_addr, uint16_t dst_addr, u8 *par, int par_len){
    if(par_len < 2){
        OPERATE_LIGHT ret = 1;
        if(*par != app_light_ctrl_data_mode_get_value()){ //if Mode switching
            ret = app_light_ctrl_data_mode_set(*par);
            if(LIGHT_OK == ret){
                app_light_ctrl_data_auto_save_start(APP_DATA_AUTO_SAVE_DELAY_TIME);
                app_light_ctrl_proc();
            }
        }
    }
    else{
        switch(par[0]){
            case 1:{
                app_tuya_vendor_light_dp_data(par, par_len);
            }
            break;
            #if LIGHT_CFG_REMOTE_ENABLE
            case VD_GROUP_ADDR_SUB:
            case VD_REMOTE_LIGHTNESS:
            case VD_FAVORITE_SAVE_CMD:{
                ty_light_remote_dp_data(par,par_len);
            }
            break;
            #endif
            case VD_GROUP_ADDR_INQUIRE:{
                app_light_vendor_group_addr_inquire(src_addr, dst_addr);
            }
            break;
            case VD_SCENE_SYNC_CMD:{
                u16 sync_group_addr = ((u16)par[2]<<8) + par[3];

                if((SCENE_MODE != app_light_ctrl_data_mode_get_value())||(ty_light_scene_cmd_get_groupid() != sync_group_addr)||(par[4] != ty_light_scene_ctrl_get_scenenum())||\
                        (par[5] != ty_light_scene_ctrl_get_unit())){
                    break;
                }
                ty_light_scene_cmd_sync(par[6]);
            }
            break;
            default:
            break;
        }
    }
}


/**
* @description: get uart reply addr
* @param[in] { none}
* @return:reply addr 
**/
u16 uart_reply_addr = 0;//use for the function of app_mesh_uart_upload
u16 tuya_get_uart_reply_addr(void)
{
    return uart_reply_addr;
}
/**
* @description: tuya mesh recived-data callback
* @param[in] {uint16_t src_addr} source address
* @param[in] {uint16_t dst_addr} destination address
* @param[in] {uint32_t opcode} option code
* @param[in] {uint8_t *data} data 
* @param[in] {uint16_t data_len} data length
* @param[in] {uint8_t ttl} transmit times
* @return: none 
**/
void tuya_mesh_data_recv_callback(uint16_t src_addr, uint16_t dst_addr, uint32_t opcode, uint8_t *data, uint16_t data_len, uint8_t ttl)
{
    APP_LOG("%s\r\n",__FUNCTION__);
    if(get_primary_ele_addr() == src_addr){
        return;
    }
    uart_reply_addr = src_addr;

    APP_LOG_DUMP("recv data\r\n", data, data_len);

    OPERATE_LIGHT opRet = 1;
    bool bActiveFlag = FALSE;

    switch(opcode){
        case TUYA_G_ONOFF_SET:
        case TUYA_G_ONOFF_SET_NOACK:{
            mesh_cmd_g_onoff_set_t *dev_msg_onoff = (mesh_cmd_g_onoff_set_t *)data;
            APP_LOG("set switch %d",dev_msg_onoff->onoff);

            opRet = app_light_ctrl_data_switch_set(dev_msg_onoff->onoff);
            if(TUYA_G_ONOFF_SET == opcode)
            {
                ty_mesh_cmd_g_onoff_st_t up_msg_onoff;
                up_msg_onoff.present = dev_msg_onoff->onoff;
                up_msg_onoff.target = up_msg_onoff.present;
                tuya_mesh_data_send(dst_addr, src_addr, TUYA_G_ONOFF_STATUS, &up_msg_onoff, sizeof(ty_mesh_cmd_g_onoff_st_t), 0, 1);
            }
            if(LIGHT_OK == opRet) {
                bActiveFlag = TRUE;
                app_light_ctrl_data_countdown_set(0);
            }
            }
            break;
        case TUYA_G_ONOFF_GET:{
            ty_mesh_cmd_g_onoff_st_t up_msg_onoff;
            up_msg_onoff.present = app_light_ctrl_data_switch_get();
            up_msg_onoff.target = up_msg_onoff.present;
            tuya_mesh_data_send(dst_addr, src_addr, TUYA_G_ONOFF_STATUS, &up_msg_onoff, sizeof(ty_mesh_cmd_g_onoff_st_t), 0, 1);
            }
            break;
        case TUYA_LIGHTNESS_SET:
        case TUYA_LIGHTNESS_SET_NOACK:{
            ty_mesh_cmd_lightness_set_t *dev_msg_lightness = (ty_mesh_cmd_lightness_set_t *)data;
            //whether this function is available
            APP_LOG("bright set value %h",dev_msg_lightness->lightness);

            if((dev_msg_lightness->lightness)<656)
            {
                dev_msg_lightness->lightness = 656;
            }

            opRet = app_light_ctrl_data_bright_set((u16)(((float)(dev_msg_lightness->lightness))/65535.0f*1000));
            if(TUYA_LIGHTNESS_SET == opcode)
            {
                ty_mesh_cmd_lightness_st_t up_msg_lightness;
                up_msg_lightness.present = dev_msg_lightness->lightness;
                up_msg_lightness.target = up_msg_lightness.present;
                tuya_mesh_data_send(dst_addr, src_addr, TUYA_LIGHTNESS_STATUS, &up_msg_lightness, sizeof(ty_mesh_cmd_lightness_st_t), 0, 1);
            }

            if(LIGHT_OK == opRet) {
                bActiveFlag = TRUE;
            }
            }
            break;
        case TUYA_LIGHTNESS_GET:{
                ty_mesh_cmd_lightness_st_t up_msg_lightness;
                u16 ty_lightness = app_light_ctrl_data_bright_get();
                up_msg_lightness.present = (u16)(((float)ty_lightness)/1000.0f*65535);
                up_msg_lightness.target = up_msg_lightness.present;
                tuya_mesh_data_send(dst_addr, src_addr, TUYA_LIGHTNESS_STATUS, &up_msg_lightness, sizeof(ty_mesh_cmd_lightness_st_t), 0, 1);
            }
            break;
        case TUYA_LIGHT_CTL_TEMP_SET:
        case TUYA_LIGHT_CTL_TEMP_SET_NOACK:{

            if(WHITE_MODE!=app_light_ctrl_data_mode_get_value()){
                app_light_ctrl_data_mode_set(WHITE_MODE);
            }

            ty_mesh_cmd_light_ctl_temp_set_t *dev_msg_ctl = (ty_mesh_cmd_light_ctl_temp_set_t *)data;
            //whether this function is available
            //APP_DEBUG("temper set value %h",dev_msg_ctl->temp);

            opRet = app_light_ctrl_data_temperature_set((u16)(((float)(dev_msg_ctl->temp-800))/19200.0f*1000));
            if(TUYA_LIGHT_CTL_TEMP_SET == opcode)
            {
                ty_mesh_cmd_light_ctl_temp_st_t up_msg_ctl;
                up_msg_ctl.present_temp = dev_msg_ctl->temp;
                up_msg_ctl.target_temp = up_msg_ctl.present_temp;
                tuya_mesh_data_send(dst_addr, src_addr, TUYA_LIGHT_CTL_TEMP_STATUS, &up_msg_ctl, sizeof(ty_mesh_cmd_light_ctl_temp_st_t), 0, 1);
            }
            if(LIGHT_OK == opRet) {
                bActiveFlag = TRUE;
            }
            }
            break;
        case TUYA_LIGHT_CTL_GET:{
            ty_mesh_cmd_light_ctl_st_t up_msg_ctl;
            u16 tuya_lightness = app_light_ctrl_data_bright_get();
            u16 tuya_temp = app_light_ctrl_data_temperature_get(); 
            up_msg_ctl.present_lightness = (u16)(((float)tuya_lightness)/1000.0f*65535);
            up_msg_ctl.present_temp = (u16)(((float)tuya_temp)/1000.0f*19200)+800;
            up_msg_ctl.target_lightness = up_msg_ctl.present_lightness;
            up_msg_ctl.target_temp = up_msg_ctl.present_temp;
            tuya_mesh_data_send(dst_addr, src_addr, TUYA_LIGHT_CTL_STATUS, &up_msg_ctl, sizeof(ty_mesh_cmd_light_ctl_st_t), 0, 1);
            }
            break;
        case TUYA_LIGHT_CTL_TEMP_GET:{
            ty_mesh_cmd_light_ctl_temp_st_t up_msg_ctl;
            u16 tuya_temp = app_light_ctrl_data_temperature_get(); 
            up_msg_ctl.present_temp = (u16)(((float)tuya_temp)/1000.0f*19200)+800;
            up_msg_ctl.target_temp = up_msg_ctl.present_temp;
            tuya_mesh_data_send(dst_addr, src_addr, TUYA_LIGHT_CTL_TEMP_STATUS, &up_msg_ctl, sizeof(ty_mesh_cmd_light_ctl_temp_st_t), 0, 1);
            }
            break;
        case TUYA_LIGHT_HSL_SET:
        case TUYA_LIGHT_HSL_SET_NOACK:{

            if(COLOR_MODE!=app_light_ctrl_data_mode_get_value()){
                app_light_ctrl_data_mode_set(COLOR_MODE);
            }
            
            ty_mesh_cmd_light_hsl_set_t *dev_msg_hsl = (ty_mesh_cmd_light_hsl_set_t *)data;
            //whether this function is available

            if((dev_msg_hsl->lightness)<330)
            {
                dev_msg_hsl->lightness = 330;
            }
            
            opRet = app_light_ctrl_data_hsl_set(dev_msg_hsl->hue, dev_msg_hsl->sat, dev_msg_hsl->lightness);

            if(TUYA_LIGHT_HSL_SET == opcode)
            {
                ty_mesh_cmd_light_hsl_st_t up_msg_hsl;
                up_msg_hsl.hue = dev_msg_hsl->hue;
                up_msg_hsl.sat = dev_msg_hsl->sat;
                up_msg_hsl.lightness = dev_msg_hsl->lightness;
                tuya_mesh_data_send(dst_addr, src_addr, TUYA_LIGHT_HSL_STATUS, &up_msg_hsl, sizeof(ty_mesh_cmd_light_hsl_st_t), 0, 1);
            }
            if(LIGHT_OK == opRet) {
                bActiveFlag = TRUE;
            }
            }
            break;
        case TUYA_LIGHT_HSL_GET:{
            ty_mesh_cmd_light_hsl_st_t up_msg_hsl;
            app_light_ctrl_data_hsl_get(&up_msg_hsl.hue,&up_msg_hsl.sat,&up_msg_hsl.lightness);
            tuya_mesh_data_send(dst_addr, src_addr, TUYA_LIGHT_HSL_STATUS, &up_msg_hsl, sizeof(ty_mesh_cmd_light_hsl_st_t), 0, 1);
            }
            break;
  // vendor model cmd-----------------
        case TUYA_VD_TUYA_WTITE:
        case TUYA_VD_TUYA_WRITE_NOACK:{

            if((((data[0]==1)&&(data[1]==6))||((data[0]==2)&&(data_len<2))) && ((dst_addr & 0xC000) == 0xC000))//场景命令或者发场景模式 拿到地址
            {
                ty_light_scene_cmd_set_groupid(dst_addr);
            }
            ty_light_scene_cmd_set_ttl(ttl);

            app_tuya_vendor_set_light_data(src_addr, dst_addr,&(*data), data_len);
            if(TUYA_VD_TUYA_WTITE == opcode)
            {
                tuya_mesh_data_send(dst_addr, src_addr, TUYA_VD_TUYA_DATA, &(*data), data_len, 0, 1);
            } 
            }
        break;
        case TUYA_VD_TUYA_READ:{ 
            if(0==data_len)
            {
                u8 data = app_light_ctrl_data_mode_get_value();
                tuya_mesh_data_send(dst_addr, src_addr, TUYA_VD_TUYA_DATA, &data, 1, 0, 1);
            }else{
                switch(data[0]){
                    case 1:{
                        for(u8 i=0;i<data[1];i++)
                        {
                            switch(data[i+2]){
                                case VD_CMD_WORK_MODE:{
                                    u8 lightmode = app_light_ctrl_data_mode_get_value();
                                    tuya_mesh_data_send(dst_addr, src_addr, TUYA_VD_TUYA_DATA, &lightmode, 1, 0, 1);
                                }
                                break;
                                case VD_CMD_SCENE_DATA:{
                                    u8 scene_len=0;
                                    u8 scene_data[38]= {0};
                                    memset(scene_data,0 ,38);
                                    ty_light_scene_cmd_dataget(&scene_data[4],&scene_len);
                                    scene_data[0] = 0x01;
                                    scene_data[1] = 0x06;
                                    scene_data[2] = 0x00;
                                    scene_data[3] = scene_len;
                                    tuya_mesh_data_send(dst_addr, src_addr, TUYA_VD_TUYA_DATA, &scene_data, scene_len+4, 0, 1);
                                }
                                break;
                                case VD_CMD_COUNTDOWN:{
                                    u8 countdown_data[7]= {0};
                                    app_tuya_vendor_light_countdown_data(&countdown_data[3]);
                                    countdown_data[0] = 0x01;
                                    countdown_data[1] = 0x07;
                                    countdown_data[2] = 0x02;
                                    tuya_mesh_data_send(dst_addr, src_addr, TUYA_VD_TUYA_DATA, &countdown_data, sizeof(countdown_data), 0, 1);
                                }
                                break;
                                case VD_CMD_POWER_MEMORY:{
                                    u8 LightMem_data[16]= {0};
                                    app_tuya_vendor_light_lightmem_data(&LightMem_data[3]);
                                    LightMem_data[0] = 0x01;
                                    LightMem_data[1] = 0x21;
                                    LightMem_data[2] = 0x00;//raw
                                    tuya_mesh_data_send(dst_addr, src_addr, TUYA_VD_TUYA_DATA, &LightMem_data, sizeof(LightMem_data), 0, 1);
                                }
                                break;
                                case VD_CMD_DONT_DISTURB:{//do_not_disturb
                                    u8 NotDisturb_data[4]= {0};
                                    NotDisturb_data[0] = 0x01;
                                    NotDisturb_data[1] = 0x22;
                                    NotDisturb_data[2] = 0x01;//value
                                    NotDisturb_data[3] = app_light_ctrl_data_not_disturb_get();
                                    tuya_mesh_data_send(dst_addr, src_addr, TUYA_VD_TUYA_DATA, &NotDisturb_data, sizeof(NotDisturb_data), 0, 1);
                                    }
                                    break;
                                default:
                                break;
                            }
                        }
                    }
                    break;
                    default:
                    break;
                }
            }
        }
        break;
	}
#if LIGHT_CFG_UART_ENABLE 
    app_mesh_data_to_uart_send(opcode, data, data_len);
#endif

    APP_LOG("ctrl data need to proc flag =%d", bActiveFlag);
    if(bActiveFlag) {
        app_light_ctrl_data_auto_save_start(APP_DATA_AUTO_SAVE_DELAY_TIME);
        opRet = app_light_ctrl_proc();
        if(opRet != LIGHT_OK) {
            APP_LOG("ctrl proc deal error!");
        }
    }
}

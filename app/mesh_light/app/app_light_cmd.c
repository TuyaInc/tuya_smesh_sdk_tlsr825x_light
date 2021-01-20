/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: light_control.c
 * @Description: light control process
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-28 14:35:54
 * @LastEditTime: 2019-10-18 10:54:16
 */
#include "app_light_cmd.h"
#include "ty_light_cfg_init.h"
#include "app_light_control.h"
#include "ty_light_basis_sw_timer.h"
#include "ty_light_scene.h"
#include "ty_light_basis_tools.h"
#include "ty_light_save_user_flash.h"

u8 light_not_disturb;
extern LIGHT_MDEV_TEST_DATA_FLASH_T tProdResult;
/**
 * @brief: set light switch data, adapte control data issued by system
 *          to control data format.
 * @param {IN bool bONOFF -> switch data, TRUE will turn on} 
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_switch_set(IN bool bONOFF)
{   
    bool bLastStatus ;

    bLastStatus = tLightCtrlData.bSwitch;

    if( TRUE == bONOFF ) {
        tLightCtrlData.bSwitch = TRUE;
        if(SCENE_MODE == tLightCtrlData.eMode) {              /* restart scene mode */
            ty_light_scene_ctrl_firstset(TRUE);
        }
    } else {
        tLightCtrlData.bSwitch = FALSE;
    }
    
    if(bONOFF == bLastStatus) {
        TY_LOG_DEBUG("the same switch set");
        return LIGHT_INVALID_PARM;
    }
    
    return LIGHT_OK;
}

/**
 * @brief: set light mode data
 * @param {IN LIGHT_MODE_E eMode} 
 * @attention: Mode value is below:
 *                                  WHITE_MODE = 0,
 *                                  COLOR_MODE = 1,
 *                                  SCENE_MODE = 2,
 *                                  MUSIC_MODE = 3,
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_mode_set(IN LIGHT_MODE_E eMode)
{   
	OPERATE_LIGHT opRet = 1;
    LIGHT_MODE_E eLastMode ;

    eLastMode = tLightCtrlData.eMode;

    /* mode set need limit when light way is RGB, or CW or C */
    if( ((LIGHT_RGB == ty_light_cfg_init_get_lightway()) && (WHITE_MODE == eMode)) \
        ||((ty_light_cfg_init_get_lightway() <= LIGHT_CW) && (COLOR_MODE == eMode)) \
        || (eMode > MODE_MAX)) {

        TY_LOG_ERR("mode is illegal,set error");
        return LIGHT_INVALID_PARM;
    }
    
    tLightCtrlData.eMode = eMode;
    
    if((eMode == eLastMode) && (eMode != SCENE_MODE)) {      /* only scene mode can be set many time */
        TY_LOG_DEBUG("the same mode set");
        return LIGHT_INVALID_PARM;
    }

    /* don't need to proc, as the adjusted new scene control data will issued later  */
    if((eMode == SCENE_MODE) && (tLightCtrlData.ucRealTimeFlag == TRUE)) { 
        PR_DEBUG("this scene mode don't need proc!");
        extern void ty_light_scene_auto_restart_time_callback(void);
        opRet = ty_light_basis_sw_timer_start(SCENE_AUTO_RESTART_TIMER, 1500, ty_light_scene_auto_restart_time_callback);
        if( opRet != LIGHT_OK) {
            TY_LOG_ERR("scene auto restart timer init error!");
        }
        return LIGHT_INVALID_PARM;
    }

    if(SCENE_MODE == tLightCtrlData.eMode) {              /* restart scene mode */
        ty_light_scene_ctrl_firstset(TRUE);
    }
    
    return LIGHT_OK;
}

/**
 * @brief: set light bright data, adapte control data issued by system
 *          to control data format.
 * @param {IN u16 usBright} 
 * @attention: acceptable range:10~1000
 * @attention: set bright value, will auto set the Mode to WHITE_MODE !
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_bright_set(IN u16 usBright)
{   
    if(((usBright < CTRL_CW_BRIGHT_VALUE_MIN) || (usBright > CTRL_CW_BRIGHT_VALUE_MAX)) \
        || (LIGHT_RGB == ty_light_cfg_init_get_lightway())){
        TY_LOG_ERR("bright value is exceed range,set error");
        return LIGHT_INVALID_PARM;
    }
    
    tLightCtrlData.usBright = usBright;
    tLightCtrlData.eMode = WHITE_MODE;    /* change mode to white mode forcibly */

    return LIGHT_OK;
}

/**
 * @brief: set light temrperature data, adapte control data issued by system
 *          to control data format.
 * @param {IN u16 usTemperature} 
 * @attention: acceptable range:0~1000
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_temperature_set(IN u16 usTemperature)
{

    if((usTemperature > CTRL_CW_BRIGHT_VALUE_MAX) \
        || (LIGHT_RGB == ty_light_cfg_init_get_lightway())) {
        TY_LOG_ERR("temperature value is exceed range,set error");
        return LIGHT_INVALID_PARM;
    }

    tLightCtrlData.usTemper = usTemperature;
    tLightCtrlData.eMode = WHITE_MODE;    /* change mode to white mode forcibly */

    return LIGHT_OK;
}

/**
 * @brief: set light hsv data
 * @param {IN u16 usHue}  hue, range:0 ~ 360
 * @param {IN u16 usSat}  saturation, range:0 ~ 1000
 * @param {IN u16 usValue} value, range:0 ~ 1000
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_hsv_set(IN u16 usHue,IN u16 usSat,IN u16 usValue)
{   
    //存储的是LightCtrlData
    COLOR_RGB_T       ColorData;
    
    u16 usLimitMin = 10;
    u16 usLimitMax = 1000;
    
    usLimitMax = 1000 * ( (float) ty_light_cfg_init_get_colormax() / 100.0 );
    usLimitMin = 1000 * ( (float) ty_light_cfg_init_get_colormin() / 100.0 );

    u16 tuya_hsv_val = ( usValue - 10 ) * ( usLimitMax - usLimitMin ) / ( 1000 - 10 ) + usLimitMin;

    ty_light_basis_tools_hsv2rgb(usHue, usSat, tuya_hsv_val, &ColorData.usRed, &ColorData.usGreen, &ColorData.usBlue);

    if(ty_light_cfg_init_get_lightway() <= LIGHT_CW) {
        return LIGHT_INVALID_PARM;
    }
    
    /* to avoid the situation:adjust color data ver fast and turn off the light with remote cmd,
    as exist realtime control dp, the ightCtrlData.Color will equal to the control data */
    tLightCtrlData.tColorOrigin.usHue = usHue;
    tLightCtrlData.tColorOrigin.usSat = usSat;
    tLightCtrlData.tColorOrigin.usValue = usValue;

    memcpy(&tLightCtrlData.tColor, &ColorData, sizeof(COLOR_RGB_T));
    
    return LIGHT_OK;
}



/**
 * @brief: set light hsl, transform hsl to hsv for control light
 * @param {IN u16 usHue}  hue, range:0 ~ 360
 * @param {IN u16 usSat}  saturation, range:0 ~ 1000
 * @param {IN u16 usLightness} Lightness, range:0 ~ 1000
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_hsl_set(IN u16 usHue,IN u16 usSat,IN u16 usLightness)
{
    u16 h,s,v;
    ty_light_basis_tools_hsl2hsv(usHue, usSat, usLightness, &h, &s, &v);
    app_light_ctrl_data_hsv_set(h, s, v);
}


/**
 * @brief: get light switch data
 * @param {OUT bool *pONOFF -> switch data return} 
 * @retval: OOPERATE_RET
 */
bool app_light_ctrl_data_switch_get(void)
{
    return tLightCtrlData.bSwitch;
}


/**
 * @berief: get light mode data
 * @param {OUT LIGHT_MODE_E *Mode -> mode data return} 
 *                                    typedef enum{
 *                                        WHITE_MODE = 0,
 *                                        COLOR_MODE,
 *                                        SCENE_MODE,
 *                                        MUSIC_MODE,
 *                                       MODE_MAX,
 *                                    }LIGHT_MODE_E;
 * @retval: OOPERATE_RET
 */
u8 app_light_ctrl_data_mode_get_value(void)
{
    return tLightCtrlData.eMode;
}


/**
 * @berief: get light way data
 * @param :none
 * @retval: u8 ->typedef enum {
                    LIGHT_C = 1,
                    LIGHT_CW,
                    LIGHT_RGB,
                    LIGHT_RGBC,
                    LIGHT_RGBCW,
                    LIGHT_MAX,
                }CTRL_LIGHT_WAY_E;
 */
u8 app_light_cfg_light_way_get_value(void)
{
    return ty_light_cfg_init_get_lightway();
}


/**
 * @brief: geta light bright data
 * @param {OUT u16 *pBright -> bright data return} 
 * @retval: OOPERATE_RET
 */
u16 app_light_ctrl_data_bright_get(void)
{
    return tLightCtrlData.usBright;
}


/**
 * @brief: get light temrperature data
 * @param {OUT u16 *pTemperature -> temperature data return} 
 * @retval: OPERATE_LIGHT
 */
u16 app_light_ctrl_data_temperature_get(void)
{
    return tLightCtrlData.usTemper;
}


/**
 * @brief: get light RGB data & original data
 * @param {OUT COLOR_ORIGIN_T *ptOriginalColor -> color original data return} 
 * @retval: OPERATE_LIGHT
 */
void app_light_ctrl_data_hsl_get(OUT u16 *hue,OUT u16 *sat,OUT u16 *lightness)
{

    ty_light_basis_tools_hsv2hsl(tLightCtrlData.tColorOrigin.usHue, tLightCtrlData.tColorOrigin.usSat, tLightCtrlData.tColorOrigin.usValue, &(*hue), &(*sat), &(*lightness));

}


/**
 * @brief: get light RGB data & original data
 * @param {OUT COLOR_ORIGIN_T *ptOriginalColor -> color original data return} 
 * @retval: OPERATE_LIGHT
 */
void app_light_ctrl_data_hsv_get(OUT u16 *hue,OUT u16 *sat,OUT u16 *value)
{

    *hue = tLightCtrlData.tColorOrigin.usHue;
    *sat = tLightCtrlData.tColorOrigin.usSat;
    *value = tLightCtrlData.tColorOrigin.usValue;
}

/**
 * @brief: light ctrl breath proc
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
static void app_light_ctrl_countdown_timer_callback(void)
{
    OPERATE_LIGHT opRet = 1;
    
    if(tLightCtrlData.uiCountDown > 1) {    /* to avoid ulCountDown = 0 %60 also equal to 0 */
        tLightCtrlData.uiCountDown --;     
    } else {
        tLightCtrlData.uiCountDown = 0; 
        tLightCtrlData.bSwitch = (tLightCtrlData.bSwitch != FALSE) ? FALSE : TRUE;

        opRet = app_light_ctrl_proc();
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("CountDown process error!");
        }

        app_light_ctrl_data_onoff_response(tLightCtrlData.bSwitch);     /* opload after ctrl deal with */
		opRet = app_light_ctrl_data_auto_save(TYPE_APP_DATA);
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("Light ctrl data auto save error!");
        }else{
            TY_LOG_DEBUG("Light ctrl app auto data save OK !");
        }

        opRet = ty_light_basis_sw_timer_stop(COUNTDOWN_SW_TIMER);  /* stop timer */
        if(opRet != LIGHT_OK) {
           TY_LOG_ERR("stop countdown software timer error!");
        }
        return;     

    }

    opRet = ty_light_basis_sw_timer_start(COUNTDOWN_SW_TIMER, 1000, (void*)app_light_ctrl_countdown_timer_callback);
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("CountDown timer restart error!");
    }
    
}

/**
 * @brief: set light countdown value
 * @param {IN INT_T uiCountDownSec -> unit:second} 
 * @attention: countdown lave time will return with 
 *              switch status will return with calling
 *              vLightCtrlDataSwitchRespone function when countdown active.
 * @retval: OPERATE_LIGHT -> LIGHT_OK set countdown OK.
 */
OPERATE_LIGHT app_light_ctrl_data_countdown_set(IN u32 uiCountDownSec)
{
    OPERATE_LIGHT opRet = 1;
    
    if((uiCountDownSec < 0)|| (uiCountDownSec > 86400)){
        TY_LOG_ERR("Set countdwon value error!");
        return LIGHT_INVALID_PARM;
    }

    tLightCtrlData.uiCountDown = uiCountDownSec;
    if(uiCountDownSec <= 0) {
        opRet = ty_light_basis_sw_timer_stop(COUNTDOWN_SW_TIMER);      /* cancel coutdown proc */
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("Stop countdown timer error!");
        }
        return LIGHT_OK;

    } else {
        opRet = ty_light_basis_sw_timer_start(COUNTDOWN_SW_TIMER, 1000, (void*)app_light_ctrl_countdown_timer_callback);
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("Start countdown timer error!");
        }
    }
    return LIGHT_OK;
}

/**
 * @berief: get light countdown time
 * @param {OUT u32 *CountdownTime -> Countdown time return} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_ctrl_data_countdown_get(OUT u32 *CountdownTime)
{
    *CountdownTime = tLightCtrlData.uiCountDown;
}


/**
 * @berief: get light if disturb
 * @param {OUT bool light_not_disturb} 
 * @retval: OPERATE_LIGHT
 */
bool app_light_ctrl_data_not_disturb_get(void)
{
    return light_not_disturb;
}


/**
 * @berief: set light if disturb
 * @param {IN bool bNotDisturb} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_ctrl_data_not_disturb_set(IN bool bNotDisturb)
{
    light_not_disturb = bNotDisturb;
}

/**
 * @brief: set light realtime control data
 * @param {IN BOOL_T bMusicFlag}
 * @param {IN CHAR_T *pRealTimeData}
 * @attention: data format: please reference to DP protocol
 * @retval: OPERATE_LIGHT -> LIGHT_OK need to call app_light_real_time_ctrl_proc function.
 */
OPERATE_LIGHT app_light_ctrl_data_realtime_adjust_set(IN bool bMusicFlag, IN char *pRealTimeData)
{
    if(bMusicFlag) {
        if(tLightCtrlData.eMode != MUSIC_MODE) {
            TY_LOG_DEBUG("Music data don't accpected, bacause mode is not music mode!");
            return LIGHT_INVALID_PARM; 
        }
    }
    
    if(strcmp(pRealTimeData, tLightCtrlData.cRealTimeData) == 0) {
        TY_LOG_DEBUG("the same realtime adjust data");
        return LIGHT_INVALID_PARM;
    }

    if(strlen(pRealTimeData) != 21) {
        TY_LOG_ERR("Real time adjust data is error! please chek!");
        return LIGHT_INVALID_PARM;
    }

    strcpy((char*)&tLightCtrlData.cRealTimeData, pRealTimeData);
    TY_LOG_DEBUG("light real time adjust ctrl data buf %s",tLightCtrlData.cRealTimeData);

    return LIGHT_OK;
}


/**
 * @brief: get light countdown data for vendor
 * @param {OUT u8 *data}
 * @attention: data format: please reference to DP protocol
 * @retval: OPERATE_LIGHT -> LIGHT_OK need to call app_light_real_time_ctrl_proc function.
 */
void app_tuya_vendor_light_countdown_data(u8 *data){

    u32 Countdown_time=0;
    app_light_ctrl_data_countdown_get(&Countdown_time);
    data[0] = (u8)((Countdown_time>>24)&0xFF);
    data[1] = (u8)((Countdown_time>>16)&0xFF);
    data[2] = (u8)((Countdown_time>>8)&0xFF);
    data[3] = (u8)(Countdown_time&0xFF);
}


/**
 * @brief: get light memory data for vendor
 * @param {OUT u8 *data}
 * @attention: data format: please reference to DP protocol
 * @retval: OPERATE_LIGHT -> LIGHT_OK need to call app_light_real_time_ctrl_proc function.
 */
void app_tuya_vendor_light_lightmem_data(u8 *data){

    data[0] = 0x0C;//len
    data[1] = 0x00;
    data[2] = tLightMemCfgData.usMemory;//mode
    data[3] = (u8)((tLightMemCfgData.usHue>>8)&0xFF);//H
    data[4] = (u8)(tLightMemCfgData.usHue&0xFF);//H
    data[5] = (u8)((tLightMemCfgData.usSat>>8)&0xFF);//S
    data[6] = (u8)(tLightMemCfgData.usSat&0xFF);//S
    data[7] = (u8)((tLightMemCfgData.usValue>>8)&0xFF);//V
    data[8] = (u8)(tLightMemCfgData.usValue&0xFF);//V
    data[9] = (u8)((tLightMemCfgData.usBright>>8)&0xFF);//B
    data[10] = (u8)(tLightMemCfgData.usBright&0xFF);//B
    data[11] = (u8)((tLightMemCfgData.usTemper>>8)&0xFF);//T
    data[12] = (u8)(tLightMemCfgData.usTemper&0xFF);//T
}


OPERATE_LIGHT app_light_ctrl_data_auto_save(IN LIGHT_FLASH_SAVE_TYPE_E eDataType)
{
    OPERATE_LIGHT opRet = 1;

	switch(eDataType){
        case TYPE_APP_DATA:{ 
                LIGHT_APP_DATA_FLASH_T tSaveAppData;    
                memset(&tSaveAppData, 0, sizeof(LIGHT_APP_DATA_FLASH_T));
                tSaveAppData.bPower = tLightCtrlData.bSwitch;
                tSaveAppData.eMode = tLightCtrlData.eMode;
                tSaveAppData.usBright = tLightCtrlData.usBright;
                tSaveAppData.usTemper = tLightCtrlData.usTemper;    
                tSaveAppData.tColor.usRed = tLightCtrlData.tColor.usRed;
                tSaveAppData.tColor.usGreen = tLightCtrlData.tColor.usGreen;
                tSaveAppData.tColor.usBlue = tLightCtrlData.tColor.usBlue;
                tSaveAppData.tColorOrigin.usHue = tLightCtrlData.tColorOrigin.usHue;
                tSaveAppData.tColorOrigin.usSat = tLightCtrlData.tColorOrigin.usSat;
                tSaveAppData.tColorOrigin.usValue  = tLightCtrlData.tColorOrigin.usValue;
                strcpy((char*)&tSaveAppData.tColorOrigin.cColorStr, (char*)&tLightCtrlData.tColorOrigin.cColorStr);
            #if (LIGHT_CFG_3IN1_SAVE == 1)
                ty_light_scene_ctrl_get_scenedata(&tSaveAppData.cScene);
            #else
                tSaveAppData.tSceneId = ty_light_scene_ctrl_get_scenenum();
            #endif
            #if (LIGHT_CFG_3IN1_SAVE == 1) 
                ty_light_save_user_flash_open_appdata();
                ty_light_save_user_flash_write_3in1_data(TYPE_APP_DATA,&tSaveAppData);
                ty_light_save_user_flash_close_appdata();
            #else
                ty_light_save_user_flash_write_data(TYPE_APP_DATA,&tSaveAppData);
            #endif
            }
            break;
		case TYPE_MDEV_DATA:{ 
                LIGHT_MDEV_TEST_DATA_FLASH_T tSaveMdevData;
                memset(&tSaveMdevData, 0, sizeof(LIGHT_MDEV_TEST_DATA_FLASH_T));
                tSaveMdevData.eTestMode = tProdResult.eTestMode;
                tSaveMdevData.ucResetCnt = tProdResult.ucResetCnt;
                tSaveMdevData.usAgingTestedTime = tProdResult.usAgingTestedTime;
            #if (LIGHT_CFG_3IN1_SAVE == 1)  
                ty_light_save_user_flash_open_appdata();
                ty_light_save_user_flash_write_3in1_data(TYPE_MDEV_DATA,&tSaveMdevData);
                ty_light_save_user_flash_close_appdata();
            #else
                ty_light_save_user_flash_write_data(TYPE_MDEV_DATA,&tSaveMdevData);
            #endif
            }
            break;
		case TYPE_MEM_DATA:{ 
                LIGHT_MEM_USER_FLASH_T tSaveMemData; 
                memset(&tSaveMemData, 0, sizeof(LIGHT_MEM_USER_FLASH_T));
                tSaveMemData.usFlag = tLightMemCfgData.usFlag;
                tSaveMemData.usNotDisturb = light_not_disturb;
                tSaveMemData.usMemory = tLightMemCfgData.usMemory;
                tSaveMemData.usHue = tLightMemCfgData.usHue;
                tSaveMemData.usSat = tLightMemCfgData.usSat;
                tSaveMemData.usValue = tLightMemCfgData.usValue;
                tSaveMemData.usBright = tLightMemCfgData.usBright;
                tSaveMemData.usTemper = tLightMemCfgData.usTemper;
                tSaveMemData.usSyncGroupAddr = ty_light_scene_cmd_get_groupid();
            #if (LIGHT_CFG_3IN1_SAVE == 1)  
                ty_light_save_user_flash_open_appdata();
                ty_light_save_user_flash_write_3in1_data(TYPE_MEM_DATA,&tSaveMemData); 
                ty_light_save_user_flash_close_appdata();
            #else
                ty_light_save_user_flash_write_data(TYPE_MEM_DATA,&tSaveMemData); 
            #endif       			
            }
            break;
		case TYPE_FAV_FST_DATA:
        case TYPE_FAV_SEC_DATA:
        case TYPE_FAV_THD_DATA:
        case TYPE_FAV_FUR_DATA:{ 
                LIGHT_FAVORITE_DATA_FLASH_T tSaveFavData;
                memset(&tSaveFavData, 0, sizeof(LIGHT_FAVORITE_DATA_FLASH_T));
                tSaveFavData.bPower = tLightCtrlData.bSwitch;
                tSaveFavData.eMode = tLightCtrlData.eMode;
                tSaveFavData.usBright = tLightCtrlData.usBright;
                tSaveFavData.usTemper = tLightCtrlData.usTemper;    
                tSaveFavData.usRed = tLightCtrlData.tColor.usRed;
                tSaveFavData.usGreen = tLightCtrlData.tColor.usGreen;
                tSaveFavData.usBlue  = tLightCtrlData.tColor.usBlue;
                //-------------
                tSaveFavData.sence = ty_light_scene_ctrl_get_scenenum();
                tSaveFavData.usHue = tLightCtrlData.tColorOrigin.usHue;
                tSaveFavData.usSat = tLightCtrlData.tColorOrigin.usSat;
                tSaveFavData.usValue =  tLightCtrlData.tColorOrigin.usValue;
            #if (LIGHT_CFG_3IN1_SAVE == 1) 
                ty_light_save_user_flash_open_appdata();
                ty_light_save_user_flash_write_3in1_data(eDataType,&tSaveFavData);
                ty_light_save_user_flash_close_appdata();
            #else
                ty_light_save_user_flash_write_data(eDataType,&tSaveFavData);
            #endif
            }
            break;
		case TYPE_USER_DATA:{ 
                LIGHT_CUST_DATA_FLASH_T tSaveCustData;
                memset(&tSaveCustData, 0, sizeof(LIGHT_CUST_DATA_FLASH_T));
            #if (LIGHT_CFG_3IN1_SAVE == 1) 
                ty_light_save_user_flash_open_appdata();
                ty_light_save_user_flash_write_3in1_data(TYPE_USER_DATA,&tSaveCustData);
                ty_light_save_user_flash_close_appdata();
            #else
                ty_light_save_user_flash_write_data(TYPE_USER_DATA,&tSaveCustData);
            #endif
            }
		    break;
		default:
		    break;
	}
    return LIGHT_OK;
}

OPERATE_LIGHT app_light_ctrl_data_auto_read(IN LIGHT_FLASH_SAVE_TYPE_E eDataType)
{
    OPERATE_LIGHT opRet = 1;

	switch(eDataType){
		case TYPE_MDEV_DATA:{ 
                LIGHT_MDEV_TEST_DATA_FLASH_T tReadMdevData;
                memset(&tReadMdevData, 0, sizeof(LIGHT_MDEV_TEST_DATA_FLASH_T));
            #if (LIGHT_CFG_3IN1_SAVE == 1)  
                opRet = ty_light_save_user_flash_read_3in1_data(TYPE_MDEV_DATA,&tReadMdevData);
            #else
                opRet = ty_light_save_user_flash_read_data(TYPE_MDEV_DATA,&tReadMdevData);
            #endif
                if( LIGHT_OK != opRet ) {
                    TY_LOG_ERR("Read data error!");
                    return LIGHT_COM_ERROR;
                }
                tProdResult.eTestMode = tReadMdevData.eTestMode;
                tProdResult.ucResetCnt = tReadMdevData.ucResetCnt;
                tProdResult.usAgingTestedTime = tReadMdevData.usAgingTestedTime;
            }
            break;
		case TYPE_FAV_FST_DATA:
        case TYPE_FAV_SEC_DATA:
        case TYPE_FAV_THD_DATA:
        case TYPE_FAV_FUR_DATA:{ 
                LIGHT_FAVORITE_DATA_FLASH_T tReadFavData;
                memset(&tReadFavData, 0, sizeof(LIGHT_FAVORITE_DATA_FLASH_T));
            #if (LIGHT_CFG_3IN1_SAVE == 1) 
                opRet = ty_light_save_user_flash_read_3in1_data(eDataType,&tReadFavData);
            #else
                opRet = ty_light_save_user_flash_read_data(eDataType,&tReadFavData);
            #endif
                if( LIGHT_OK != opRet ) {
                    TY_LOG_ERR("Read data error!");
                    return LIGHT_COM_ERROR;
                }
                if(FALSE == tReadFavData.bPower)
                {
                    tLightCtrlData.bSwitch = tReadFavData.bPower;
                }else{
                    tLightCtrlData.bSwitch = tReadFavData.bPower;
                    tLightCtrlData.eMode = tReadFavData.eMode;
                    switch(tLightCtrlData.eMode)
                    {
                    case WHITE_MODE:
                        tLightCtrlData.usBright = tReadFavData.usBright;
                        tLightCtrlData.usTemper = tReadFavData.usTemper; 
                        break;
                    case COLOR_MODE:
                        tLightCtrlData.tColor.usRed = tReadFavData.usRed;
                        tLightCtrlData.tColor.usGreen = tReadFavData.usGreen;
                        tLightCtrlData.tColor.usBlue = tReadFavData.usBlue;
                        tLightCtrlData.tColorOrigin.usHue = tReadFavData.usHue;
                        tLightCtrlData.tColorOrigin.usSat = tReadFavData.usSat;
                        tLightCtrlData.tColorOrigin.usValue = tReadFavData.usValue;
                        break;
                    case SCENE_MODE:{
                        u8 scene_num = tReadFavData.sence;
                        scene_num = scene_num << 4;
                        //TY_LOG_DEBUG("tReadFavData.sence == %x, scene_num == %x", tReadFavData.sence, scene_num);
                        ty_light_scene_cmd_data_set(&scene_num,2);
                        }
                        break;
                    default:
                        break;
                    }
                }
                app_light_ctrl_data_auto_save_start(APP_DATA_AUTO_SAVE_DELAY_TIME);
                app_light_ctrl_proc();
            }
            break;
		case TYPE_USER_DATA:{ 
                LIGHT_CUST_DATA_FLASH_T tReadCustData;
                memset(&tReadCustData, 0, sizeof(LIGHT_CUST_DATA_FLASH_T));
            #if (LIGHT_CFG_3IN1_SAVE == 1) 
                ty_light_save_user_flash_read_3in1_data(TYPE_USER_DATA,&tReadCustData);
            #else
                ty_light_save_user_flash_read_data(TYPE_USER_DATA,&tReadCustData);
            #endif
            }
		    break;
		default:
		    break;
	}
    return LIGHT_OK;
}


/* @brief: light ctrl data autosave timer callback
 * @param {none}
 * @retval: none
 */
static void app_light_ctrl_data_autosave_timer_callback(void)
{
    OPERATE_LIGHT opRet = 1;

    TY_LOG_DEBUG("auto save dp ctrl!");
	opRet = app_light_ctrl_data_auto_save(TYPE_APP_DATA);
    opRet = app_light_ctrl_data_auto_save(TYPE_MEM_DATA);
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("Light ctrl data auto save error!");
    }else{
        TY_LOG_DEBUG("Light ctrl app auto data save OK !");
    }

    opRet = ty_light_basis_sw_timer_stop(AUTOSAVE_SW_TIMER);
    if(opRet != LIGHT_OK) {
       TY_LOG_ERR("stop autosave software timer error!");
    }
}


/**
 * @brief: light ctrl data auto save proc
 * @param {IN u32 uiDelayTimeMs -> ctrl data auto save delay time,uint:ms}
 * @attention: save data proc will do after the last call actually!
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_ctrl_data_auto_save_start(IN u32 uiDelayTimeMs)
{
    OPERATE_LIGHT opRet = 1;
    
    opRet = ty_light_basis_sw_timer_start(AUTOSAVE_SW_TIMER, uiDelayTimeMs, (void*)app_light_ctrl_data_autosave_timer_callback);
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("data save timer start error!");
    }

    return LIGHT_OK;
}


/**
 * @brief: light ctrl data init
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_ty_light_ctrldata_init(void)
{
    LIGHT_APP_DATA_FLASH_T tAPPData;
    LIGHT_MEM_USER_FLASH_T tMemData;
    OPERATE_LIGHT opRet = 1,opRet2 = 1;
    memset(&tAPPData, 0, sizeof(LIGHT_APP_DATA_FLASH_T));
#if (LIGHT_CFG_3IN1_SAVE == 1)
    opRet = ty_light_save_user_flash_read_3in1_data(TYPE_APP_DATA,(u8*)&tAPPData);       /* read app flash data! */	
    opRet2 = ty_light_save_user_flash_read_3in1_data(TYPE_MEM_DATA,(u8*)&tMemData);
#else
    ty_light_save_user_flash_offset_init();
	opRet = ty_light_save_user_flash_read_data(TYPE_APP_DATA,&tAPPData);       /* read app flash data! */	
    opRet2 = ty_light_save_user_flash_read_data(TYPE_MEM_DATA,&tMemData);
#endif
    if(opRet2 != LIGHT_OK)
    {
        tLightMemCfgData.usMemory = ty_light_json_config_get_pmemory_cfg();
        light_not_disturb = ty_light_json_config_get_notdisturb();
        tLightMemCfgData.usFlag = 0;//此flag只标记断电记忆，不标注勿扰模式
    }else
    {
        tLightMemCfgData.usFlag = tMemData.usFlag;
        light_not_disturb = tMemData.usNotDisturb ;
        tLightMemCfgData.usMemory = tMemData.usMemory;
        tLightMemCfgData.usHue = tMemData.usHue;
        tLightMemCfgData.usSat = tMemData.usSat;
        tLightMemCfgData.usValue = tMemData.usValue;
        tLightMemCfgData.usBright = tMemData.usBright;
        tLightMemCfgData.usTemper = tMemData.usTemper;

        if(0xFFFF != tMemData.usSyncGroupAddr)
        {
            ty_light_scene_cmd_set_groupid(tMemData.usSyncGroupAddr);
        }
    }
    //tLightMemCfgData.usFlag = TRUE;
	//tLightMemCfgData.usMemory = USER_MODE;
    /*
        1、APP  not save,Mem not save-----reset
        2、APP  not save,Mem save(0x00)---reset
        3、APP  not save,Mem save(0x01)---ctrl
        4、APP  not save,Mem save(0x02)---user
        5、APP  save,Mem not save---------ctrl
        6、APP  save,Mem save(0x00)-------reset
        7、APP  save,Mem save(0x01)-------ctrl
        8、APP  save,Mem save(0x02)-------user

        因为需要考虑(不记忆模式),现在autosave除了user模式不保存以外,其余都保存
        所以考虑以下状态

        1、 APP  not save,Mem not save-----reset
        2、 APP  not save,Mem save(0x00)---reset
        3、 APP  not save,Mem save(0x01)---reset
        4、 APP  not save,Mem save(0x02)---user
        5、 APP  save--DEFA,Mem not save---------reset(onoff)
        6、 APP  save--DEFA,Mem save(0x00)-------reset(onoff)
        7、 APP  save--DEFA,Mem save(0x01)-------ctrl
        8、 APP  save--DEFA,Mem save(0x02)-------user
        9、 APP  save--PMEM,Mem not save---------ctrl
        10、APP  save--PMEM,Mem save(0x00)-------reset(onoff)
        11、APP  save--PMEM,Mem save(0x01)-------ctrl
        12、APP  save--PMEM,Mem save(0x02)-------user

    */
    if((TRUE == tLightMemCfgData.usFlag)&&(USER_MODE == tLightMemCfgData.usMemory))
    {
        tLightCtrlData.bSwitch = tAPPData.bPower;
        tLightCtrlData.eMode = tAPPData.eMode;
        tLightCtrlData.usBright = tAPPData.usBright;
        tLightCtrlData.usTemper = tAPPData.usTemper;
        tLightCtrlData.tColor.usRed = tAPPData.tColor.usRed;
        tLightCtrlData.tColor.usGreen = tAPPData.tColor.usGreen;
        tLightCtrlData.tColor.usBlue = tAPPData.tColor.usBlue;
        tLightCtrlData.tColorOrigin.usHue = tAPPData.tColorOrigin.usHue;
        tLightCtrlData.tColorOrigin.usSat = tAPPData.tColorOrigin.usSat;
        tLightCtrlData.tColorOrigin.usValue = tAPPData.tColorOrigin.usValue;
        strcpy((char*)&tLightCtrlData.tColorOrigin.cColorStr, (char*)&tAPPData.tColorOrigin.cColorStr);
    #if (LIGHT_CFG_3IN1_SAVE == 1)
        ty_light_scene_init_dataset(&tAPPData.cScene, sizeof(tAPPData.cScene));
    #else
        ty_light_scene_init_id_dataset(tAPPData.tSceneId);
    #endif
        ty_light_scene_ctrl_firstset(TRUE);
        tLightCtrlData.uiCountDown = 0;

        //user
        if(tLightMemCfgData.usValue > 0)
        {
            tLightCtrlData.eMode = COLOR_MODE;

            app_light_ctrl_data_hsv_set(tLightMemCfgData.usHue, tLightMemCfgData.usSat, tLightMemCfgData.usValue);
        }else if(tLightMemCfgData.usBright > 0)
        {
            tLightCtrlData.eMode = WHITE_MODE;
            tLightCtrlData.usBright = tLightMemCfgData.usBright;
            tLightCtrlData.usTemper = tLightMemCfgData.usTemper;
            if((LIGHT_C == ty_light_cfg_init_get_lightway())||(LIGHT_RGBC == ty_light_cfg_init_get_lightway()))
            {
                tLightCtrlData.usTemper = 0x03e8;
            }
        }

        return LIGHT_OK;
    }else if(((FALSE == tLightMemCfgData.usFlag)&&(DEFA_MODE == ty_light_cfg_init_get_bMemory()))||((opRet == LIGHT_OK)&&(TRUE == tLightMemCfgData.usFlag)&&(DEFA_MODE == tLightMemCfgData.usMemory))) {
        TY_LOG_NOTICE("No application data!");
        ty_light_cfg_init_ctrl_data_reset();
    #if (LIGHT_CFG_3IN1_SAVE == 1)
        ty_light_scene_init_dataset(&tAPPData.cScene, sizeof(tAPPData.cScene));
    #else
        ty_light_scene_init_id_dataset(tAPPData.tSceneId);
    #endif
        tLightCtrlData.bSwitch = tAPPData.bPower;
        return LIGHT_OK;
    }else if((USER_MODE != tLightMemCfgData.usMemory)&&(opRet != LIGHT_OK)) {
        TY_LOG_NOTICE("No application data!");
        ty_light_cfg_init_ctrl_data_reset();
    #if (LIGHT_CFG_3IN1_SAVE == 1)
        ty_light_scene_init_dataset(&tAPPData.cScene, sizeof(tAPPData.cScene));
    #else
        ty_light_scene_init_id_dataset(tAPPData.tSceneId);
    #endif
        return LIGHT_OK;
    }else {
        tLightCtrlData.bSwitch = tAPPData.bPower;
        tLightCtrlData.eMode = tAPPData.eMode;
        tLightCtrlData.usBright = tAPPData.usBright;
        tLightCtrlData.usTemper = tAPPData.usTemper;
        tLightCtrlData.tColor.usRed = tAPPData.tColor.usRed;
        tLightCtrlData.tColor.usGreen = tAPPData.tColor.usGreen;
        tLightCtrlData.tColor.usBlue = tAPPData.tColor.usBlue;
        tLightCtrlData.tColorOrigin.usHue = tAPPData.tColorOrigin.usHue;
        tLightCtrlData.tColorOrigin.usSat = tAPPData.tColorOrigin.usSat;
        tLightCtrlData.tColorOrigin.usValue = tAPPData.tColorOrigin.usValue;
        strcpy((char*)&tLightCtrlData.tColorOrigin.cColorStr, (char*)&tAPPData.tColorOrigin.cColorStr);
    #if (LIGHT_CFG_3IN1_SAVE == 1)
        ty_light_scene_init_dataset(&tAPPData.cScene, sizeof(tAPPData.cScene));
    #else
        ty_light_scene_init_id_dataset(tAPPData.tSceneId);
    #endif
        ty_light_scene_ctrl_firstset(TRUE);
        tLightCtrlData.uiCountDown = 0;
        return LIGHT_OK;
    }
    return -1;
}


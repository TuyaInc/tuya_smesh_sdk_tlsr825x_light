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

#include "app_light_control.h"
#include "ty_light_driver_set.h"
#include "ty_light_basis_tools.h"
#include "ty_light_save_user_flash.h"
#include "ty_light_cfg_init.h"
#include "ty_light_basis_sw_timer.h"
#include "ty_light_scene.h"

/// light ctrl data(status)
LIGHT_CTRL_DATA_T tLightCtrlData;

GRADUAL_CTRL_DATA_T params;
BRIGHT_DATA_T tCtrlData;

/**
 * @brief: Calc RGBCW handle data
 * @param {IN LIGHT_MODE_E Mode -> current mode}
 * @param {IN LIGHT_CTRL_DATA_T *CtrlData -> current ctrl data}
 * @param {OUT BRIGHT_DATA_T *Result -> output handle data}
 * @attention: support white and color mode only
 * @retval: none
 */
void app_ty_light_gradual_ctrl_calc_rgbcw(IN LIGHT_MODE_E Mode, IN LIGHT_CTRL_DATA_T *CtrlData, OUT BRIGHT_DATA_T *Result)
{   
    static LIGHT_MODE_E LastMode = MODE_MAX ;
    
    memset(Result, 0, sizeof(BRIGHT_DATA_T));

    switch(Mode) {
        case WHITE_MODE:
            ty_light_gradual_ctrl_calc_cw(CtrlData ->usBright, CtrlData ->usTemper, Result);
            TY_LOG_DEBUG(" set bright %d temper %d,calc ctrl data %d %d %d %d %d", CtrlData ->usBright, CtrlData ->usTemper,\
                Result ->usRed, Result->usGreen, Result ->usBlue, Result ->usWhite, Result ->usWarm);
            if((BRIGHT_MODE_CCT == ty_light_cfg_init_get_brightmode()) && (LastMode != Mode)) {
                BRIGHT_DATA_T CurrData;
                ty_light_gradual_ctrl_currentget(&CurrData);
                CurrData.usWarm = Result ->usWarm;
                ty_light_gradual_ctrl_currentset(CW_MODE,&CurrData);
            }
            break;
            
        case COLOR_MODE:
            if(BRIGHT_MODE_CCT == ty_light_cfg_init_get_brightmode()) {        
            /* to aviod the CCT temperature change, when CCT set as white mode, make sure color mode not change CCT! */
                BRIGHT_DATA_T CurrData;
                ty_light_gradual_ctrl_currentget(&CurrData);
                Result ->usWarm = CurrData.usWarm ;
            }
            ty_light_gradual_ctrl_calc_rgb(&(CtrlData ->tColor), Result);
            TY_LOG_DEBUG(" set bright %d temper %d,calc ctrl data %d %d %d %d %d", CtrlData ->usBright, CtrlData ->usTemper,\
                Result ->usRed, Result->usGreen, Result ->usBlue, Result ->usWhite, Result ->usWarm);
            break;

        case SCENE_MODE:
            ty_light_gradual_ctrl_calc_rgb(&(CtrlData ->tColor), Result);
            ty_light_gradual_ctrl_calc_cw(CtrlData ->usBright, CtrlData ->usTemper, Result); 
            TY_LOG_DEBUG("mode is scene mode!");
            
            TY_LOG_DEBUG(" set bright %d temper %d,calc ctrl data %d %d %d %d %d", CtrlData ->usBright, CtrlData ->usTemper,\
                Result ->usRed, Result->usGreen, Result ->usBlue, Result ->usWhite, Result ->usWarm);
            break;
            
        case MUSIC_MODE:
            ty_light_gradual_ctrl_calc_rgb(&(CtrlData ->tColor), Result);
            TY_LOG_DEBUG("mode is music mode!");
            TY_LOG_DEBUG(" set bright %d temper %d,calc ctrl data %d %d %d %d %d", CtrlData ->usBright, CtrlData ->usTemper,\
                Result ->usRed, Result->usGreen, Result ->usBlue, Result ->usWhite, Result ->usWarm);
            if(BRIGHT_MODE_CCT == ty_light_cfg_init_get_brightmode()) {
                BRIGHT_DATA_T CurrData;
                ty_light_gradual_ctrl_currentget(&CurrData);
                Result ->usWarm = CurrData.usWarm ;
            }
            break;
        default:
            break;
    }
    LastMode = Mode;
}


/**
 * @brief: Light control proc
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_ctrl_proc(void)
{
    OPERATE_LIGHT opRet = 1;
    u32 uiCycleTime = 0;
    bool bEableFlag = 0;
    static bool bLastSwitchStatus = FALSE;

    ty_light_gradual_ctrl_onoffset(tLightCtrlData.bSwitch);

    if(tLightCtrlData.bSwitch == FALSE) {  /* onoff ctrl state -- turn off */
    
        app_light_shade_ctrl_disable();       /* stop all shade process! */
        
        memset(&tCtrlData, 0, sizeof(BRIGHT_DATA_T));  /* set target contol data!!! */

        ty_light_gradual_ctrl_targetset(RGBCW_MODE,&tCtrlData);

        if(SWITCH_GRADUAL == ty_light_cfg_init_get_switchmode()) {     /* shut down gradually */
            switch(tLightCtrlData.eMode)
            {
                case WHITE_MODE:
                case COLOR_MODE:
                    TY_LOG_DEBUG("start to shut down!");
                    params.eMode = NORMAL_MODE;
                    ty_light_gradual_ctrl_start(&params);
                    break;
                    
                case SCENE_MODE:
                    TY_LOG_DEBUG("turn off disable scene timer flag!");
                    params.eMode = NORMAL_MODE;
                    ty_light_gradual_ctrl_start(&params);
                    break;

                case MUSIC_MODE:
                    params.eMode = JUMP_MODE;
                    ty_light_gradual_ctrl_start(&params);
                    break;
                    
                default:
                    break;
            }
        } else if(SWITCH_DIRECT == ty_light_cfg_init_get_switchmode()) {   /* shut down directly */
            ty_light_scene_ctrl_change_stop();
            params.eMode = JUMP_MODE;
            ty_light_gradual_ctrl_start(&params);
        } else {    /* LightCfgData.Switch_mode */
            ;       /* no possible  */
        }
        

    } else {    /* onoff ctrl state -- turn on */ 

        TY_LOG_DEBUG("current mode %d", tLightCtrlData.eMode);
        
        if(tLightCtrlData.eMode == SCENE_MODE) {
            
            ty_light_scene_ctrl_start();
            opRet = ty_light_scene_ctrl_change(&bEableFlag, &uiCycleTime);   /* start */
            if(opRet != LIGHT_OK){
                TY_LOG_ERR("Scene change err!");
            }

            if(bEableFlag == TRUE) {
                ty_light_scene_ctrl_change_start(uiCycleTime);
            } else {
                TY_LOG_DEBUG("Scene don't need change!");
                opRet = ty_light_basis_sw_timer_stop(SCENE_SW_TIMER);      /* stop timer dominantly */
                if(opRet != LIGHT_OK) {
                    TY_LOG_ERR("stop scene timer error!");
                }
            }
        } else if(tLightCtrlData.eMode == MUSIC_MODE) {   /* mode is music mode!!!! */ /* this mode only appear turn on firstly */
            ty_light_scene_ctrl_change_stop();

            LIGHT_CTRL_DATA_T tMusicCtrlData;
            memset(&tMusicCtrlData, 0, sizeof(LIGHT_CTRL_DATA_T));
            memcpy(&tMusicCtrlData, &tLightCtrlData, sizeof(LIGHT_CTRL_DATA_T));     /* make sure music mode restart as red color */
            memset(&tCtrlData, 0, sizeof(BRIGHT_DATA_T));  /* set target contol data!!! */

            tMusicCtrlData.tColor.usRed = CTRL_CAL_VALUE_RANGE;
            tMusicCtrlData.tColor.usGreen = 0;
            tMusicCtrlData.tColor.usBlue = 0;
            
            app_ty_light_gradual_ctrl_calc_rgbcw(tLightCtrlData.eMode,  &tMusicCtrlData, &tCtrlData);

            ty_light_gradual_ctrl_targetset(RGB_MODE,&tCtrlData);
            params.eMode = NORMAL_MODE;
            ty_light_gradual_ctrl_start(&params);
            
        } else {    /* mode is not scene and music mode ,mode is white or color mode ! */
            ty_light_scene_ctrl_change_stop();
            if(bLastSwitchStatus != FALSE) {        /* already turn on */
                app_ty_light_gradual_ctrl_calc_rgbcw(tLightCtrlData.eMode, &tLightCtrlData, &tCtrlData);
                
                ty_light_gradual_ctrl_targetset(RGBCW_MODE,&tCtrlData);
                params.eMode = NORMAL_MODE;
                ty_light_gradual_ctrl_start(&params);
                
            } else {        /* firstly turn on */
            
                if(SWITCH_GRADUAL == ty_light_cfg_init_get_switchmode()) {
                    app_ty_light_gradual_ctrl_calc_rgbcw(tLightCtrlData.eMode, &tLightCtrlData, &tCtrlData);

                    ty_light_gradual_ctrl_targetset(RGBCW_MODE,&tCtrlData);
                    params.eMode = NORMAL_MODE;
                    ty_light_gradual_ctrl_start(&params);
                    
                } else if(SWITCH_DIRECT == ty_light_cfg_init_get_switchmode()) {
                    app_ty_light_gradual_ctrl_calc_rgbcw(tLightCtrlData.eMode,  &tLightCtrlData, &tCtrlData);

                    ty_light_gradual_ctrl_targetset(RGBCW_MODE,&tCtrlData);
                    params.eMode = NORMAL_MODE;
                    ty_light_gradual_ctrl_start(&params);
                    
                } else {    /* LightCfgData.Switch_mode */    
                    ;       /* no possible  */
                }
            }
       } 

    }

    bLastSwitchStatus = tLightCtrlData.bSwitch;
    
    return LIGHT_OK;
}

/**
 * @brief: light ctrl disable
 * @param {none}
 * @retval: none
 */
void app_light_ctrl_disable(void)
{
    OPERATE_LIGHT opRet = 1;
    u8 i = 0;
    
    opRet = ty_light_basis_hw_timer_stop();
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("hardware timer stop error!");
    }

    for(i = 0; i < SW_TIMER_MAX; i++) {
        opRet = ty_light_basis_sw_timer_stop(i);
        if(opRet != LIGHT_OK) {
           TY_LOG_ERR("stop %d timer error!");
        }
    }
}

/**
 * @brief: shade process disable
 * @param {none} 
 * @retval: none
 */
void app_light_shade_ctrl_disable(void)
{
    ty_light_gradual_ctrl_stop();                  /* first stop shade change! */
    ty_light_scene_ctrl_change_stop();            /* stop scene change firstly */
}

/**
 * @brief: Light realtime ctrl process
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_real_time_ctrl_proc(void)
{
    u8 ucChangeMode = 0;
    u16 usBright ,usTemp;
    u16 usHue, usSat, usVal;
    LIGHT_CTRL_DATA_T tCtrlDataTemp;
    OPERATE_LIGHT opRet = 1;

    COLOR_RGB_T ColorDataTemp;

    if(!tLightCtrlData.bSwitch) {        /* make sure realtime adjust dp can't active! */
        return LIGHT_OK;
    }
    
    ty_light_scene_hsvbt_analysize(&tLightCtrlData.cRealTimeData[1], &usHue, &usSat, &usVal, &usBright, &usTemp);
    
    usVal = ty_light_gradual_ctrl_calc_hsv_limit(usVal);
    
    TY_LOG_DEBUG("hsv %d %d %d",usHue,usSat,usVal);

    
    if(WHITE_MODE == tLightCtrlData.eMode) {

        tLightCtrlData.usBright = usBright;   /* attention: realtime control data need save to ctrl data directly! */
        tLightCtrlData.usTemper = usTemp;     
        /* attention: as when adjust the temper or color but set the same data finally ! */
        app_ty_light_gradual_ctrl_calc_rgbcw(tLightCtrlData.eMode, &tLightCtrlData, &tCtrlData);
        ty_light_gradual_ctrl_targetset(CW_MODE,&tCtrlData);
    } else if(COLOR_MODE == tLightCtrlData.eMode) {
                    
        memset(&ColorDataTemp, 0, sizeof(COLOR_RGB_T));
        ty_light_basis_tools_hsv2rgb(usHue, usSat, usVal, &ColorDataTemp.usRed, &ColorDataTemp.usGreen, &ColorDataTemp.usBlue);
        tLightCtrlData.tColor.usRed = ColorDataTemp.usRed;
        tLightCtrlData.tColor.usGreen = ColorDataTemp.usGreen;
        tLightCtrlData.tColor.usBlue = ColorDataTemp.usBlue;
        app_ty_light_gradual_ctrl_calc_rgbcw(tLightCtrlData.eMode, &tLightCtrlData, &tCtrlData);
        ty_light_gradual_ctrl_targetset(RGB_MODE,&tCtrlData);
    } else {    /* scene mode or music mode */

        if(SCENE_MODE == tLightCtrlData.eMode) {
            tLightCtrlData.ucRealTimeFlag = TRUE;        /* have set realtime data  */
        }
        memset(&tCtrlDataTemp, 0, sizeof(LIGHT_CTRL_DATA_T));
        memset(&ColorDataTemp, 0, sizeof(COLOR_RGB_T));

        tCtrlDataTemp.usBright = usBright;
        tCtrlDataTemp.usTemper = usTemp;

        TY_LOG_DEBUG(" scene hsv %d %d %d",usHue,usSat,usVal);
        
        ty_light_basis_tools_hsv2rgb(usHue, usSat, usVal, &ColorDataTemp.usRed, &ColorDataTemp.usGreen, &ColorDataTemp.usBlue);
        tCtrlDataTemp.tColor.usRed = ColorDataTemp.usRed;
        tCtrlDataTemp.tColor.usGreen = ColorDataTemp.usGreen;
        tCtrlDataTemp.tColor.usBlue = ColorDataTemp.usBlue;
        app_ty_light_gradual_ctrl_calc_rgbcw(tLightCtrlData.eMode, &tCtrlDataTemp, &tCtrlData);
        ty_light_gradual_ctrl_targetset(RGBCW_MODE,&tCtrlData);
    }

    ucChangeMode = tLightCtrlData.cRealTimeData[0] - '0';

    app_light_shade_ctrl_disable();       /* stop all shade process! */
            
    if(REALTIME_CHANGE_JUMP == ucChangeMode) {
        params.eMode = JUMP_MODE;
    } else {
        u16 usMaxError ;
        params.eMode = STEP_MODE;
        usMaxError = ty_light_gradual_get_difmax();
        params.usData = (u16)((u32)((usMaxError * LIGHT_SHADE_CYCLE / 300.0)));     /* 150ms APP issue realtime control dp */
    }

    ty_light_gradual_ctrl_start(&params);

    return LIGHT_OK;
}


/**
 * @brief: directly control light on
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
void app_light_on_direct(void)
{
    ty_light_gradual_startblink();
}


/**
 * @brief: directly control light off
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
void app_light_off_direct(void)
{
    ty_light_driver_set_rgbcw(0, 0, 0, 0, 0);
}


/**
 * @brief: light control in loop
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
void app_light_ctrl_loop(void)
{
    ty_light_basis_sw_timer_handler();
}


/**
 * @brief: enable light control gradually
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
void app_light_ctrl_gradual_enable(void)
{
    OPERATE_LIGHT opRet = 1;
    opRet = ty_light_basis_hw_timer_start(HW_TIMER_CYCLE_US, (void *)ty_light_gradual_hw_timer_handler);     /* start shade need hardware timer! */
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("Light hardware timer init error!");
        return opRet;
    }
}

void app_show_tuya_ota_result(int result)
{
    if(tLightCtrlData.eMode == SCENE_MODE){
        app_light_ctrl_data_mode_set(SCENE_MODE);
    }
}

void app_entry_tuya_ota_mode(void)
{
    extern u8 ui_ota_is_working;
    ui_ota_is_working = 1;

    if(tLightCtrlData.eMode == SCENE_MODE){
        ty_light_scene_ctrl_change_stop();
    }

	blc_ll_setScanEnable (0, 0);
	bls_ota_setTimeout(30 * 1000000); //set OTA timeout
}





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

#include "app_light_prompt.h"
#include "ty_light_driver_set.h"


#include "ty_light_gradual.h"
#include "ty_light_basis_sw_timer.h"
#include "app_light_control.h"
#include "ty_light_cfg_init.h"
#include "ty_light_json_config.h"
#define BREATH_UNIT_TIME             2  //2s @attention: accurate time is 2100ms
#define BREATH_ALWAY_ON_TIME        602         //@attention: breath alway on config time
#define BREATH_ALWAY_ON_CNT         0xFFFFFFFF   //@attention: this value to avoid breath stop!(bigger than 65535)

/// Light blink cycletime
static u32 guiBlinkTimeMs = 0;
/**
 * @brief: light ctrl normal status(constantly bright) display proc
 * @param {type} 
 * @retval: none
 */
static void app_light_ctrl_normal_display(void)
{
    OPERATE_LIGHT opRet = 1;
    BRIGHT_DATA_T tCtrlData;

    TY_LOG_DEBUG("normal display....");
    memset(&tCtrlData, 0, sizeof(BRIGHT_DATA_T));
    
    app_light_shade_ctrl_disable();       /* stop all shade process! */

    ty_light_gradual_ctrl_defbc_calc(ty_light_cfg_init_get_defcolor(), ty_light_cfg_init_get_defbright(), ty_light_cfg_init_get_deftemper(), &tCtrlData);
    ty_light_gradual_ctrl_targetset(RGBCW_MODE,&tCtrlData);
    TY_LOG_DEBUG("normal display %d %d %d %d %d", tCtrlData.usRed, tCtrlData.usGreen, tCtrlData.usBlue, tCtrlData.usWhite, tCtrlData.usWarm);
}



/**
 * @brief: light ctrl blink proc
 * @param {type} 
 * @retval: none
 */
static void app_light_ctrl_blink_display(void)
{
    OPERATE_LIGHT opRet = 1;
    static u16 ucCnt = 0;       /* first blink off */
    BRIGHT_DATA_T tCtrlData;
    u16 usBrightTemp;

    memset(&tCtrlData, 0, sizeof(BRIGHT_DATA_T));
    app_light_shade_ctrl_disable();       /* stop all shade process! */
    
    if((ty_light_cfg_init_get_netcolor() >= DEF_COLOR_R) && (ty_light_cfg_init_get_netcolor() <= DEF_COLOR_RGB)) {
        usBrightTemp = ty_light_gradual_ctrl_calc_hsv_limit(ty_light_cfg_init_get_netbright());
    }
    
    switch(ty_light_cfg_init_get_netcolor()) {
        case DEF_COLOR_C:
        case DEF_COLOR_W:
            ty_light_gradual_ctrl_calc_cw(ty_light_cfg_init_get_netbright(), ty_light_cfg_init_get_nettemper(), &tCtrlData);
            if(BRIGHT_MODE_CW == ty_light_cfg_init_get_brightmode()) {
                tCtrlData.usWhite = (ucCnt % 2) ? tCtrlData.usWhite : 0;
                tCtrlData.usWarm = (ucCnt % 2) ? tCtrlData.usWarm : 0;
            } else if(BRIGHT_MODE_CCT == ty_light_cfg_init_get_brightmode()) {
                tCtrlData.usWhite = (ucCnt % 2) ? tCtrlData.usWhite : 0;
                tCtrlData.usWarm = tCtrlData.usWarm ;
            }
            break;
            
        case DEF_COLOR_R:
            tCtrlData.usRed = (ucCnt % 2) ? (ty_light_gradual_ctrl_calc_gamma(DEF_COLOR_R, usBrightTemp)) : 0;
            break;

        case DEF_COLOR_G:
            tCtrlData.usGreen = (ucCnt % 2) ? (ty_light_gradual_ctrl_calc_gamma(DEF_COLOR_G, usBrightTemp)) : 0;
            break;

        case DEF_COLOR_B:
            tCtrlData.usBlue = (ucCnt % 2) ? (ty_light_gradual_ctrl_calc_gamma(DEF_COLOR_B, usBrightTemp)) : 0;
            break;

        case DEF_COLOR_RGB:
            tCtrlData.usRed = (ucCnt % 2) ? (ty_light_gradual_ctrl_calc_gamma(DEF_COLOR_R, usBrightTemp)) : 0;
            tCtrlData.usGreen = (ucCnt % 2) ? (ty_light_gradual_ctrl_calc_gamma(DEF_COLOR_G, usBrightTemp)) : 0;
            tCtrlData.usBlue = (ucCnt % 2) ? (ty_light_gradual_ctrl_calc_gamma(DEF_COLOR_B, usBrightTemp)) : 0;
            break;
            
        case DEF_COLOR_MAX:
        default:
            break;
    }

    opRet = ty_light_driver_set_rgbcw(tCtrlData.usRed, tCtrlData.usGreen, tCtrlData.usBlue, tCtrlData.usWhite, tCtrlData.usWarm);
    if(opRet != LIGHT_OK){
        TY_LOG_ERR("blink set RGBCW error!");
    }    
    if(ucCnt>=720)
    {
        ucCnt=0;
		ty_light_gradual_ctrl_defbc_calc(ty_light_cfg_init_get_defcolor(), ty_light_cfg_init_get_defbright(), ty_light_cfg_init_get_deftemper(), &tCtrlData);
        ty_light_gradual_ctrl_targetset(RGBCW_MODE,&tCtrlData);
		ty_light_gradual_startblink();
        opRet = ty_light_basis_sw_timer_stop(BLINK_SW_TIMER);
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("blink timer start error!");
        }
	}

    ucCnt++;
    
}

/**
 * @brief: light ctrl blink timer callback
 * @param {type} 
 * @retval: none
 */
static void app_light_ctrl_blink_timer_callback(void)
{
    OPERATE_LIGHT opRet = 1;
    
    //app_light_ctrl_blink_display();

    opRet = ty_light_basis_sw_timer_start(BLINK_SW_TIMER, guiBlinkTimeMs, (void*)app_light_ctrl_blink_timer_callback);
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("blink timer start error!");
    }
    app_light_ctrl_blink_display();
}

/**
 * @brief: start blink function
 * @param {IN u32 BlinkTimeMs -> blink on/off time, unit:ms} 
 * @attention: blink display will as the parm 
 *             -- eNetColor, usNetBright, usNetTempr in configuration.
 * @retval: none
 */
static OPERATE_LIGHT app_light_ctrl_blink_start(IN u32 uiBlinkTimeMs)
{
    OPERATE_LIGHT opRet = 1;
    
    guiBlinkTimeMs = uiBlinkTimeMs;
    opRet = ty_light_basis_sw_timer_start(BLINK_SW_TIMER, uiBlinkTimeMs, (void*)app_light_ctrl_blink_timer_callback);
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("blink timer start error!");
    }

    return LIGHT_OK;
}

/**
 * @brief: stop blink
 * @param {type}
 * @attention: blink stop will directly go to normal status display
 *              normal status will bright as default bright parm 
 *              -- DefColor, usDefBright, usDefTemper in configuration.
 * @retval: none
 */
static OPERATE_LIGHT app_light_ctrl_blink_stop(void)
{
    OPERATE_LIGHT opRet = 1;
    
    opRet = ty_light_basis_sw_timer_stop(BLINK_SW_TIMER);
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("blink timer stop error!");
    }
    app_light_ctrl_normal_display();
    return LIGHT_OK;
}


#define BREATH_TIMER_CYCLE_MS        10 //ms

/// Light breath cycle cnt
static u32 uiBreathCnt = 0;
static u8 ucBreathRate = 0;

typedef enum {
    BREATH_INIT = 0,
    BREATH_WAIT_INIT_OK,
    BREATH_CHANGE,
    BREATH_OK
}BREATH_STEP_E;

/**
 * @brief: light ctrl breath proc(breath as net config color&bright)
 * @param {none}
 * @return: OPERATE_LIGHT
 * @retval: none
 */
static OPERATE_LIGHT app_light_ctrl_breathing(void)
{    
    //Gaussian distribution                              
    const u16 sBreathParm[]  = {1000, 999, 997, 994, 989, 982, 975, 966, 956, 944, 
                                    932,  918, 903, 887, 870, 853, 834, 815, 795, 774,
                                    753,  731, 709, 687, 665, 642, 619, 596, 573, 551, 
                                    528,  506, 484, 462, 440, 419, 399, 379, 359, 340,
                                    322,  304, 286, 269, 253, 238, 223, 209, 195, 182,
                                    170,  158, 147, 136, 126, 117, 108, 100,  92,  85,
                                     78,   71,  65,  60,  55,  50,  46,  41,  38,  34,  
                                     31,   28,  25,  23,  21,  19,  17,  15,  13,  12,
                                     11,   10,   8,   8,   7,   6,   5,   5,   4,   4,   
                                      3,    3,   2,   2,   2,   2,   1,   1,   1,   1, 1};

    OPERATE_LIGHT opRet = 1;    
    static u8 ucBreathingStep = 0;
    static u16 usStandCnt = 0;
    static u16 usStandIndex = 0;
    static u8 ucStandAgainCnt = 0;
    BRIGHT_DATA_T tCtrlData;
    static u16 usMin = 0;
    u16 usBrightTemp = 0;
    u16 usTempIndex = 0;
     
    // opRet = ty_light_basis_sw_timer_start(BREATH_SW_TIMER, ucBreathRate * BREATH_TIMER_CYCLE_MS, (void*)app_light_ctrl_breathing);
    // if(opRet != LIGHT_OK) {
    //     TY_LOG_ERR("breath timer restart error!");
    //     return LIGHT_COM_ERROR;
    // }

    TY_LOG_DEBUG("Breath ok!!! start!!!!!");
    
    GRADUAL_CTRL_DATA_T params;

    if(BREATH_INIT == ucBreathingStep) {  /* change to mid brightness firstly */
        
        app_light_shade_ctrl_disable();       /* stop all shade process! */

        if((ty_light_cfg_init_get_netcolor() >= DEF_COLOR_R) && (ty_light_cfg_init_get_netcolor() <= DEF_COLOR_RGB)) {
            usMin = CTRL_RGB_BRIGHT_VALUE_MAX * ( (float) ty_light_cfg_init_get_colormin() / 100.0 );
        } else {
            usMin = CTRL_CW_BRIGHT_VALUE_MAX * ( (float) ty_light_cfg_init_get_cwmin() / 100.0 );
        }
        memset(&tCtrlData, 0, sizeof(BRIGHT_DATA_T));
        ty_light_gradual_ctrl_defbc_calc(ty_light_cfg_init_get_netcolor(), usMin, ty_light_cfg_init_get_nettemper(), &tCtrlData);

        ty_light_gradual_ctrl_targetset(RGBCW_MODE,&tCtrlData);

        params.eMode = NORMAL_MODE;

        ty_light_gradual_ctrl_start(&params); 
        
        ucBreathingStep = BREATH_WAIT_INIT_OK;
    }

    if(BREATH_WAIT_INIT_OK == ucBreathingStep) {
        usStandIndex = 0;
        ucBreathingStep = BREATH_CHANGE;
    } else if(BREATH_CHANGE == ucBreathingStep) {
        
        memset(&tCtrlData, 0, sizeof(BRIGHT_DATA_T));

        if(ucStandAgainCnt < 1) {
            usTempIndex = ((sizeof(sBreathParm)/sizeof(u16)) - usStandIndex - 1);  /*  */
        } else {
            usTempIndex = usStandIndex;
        }

        usBrightTemp = (u16) (sBreathParm[usTempIndex] /1000.0 * (ty_light_cfg_init_get_netbright() - usMin)) + usMin ;
        ty_light_gradual_ctrl_defbc_calc(ty_light_cfg_init_get_netcolor(), usBrightTemp, ty_light_cfg_init_get_nettemper(), &tCtrlData);

        usStandIndex ++;
        if(usStandIndex >= (sizeof(sBreathParm)/sizeof(u16))) {
            usStandIndex = 0;
            ucStandAgainCnt++;
            if(ucStandAgainCnt >= 2) {
                ucBreathingStep = BREATH_OK;
                ucStandAgainCnt = 0;
            } else {
                 ucBreathingStep = BREATH_CHANGE;
            }

        }       
        
        ty_light_gradual_ctrl_targetset(RGBCW_MODE,&tCtrlData);

        params.eMode = NORMAL_MODE;

        ty_light_gradual_ctrl_start(&params); 
        
    }//else 
    if(ucBreathingStep == BREATH_OK) {
        usStandIndex ++;

        if(usStandIndex < 2) {  /* wait 2*BREATH_TIMER_CYCLE_MS (ms) */
            ;
        } else {
            usStandIndex = 1;
            usStandCnt++;
            if(usStandCnt >= uiBreathCnt) { // breath unit cnt ok 
            //@attention: if set alway breath, the uiBreathCnt will set 0xFFFFFFFF or (0xFFFFFFFF / 3) (bigger than 65535)
                ucBreathingStep = BREATH_INIT;
                usStandCnt = 0;
                usStandIndex = 0;

                tuya_gatt_adv_beacon_enable(1);
                memset(&tCtrlData, 0, sizeof(BRIGHT_DATA_T));
                ty_light_gradual_ctrl_defbc_calc(ty_light_cfg_init_get_defcolor(), ty_light_cfg_init_get_defbright(), ty_light_cfg_init_get_deftemper(), &tCtrlData);
                
                ty_light_gradual_ctrl_targetset(RGBCW_MODE,&tCtrlData);

                params.eMode = NORMAL_MODE;

                ty_light_gradual_ctrl_start(&params); 

                TY_LOG_DEBUG("Breath ok!!! stop!!!!!");
                opRet = ty_light_basis_sw_timer_stop(BREATH_SW_TIMER);
                if(opRet != LIGHT_OK) {
                    TY_LOG_ERR("blink timer stop error!");
                }
                return LIGHT_OK;
            } else {
                ucBreathingStep = BREATH_CHANGE;
            }   
        }
    }else{
        opRet = ty_light_basis_sw_timer_start(BREATH_SW_TIMER, ucBreathRate * BREATH_TIMER_CYCLE_MS, (void*)app_light_ctrl_breathing);
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("breath timer restart error!");
            return LIGHT_COM_ERROR;
        }       
    }

    return LIGHT_OK;
}


/**
 * @brief: start breathing function
 * @param {IN u32 BreathTimeMs -> Branth up/down time, unit:ms} 
 * @attention: breath display will as the parm 
 *             -- NetColor, usNetBright, usNetTemper in configuration.
 * @retval: OPERATE_LIGHT
 */
static OPERATE_LIGHT app_light_ctrl_breathing_start(IN u8 BreathRate, IN u32 BreathCnt)
{
    OPERATE_LIGHT opRet = 1;
    
    ucBreathRate = BreathRate;
    uiBreathCnt = BreathCnt / ucBreathRate;
    TY_LOG_NOTICE("breath cnt %d",uiBreathCnt);
    opRet = ty_light_basis_sw_timer_start(BREATH_SW_TIMER, BREATH_TIMER_CYCLE_MS, (void*)app_light_ctrl_breathing);
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("breath timer stop error!");
        return LIGHT_COM_ERROR;
    }
    
    return LIGHT_OK;
}


/**
 * @brief: stop breathing
 * @param {type} 
 * @attention: Breathing stop will directly go to normal status display
 *             normal status will bright as default bright parm 
 *              -- DefColor, usDefBright, usDefTemper in configuration.
 * @retval: OPERATE_LIGHT
 */
static OPERATE_LIGHT app_light_ctrl_breathing_stop(void)
{
    OPERATE_LIGHT opRet = 1;

    opRet = ty_light_basis_sw_timer_stop(BREATH_SW_TIMER);
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("breath timer stop error!");
        return LIGHT_COM_ERROR;
    }
    
    app_light_ctrl_normal_display();
    return LIGHT_OK;
}


static OPERATE_LIGHT app_light_prompt_ctrl_auto_start(void)
{
    OPERATE_LIGHT opRet = 1;

    ty_light_gradual_ctrl_onoffset(TRUE);

    if(ty_light_json_config_devicecfg_get_remdmode() == TRUE) {    /* blink when pairing */  

        opRet = app_light_ctrl_blink_start(250);
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("start blink timer error!");
        }

    } else {    /* breath when pairing */
        tuya_gatt_adv_beacon_enable(0);
        opRet = app_light_ctrl_breathing_start(1, ty_light_json_config_get_remdtime() / BREATH_UNIT_TIME);
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("start breath timer error!");
        }
    } 

    return opRet;
}
/**
 * @brief: stop blink or breath according to pairing remind mode
 * @param {type}
 * @retval: none
 */
static OPERATE_LIGHT app_light_prompt_ctrl_auto_stop(void)
{
    OPERATE_LIGHT opRet = -1;

    if(ty_light_json_config_devicecfg_get_remdmode() == TRUE) {    /* blink when pairing */
        opRet = app_light_ctrl_blink_stop();
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("blink timer stop error!");
            return LIGHT_COM_ERROR;
        }
    }else{
    #if 0
       opRet = app_light_ctrl_breathing_stop();
       if(opRet != LIGHT_OK) {
           PR_ERR("breath timer stop error!");
           return LIGHT_COM_ERROR;
       }
    #endif
    }
    
    return LIGHT_OK;
}


void app_light_ctrl_prompt_start(void)
{
    OPERATE_LIGHT opRet = 1;
    if(ty_light_json_config_get_jsonver()>=117){
        if(ty_light_cfg_init_get_connect_mode()== 0x02){
            if(tuya_hal_get_power_cnt()>=ty_light_json_config_get_resetnum()){
				opRet = app_light_prompt_ctrl_auto_start();  /* start prompt */
                if(opRet != LIGHT_OK) {
                    TY_LOG_ERR("start prompt timer error!");
                }
            }else{
                app_light_ctrl_proc();
            }
            
        }else{
			opRet = app_light_prompt_ctrl_auto_start();  /* start prompt */
            if(opRet != LIGHT_OK) {
                TY_LOG_ERR("start prompt timer error!");
            }
        }
    }else{
		opRet = app_light_prompt_ctrl_auto_start();  /* start prompt */
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("start prompt timer error!");
        }
    }
}

void app_light_ctrl_prompt_stop(void)
{
    OPERATE_LIGHT opRet = 1;
    opRet = app_light_prompt_ctrl_auto_stop();  /* stop prompt */
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("stop prompt timer error!");
    }
}


/**
 * @brief: fav scene auto restart
 * @param {none} 
 * @attention: to restart scene when remote don't submit the scene data!! 
 * @retval: none
 */
void ty_light_fav_scene_auto_restart_time_callback(void)
{
    OPERATE_LIGHT opRet = 1;
    u32 uiCycleTime = 0;
    bool bEableFlag = 0;

	ty_light_scene_ctrl_start();
    opRet = ty_light_scene_ctrl_change(&bEableFlag, &uiCycleTime);   /* start */
    if(opRet != LIGHT_OK){
        TY_LOG_ERR("Scene change err!");
    }

    if(bEableFlag == TRUE) {
        ty_light_scene_ctrl_change_start(uiCycleTime);
    } else {
        PR_DEBUG("Scene don't need change!");
        opRet = ty_light_basis_sw_timer_stop(SCENE_SW_TIMER);      /* stop timer dominantly */
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("stop scene timer error!");
        }
    }

    opRet = ty_light_basis_sw_timer_stop(SCENE_AUTO_RESTART_TIMER);    /* stop timer dominantly */
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("scene auto restart timer stop error!");
    }
}
/**
 * @brief: fav scene auto restart
 * @param {none} 
 * @attention: to restart scene when app don't remote the scene data!! 
 * @retval: none
 */
OPERATE_LIGHT app_light_fav_ctrl_data_mode_set(IN LIGHT_MODE_E eMode)
{   
	OPERATE_LIGHT opRet = 1;
    opRet = ty_light_basis_sw_timer_start(SCENE_AUTO_RESTART_TIMER, 1000, ty_light_fav_scene_auto_restart_time_callback);
    if( opRet != LIGHT_OK) {
    TY_LOG_ERR("scene auto restart timer init error!");
        return LIGHT_INVALID_PARM;
    }
    
    return LIGHT_OK;
}
/**
 * @brief: light ctrl favorite proc(favorite as net config color&bright)
 * @param {none}
 * @return: OPERATE_LIGHT
 * @retval: none
 */
static OPERATE_LIGHT app_light_ctrl_favorite(void)
{                        
    OPERATE_LIGHT opRet = 1;    
	BRIGHT_DATA_T Result;

    static u16 usStandIndex = 0;
    static u8 ucCnt = 0; 

	app_light_shade_ctrl_disable();

	ty_light_scene_ctrl_change_stop();

    opRet = ty_light_basis_sw_timer_start(FAVOTIE_SAVE_TIMER, 500, (void*)app_light_ctrl_favorite);
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("breath timer restart error!");
        return LIGHT_COM_ERROR;
    }
    	
    if(ucCnt % 2 == 0)
    {

        bool bSwitch = app_light_ctrl_data_switch_get();
        app_light_ctrl_data_switch_set(!bSwitch);

        if(app_light_ctrl_data_switch_get() == TRUE)
        {
            if(ty_light_json_config_get_color_mode()==CMOD_RGB)
            {
                opRet = ty_light_driver_set_rgbcw(0,ty_light_gradual_ctrl_calc_gamma(DEF_COLOR_G, 400),0,0,0);
                if(opRet != LIGHT_OK) {
                    TY_LOG_ERR("Light ctrl turn on set RGBCW error!");
                    return opRet;
		        }
            }
            else
            {
                 ty_light_gradual_ctrl_calc_cw(400, 1000, &Result);
                 opRet = ty_light_driver_set_rgbcw(0,0,0,Result.usWhite,Result.usWarm);
                 if(opRet != LIGHT_OK) {
                 TY_LOG_ERR("Light ctrl turn on set RGBCW error!");
                 return opRet;
			     }
            }
        }
		
	    else
	    {
            opRet = ty_light_driver_set_rgbcw(0,0,0,0,0);
            if(opRet != LIGHT_OK) {
                TY_LOG_ERR("Light ctrl turn on set RGBCW error!");
                return opRet;
            }
		}       	
    }
	else
	{
        bool bSwitch = app_light_ctrl_data_switch_get();
        app_light_ctrl_data_switch_set(!bSwitch);

        if(app_light_ctrl_data_switch_get() == TRUE)
        {
            //----------------
            #if 0
            opRet = ty_light_driver_set_rgbcw(tLightCtrlHandle.tTargetVal.usRed, tLightCtrlHandle.tTargetVal.usGreen, tLightCtrlHandle.tTargetVal.usBlue, \
                                                tLightCtrlHandle.tTargetVal.usWhite, tLightCtrlHandle.tTargetVal.usWarm);
            #endif
			ty_light_gradual_startblink();
            //if(opRet != LIGHT_OK) {
               // TY_LOG_ERR("Light ctrl turn on set RGBCW error!");
               // return opRet;
           // }
		}
		else
	    {
            opRet = ty_light_driver_set_rgbcw(0,0,0,0,0);
            if(opRet != LIGHT_OK) {
                TY_LOG_ERR("Light ctrl turn on set RGBCW error!");
                return opRet;
            }
		}

	}

	ucCnt++;

    if(ucCnt>=4)
    {
       ucCnt=0;
        opRet = ty_light_basis_sw_timer_stop(FAVOTIE_SAVE_TIMER);
        if(opRet != LIGHT_OK) {
            TY_LOG_ERR("favorite timer stop error!");
        }
		

		if(app_light_ctrl_data_mode_get_value() == SCENE_MODE)
        {	
            if(app_light_ctrl_data_switch_get() == TRUE){
                ty_light_scene_ctrl_firstset(FALSE);
                app_light_fav_ctrl_data_mode_set(SCENE_MODE);
            }
		}
        else{
		    app_light_ctrl_proc();
       }
	}


    return LIGHT_OK;
}

/**
 * @brief: start favorite function
 * @param {none} 
 * @attention: breath display will as the parm 
 *             -- NetColor, usNetBright, usNetTemper in configuration.
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_ctrl_favorite_start(void)
{
    OPERATE_LIGHT opRet = 1;
    
    opRet = ty_light_basis_sw_timer_start(FAVOTIE_SAVE_TIMER, 500, (void*)app_light_ctrl_favorite);
    if(opRet != LIGHT_OK) {
        TY_LOG_ERR("favorite timer start error!");
        return LIGHT_COM_ERROR;
    }
    
    return LIGHT_OK;
}
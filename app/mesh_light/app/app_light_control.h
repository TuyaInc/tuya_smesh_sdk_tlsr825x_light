/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: app light_control.h
 * @Description: app light control include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-26 13:55:40
 * @LastEditTime: 2019-10-18 10:53:39
 */

#ifndef __APP_LIHGT_CONTROL_H__
#define __APP_LIHGT_CONTROL_H__


#include "ty_light_basis_types.h"
#include "ty_light_gradual.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


/**
 * @brief Light control data structure
 * storage the light control data(normal issued by app)
 */
#pragma pcak(1)
typedef struct 
{
    bool bSwitch;             /* on off setting */
    LIGHT_MODE_E eMode;
    u16 usBright;
    u16 usTemper;
    COLOR_RGB_T tColor;
    COLOR_ORIGIN_T tColorOrigin;
    u32 uiCountDown;
    char cRealTimeData[22];
    u8 ucRealTimeFlag;
}LIGHT_CTRL_DATA_T;
#pragma pcak()
/**
 * @brief Light control real time control change mode enum
 */
typedef enum {
    REALTIME_CHANGE_JUMP = 0,
    REALTIME_CHANGE_SHADE,
}REALTIME_CHANGE_E;

extern LIGHT_CTRL_DATA_T tLightCtrlData;

/**
 * @brief: Light control proc
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_ctrl_proc(void);

/**
 * @brief: light ctrl disable
 * @param {none}
 * @retval: none
 */
void app_light_ctrl_disable(void);

/**
 * @brief: shade process disable
 * @attention: 
 * @retval: none
 */
void app_light_shade_ctrl_disable(void);

/**
 * @brief: Light realtime ctrl process
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_real_time_ctrl_proc(void);

/**
 * @brief: directly control light off
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
void app_light_off_direct(void);

/**
 * @brief: directly control light on
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
void app_light_on_direct(void);

/**
 * @brief: ADD_QUE
 * @param 
 * @retval: 
 */
void app_show_tuya_ota_result(int result);

/**
 * @brief: ADD_QUE
 * @param 
 * @retval: 
 */
void app_entry_tuya_ota_mode(void);

/**
 * @brief: light control in loop
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
void app_light_ctrl_loop(void);

/**
 * @brief: enable light control gradually
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
void app_light_ctrl_gradual_enable(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_CONTROL_H__ */

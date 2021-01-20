/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: ty_light_gradual.h
 * @Description: light gradual include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-26 13:55:40
 * @LastEditTime: 2019-10-18 10:53:39
 */

#ifndef __TY_LIHGT_GRADUAL_H__
#define __TY_LIHGT_GRADUAL_H__


#include "ty_light_basis_types.h"
#include "ty_light_gradual_max_power.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/// hardware timer cycle (unit:us)
#define HW_TIMER_CYCLE_US           10000   //十分重要，轻易不要改动

/// light shade change cycle (unit:ms) enable to change----10ms
#define LIGHT_SHADE_CYCLE           (HW_TIMER_CYCLE_US/1000)

/// shade change max time(the worst situation),uint:ms @attention: don't change arbitrary
#define SHADE_CHANG_MAX_TIME        800

typedef enum
{
    CW_MODE = 0,
    RGB_MODE,
    RGBCW_MODE,
}GRADUAL_DATA_MODE_E;
#pragma pack(1)
/**
 * @brief Light control color(RGB) data structure
 * storage the light control color(RGB) data(normal issued by app)
 */
typedef struct 
{
    u16 usRed;         /* color R setting */
    u16 usGreen;
    u16 usBlue;
}COLOR_RGB_T;

/**
 * @brief Light control color control original data structure
 */
typedef struct {
    u16 usHue;     /* hue, range:0 ~ 360 */
    u16 usSat;     /* saturation, range:0 ~ 1000 */
    u16 usValue;   /* value, range:0 ~ 1000 */
    char cColorStr[13];
}COLOR_ORIGIN_T;
#pragma pack()
typedef enum
{
    JUMP_MODE = 0,
    NORMAL_MODE = 1,
    PERCENT_MODE = 2,
    TIME_MODE = 3,
    STEP_MODE = 4,
}GRADUAL_MODE_E;
#pragma pack(1)
typedef struct 
{
    GRADUAL_MODE_E eMode;
    u16 usData;
}GRADUAL_CTRL_DATA_T;
#pragma pack()
/**
 * @brief: CW bright output limit process
 * @param {IN u16 usBright -> bright value} 
 * @retval: u16 usResult
 */
u16 ty_light_gradual_ctrl_calc_bt_limit(IN u16 usBright);

/**
 * @brief: calculate the CW Light_Handle.TargetVal according to
 *          the Light_Data value, and cw output limit and amplify process!
 * @param {IN u16 usBright -> Light_Data bright }
 * @param {IN u16 usTemperature -> Light_Data temperature}
 * @param {IN BRIGHT_DATA_T *pResult -> calc result}
 * @retval: none
 */
void ty_light_gradual_ctrl_calc_cw(IN u16 usBright, IN u16 usTemperatue, OUT BRIGHT_DATA_T *pResult);

/**
 * @brief: calculate HSV limit v.
 * @param {IN u16 usValue -> HSV, value}
 * @retval: limit adjust value
 */
u16 ty_light_gradual_ctrl_calc_hsv_limit(IN u16 usValue);

/**
 * @brief: calculate RGB gamma,format:0~255
 * @param {IN CTRL_DEF_COLOR_E  eColor -> R\G\B}
 * @param {IN u16 usGamma -> before gamma param}
 * @attention: 
 * @retval: none
 */
u16 ty_light_gradual_ctrl_calc_gamma(IN CTRL_DEF_COLOR_E  eColor, IN u16 usGamma);

/**
 * @brief: calculate the RGB tLight_Handle.tTargetVal according to
 *          the tLight_Data value.
 * @param {IN COLOR_RGB_T *pColor -> RGB ctrl data}
 * @param {IN BRIGHT_DATA_T *pResult -> Result handle data}
 * @attention: gamma adjust proc
 * @retval: none
 */
void ty_light_gradual_ctrl_calc_rgb(IN COLOR_RGB_T *pColor, OUT BRIGHT_DATA_T *pResult);

/**
 * @brief: calc def bright temper and color, according to param
 * @param {IN CTRL_DEF_COLOR_E eColor -> default color}
 * @param {IN u16 usBright -> default bright}
 * @param {IN u16 usTemper -> default temperature}
 * @param {OUT BRIGHT_DATA_T *pCtrlResult -> calc result}
 * @attention: none
 * @retval: OPERATE_LIGHT
 */
void ty_light_gradual_ctrl_defbc_calc(IN CTRL_DEF_COLOR_E eColor, IN u16 usBright, IN u16 usTemper, OUT BRIGHT_DATA_T *pCtrlResult);

/**
 * @brief: Light control shade change start
 * @param {u32 uiShadeTimeMS -> Shade period,unit:ms} 
 * @retval: none
 */
void ty_light_gradual_ctrl_start(IN GRADUAL_CTRL_DATA_T *GradualData);

/**
 * @brief: Light control shade change stop
 * @param {none} 
 * @retval: none
 */
void ty_light_gradual_ctrl_stop(void);

/**
 * @brief: Light control shade change timer
 * @param {none} 
 * @retval: none
 */
void ty_light_gradual_hw_timer_handler(void);

/**
 * @brief: target light value set for light change
 * @param {IN GRADUAL_DATA_MODE_E eMode}
                                    typedef enum
                                    {
                                        CW_MODE = 0,
                                        RGB_MODE,
                                        RGBCW_MODE,
                                    }GRADUAL_DATA_MODE_E;
 * @param {IN BRIGHT_DATA_T *TargetData}
                                    typedef struct 
                                    {
                                        u16 usRed;
                                        u16 usGreen;
                                        u16 usBlue;
                                        u16 usWhite;
                                        u16 usWarm;
                                    }BRIGHT_DATA_T;//range:0~20000
 * @retval: none
 */
void ty_light_gradual_ctrl_targetset(IN GRADUAL_DATA_MODE_E eMode, IN BRIGHT_DATA_T *TargetData);

/**
 * @brief: get current light value
 * @param {OUT BRIGHT_DATA_T *pCurrData}
                                    typedef struct 
                                    {
                                        u16 usRed;
                                        u16 usGreen;
                                        u16 usBlue;
                                        u16 usWhite;
                                        u16 usWarm;
                                    }BRIGHT_DATA_T;//range:0~20000
 * @retval: none
 */
void ty_light_gradual_ctrl_currentget(OUT BRIGHT_DATA_T *pCurrData);

/**
 * @brief: set current light value
 * @param {IN GRADUAL_DATA_MODE_E eMode}
                                    typedef enum
                                    {
                                        CW_MODE = 0,
                                        RGB_MODE,
                                        RGBCW_MODE,
                                    }GRADUAL_DATA_MODE_E;
 * @param {IN BRIGHT_DATA_T *CurrData}
                                    typedef struct 
                                    {
                                        u16 usRed;
                                        u16 usGreen;
                                        u16 usBlue;
                                        u16 usWhite;
                                        u16 usWarm;
                                    }BRIGHT_DATA_T;//range:0~20000
 * @retval: none
 */
void ty_light_gradual_ctrl_currentset(IN GRADUAL_DATA_MODE_E eMode,IN BRIGHT_DATA_T *CurrData);

/**
 * @brief: set light gradually control onoff state
 * @param {IN bool onoff} 1 -> on; 0 -> off
 * @retval: none
 */
void ty_light_gradual_ctrl_onoffset(IN bool onoff);

/**
 * @brief: get change gradually process the max error of 5ways
 * @param {none}
 * @retval: none
 */
u16 ty_light_gradual_get_difmax(void);
/**
 * @brief: start blink for get laight state
 * @param {none}
 * @retval: none
 */
void ty_light_gradual_startblink(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_CONTROL_H__ */

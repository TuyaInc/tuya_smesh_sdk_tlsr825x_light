/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: ty_light_driver_user_pwm.h
 * @Description: pwm send out include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-22 20:01:00
 * @LastEditTime: 2019-08-13 11:23:10
 */


#ifndef __TY_LIGHT_DRIVER_USER_PWM_H__
#define __TY_LIGHT_DRIVER_USER_PWM_H__


#include "ty_light_basis_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
#pragma pack(1)
/**
 * @brief pwm init data structure
 * pwm send data structure
 */
typedef struct 
{
    u16 usFreq;            ///< PWM Frequency
    u16 usDuty;            ///< PWM Init duty
    u8 ucList[5];          ///< GPIO List 
    u8 ucChannelNum;       ///< GPIO List length
    bool bPolarity;           ///< PWM output polarity
    u8 ucCtrlPin;          ///< CTRL pin parm
    bool  bCtrlLevel;         ///< Enable level
    bool  bCCTFlag;           ///< CCT drive mode flag
}USER_PWM_INIT_T;

/**
 * @brief pwm send data structure
 * pwm send data structure
 */
typedef struct 
{
    u16 usRed;         ///< R value,rang from 0 to 20000
    u16 usGreen;       ///< G value,rang from 0 to 20000
    u16 usBlue;        ///< B value,rang from 0 to 20000
    u16 usCold;        ///< C value,rang from 0 to 20000
    u16 usWarm;        ///< W value,rang from 0 to 20000
}USER_PWM_COLOR_T;
#pragma pack()
/**
 * @brief: user pwm init
 * @param {IN USER_PWM_INIT_T *pPwmInit -> Pwm INIT Parm
 *          usFreq -> PWM Frequency unit Hz, range:100~20000Hz
 *          usDuty -> PWM Init duty unit 0.1%, range: 0~20000 
 *          ucList[5] -> RGBCW GPIO list
 *          ucChannelNum -> channel num, rang 1~5 way
 *          bPolarity -> PWM output polarity
 *              TRUE -> positive
 *              FAlse -> negative
 *          ucCtrlPin -> ctrl pin
 *          bCtrlLevel -> ctrl pin level}
 * @attention ucList[5] parm set
 *              List order is always RGBCW !
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_driver_user_pwm_init(IN USER_PWM_INIT_T *pPwmInit);

/**
 * @brief: pwm send data out
 * @param {IN user_pwm_color_t *color_data -> PWM send data 
 *          usRed   -> R send duty,rang from 0 to 20000,unit 0.005%
 *          usGreen -> G send duty,rang from 0 to 20000,unit 0.005%
 *          usBlue  -> B send duty,rang from 0 to 20000,unit 0.005%
 *          usCold  -> C send duty,rang from 0 to 20000,unit 0.005%
 *          usWarm  -> W send duty,rang from 0 to 20000,unit 0.005%}
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_driver_user_pwm_set_rgbcw(IN USER_PWM_COLOR_T *pColorData);



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __USER_PWM_H__ */

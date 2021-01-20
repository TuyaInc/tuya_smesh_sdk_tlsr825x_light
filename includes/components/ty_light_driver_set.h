/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors:   
 * @file name: ty_light_driver_set.h
 * @Description: light set color(choose drive mode)include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-26 13:55:40
 * @LastEditTime: 2019-05-30 21:35:03
 */

#ifndef __TY_LIHGT_DRIVER_SET_H__
#define __TY_LIHGT_DRIVER_SET_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "ty_light_basis_types.h"

/**
 * @brief: Light drive init!
 * @param {DRIVER_CONFIG_T *pLightConfig -> init config structure}
 * @attention: DRIVER_MODE_E eMode -> has three mode：
                DRIVER_MODE_PWM = 0
                DRIVER_MODE_SM2135 = 1
                DRIVER_MODE_SM16726B = 2
                DRIVER_MODE_SM2135EH = 3
 * @attention: PWM_CONFIG_T tPwmInit
                u16 usFreq;                    ///< PWM Frequency
                u16 usDuty;                    ///< PWM Init duty
                u8  ucList[5];                 ///< GPIO List 
                u8  ucChannelNum;              ///< GPIO List length
                bool   bPolarity;                 ///< PWM output polarity
                u8  ucCtrlPin;                 ///< CTRL pin parm
                bool   bCtrlLevel;                ///< Enable level
* @attention: SM2135_CONFIG_T tSm2135Init
                u8 ucNumber;                   ///< output channel number
                u8 ucSdaPin;                   ///< SDA pin
                u8 ucSclPin;                   ///< SCL pin
                u8 ucList[5];                  ///< RGBCW output channel index config 
                u8 ucRGBCurrent;               ///< RGB mode max current settings
                u8 ucCWCurrent;                ///< CW mode max current settings
                u8 ucCtrlPin;                  ///< CTRL pin parm
                bool  bCtrlLevel;                 ///< Enable level
* @attention: SM16726B_CONFIG_T tSm16726bInit
                u8 ucSdaPin;                   ///< SDA pin
                u8 ucSclPin;                   ///< SCL pin
                u8 ucList[3];                  ///< R, G, B index of SM16726 output pin
                u8 ucCtrlPin;                  ///< CTRL pin parm
                bool  bCtrlLevel;                 ///< Enable level
                u8 ucChannelNum;               ///< Light channel: 3/4/5
                SM16726B_PWM_CONFIG_T tPwmInit;     ///< Channel C,W config
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_driver_set_init(IN DRIVER_CONFIG_T *pLightConfig);


/**
 * @brief: light send control data
 * @param {IN u16 usRed} red color, range 0~20000
 * @param {IN u16 usGreen} Green color, range 0~20000
 * @param {IN u16 usBlue} Blue color, range 0~20000
 * @param {IN u16 usCold} cold white color, range 0~20000
 * @param {IN u16 usWarm} warm white color, range 0~20000
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_driver_set_rgbcw(IN u16 usRed, IN u16 usGreen, IN u16 usBlue, IN u16 usCold, IN u16 usWarm);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_SET_COLOR_H__ */

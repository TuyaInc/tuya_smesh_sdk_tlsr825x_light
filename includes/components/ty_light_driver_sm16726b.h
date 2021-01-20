/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: ty_light_driver_sm16726b.h
 * @Description: SM16726B IIC driver include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-17 20:49:35
 * @LastEditTime: 2019-08-13 14:46:30
 */

#ifndef __TY_LIGHT_DRIVER_SM16726B_H__
#define __TY_LIGHT_DRIVER_SM16726B_H__

#include "ty_light_basis_types.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
#pragma pack(1)
/**
 * @brief sm16726b cold & warm pwm structure
 */
typedef struct {
    u16 usFreq;            ///< PWM Frequency
    u16 usDuty;            ///< PWM Init duty
    u8 ucColdPin;
    u8 ucWarmPin;          
    bool bPolarity;           ///< PWM output polarity
    bool bCCTFlag;            ///< PWM drive CCT flag
}SM16726B_PWM_INIT_T;

/**
 * @brief sm16726b init structure
 * sm16726b init gpio structure
 */
typedef struct 
{
    u8 ucSdaPin;                   ///< SDA pin
    u8 ucSclPin;                   ///< SCL pin
    u8 ucList[3];                  ///< output channel list
    u8 ucCtrlPin;                  ///< CTRL pin parm
    bool  bCtrlLevel;                 ///< Enable level
    u8 ucChannelNum;               ///< Light channel: 3/4/5
    SM16726B_PWM_INIT_T tPwmInit;       ///< Channel C,W config
}SM16726B_INIT_T;

/**
 * @brief sm16726b set color param structure
 */
typedef struct
{
    u16 usRed;         ///< R value,rang from 0 to 255
    u16 usGreen;       ///< G value,rang from 0 to 255
    u16 usBlue;        ///< B value,rang from 0 to 255
    u16 usCold;        ///< C value,rang from 0 to 1000
    u16 usWarm;        ///< W value,rang from 0 to 1000
}SM16726B_COLOR_T;
#pragma pack()
/**
 * @brief: sm16726B init
 * @param {IN SM16726B_INIT_T *pSm16726bInit --> sm16726b INIT Parm}
 * @param {ucSdaPin -> SDA IO}
 * @param {ucSclPin -> SCL IO}
 * @param {ucList -> RGB channel list}
 * @param {ucCtrlPin -> CTRL IO(actually control power)} 
 * @param {bCtrlLevel -> CTRL pin active voltage} 
 * @param {tPwmInit -> C&W way drive pwm config}
 * @attention: *List parm set -- output channel
 *              RGB defalut output channel is R->0 G->1 B->2
 * @attention bCtrlLevel setting
 *              TRUE    --> HIGH LEVEL effective
 *              FALSE   --> LOW LEVEL effective
 * @attention tPwmInit config 
 *              usDuty range is from 0 to 1000
 * @retval: none
 */
OPERATE_LIGHT ty_light_driver_sm16726b_init(IN SM16726B_INIT_T *pSm16726bInit);

/**
 * @brief: sm16726b send data
 * @param {IN SM16726B_COLOR_T *pColorData -> need write data structure to sm16726B}
 * @attention: 
 *          usRed   -> R value,rang from 0 to 256
 *          usGreen -> G value,rang from 0 to 256
 *          usBlue  -> B value,rang from 0 to 256
 *          usCold  -> C value,range from 0 to 1000
 *          usWarm  -> W value,range from 0 to 1000
 * @retval: LIGHT_INVALID_PARM -> invalid parm
 * @retval: LIGHT_OK ->operate ok
 */
OPERATE_LIGHT ty_light_driver_sm16726b_rgbcw(IN SM16726B_COLOR_T *pColorData);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __SM16726B_H__ */

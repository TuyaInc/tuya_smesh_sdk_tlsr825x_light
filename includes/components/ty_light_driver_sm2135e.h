/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: ty_light_driver_sm2135e.h
 * @Description: sm2135 IIC drive include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-16 15:11:22
 * @LastEditTime: 2019-10-18 15:24:39
 */

#ifndef __TY_LIGHT_DRIVER_SM2135E_H__
#define __TY_LIGHT_DRIVER_SM2135E_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
#include "ty_light_basis_types.h"

/**
 * @brief sm2135 work mode enum
 * sm2135 work mode 
 */
typedef enum
{
    SM2135_MODE_WHITE = 0,
    SM2135_MODE_RGB ,
}SM2135_MODE_T;

#pragma pack(1)
/**
 * @brief sm2135 init gpio pin structure
 * sm2135 init gpio structure
 */
typedef struct 
{
    u8 ucNumber;            ///< output channel number
    u8 ucSdaPin;            ///< SDA pin
    u8 ucSclPin;            ///< SCL pin
    u8 ucList[5];           ///< RGBCW output channel index config 
    u8 ucRGBCurrent;        ///< RGB mode max current settings
    u8 ucCWCurrent;         ///< CW mode max current settings
    u8 ucCtrlPin;           ///< CTRL pin parm
    bool  bCtrlLevel;          ///< Enable level
}SM2135_INIT_T;

/**
 * @brief sm2135 send data structure
 * sm2135 send data structure
 */
typedef struct 
{
    u16 usRed;         ///< R value,rang from 0 to 256
    u16 usGreen;       ///< G value,rang from 0 to 256
    u16 usBlue;        ///< B value,rang from 0 to 256
    u16 usCold;        ///< C value,rang from 0 to 256
    u16 usWarm;        ///< W value,rang from 0 to 256
    u8 ucMode;         ///< output mode
}SM2135_COLOR_T;
#pragma pack()
/**
 * @brief: sm3125 init
 * @param {IN SM2135_INIT_T pSm2135Init -> INIT Parm
 *          u8 ucSdaPin -> SDA IO
 *          u8 ucSclPin -> SCL IO
 *          u8 uclist[5] -> RGBCW output channel config list
 *          u8 ucRGBCurrent -> RGB mode max current settings
 *          u8 ucCWCurrent -> CW mode max current settings}
 * @attention ucList[5] parm set
 *              RGB configurable output channel is 0~2
 *              CW configurable output channel is 3~4
* @attention RGBCurrent parm set
 *              RGBCurrent parm configurable max current <= 45mA
 * @attention CWCurrent parm set
 *               CWCurrent parm configurable max current <= 80mA
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_driver_sm2135e_init(IN SM2135_INIT_T* pSm2135Init);

/**
 * @berief: sm2135 send data
 * @param {IN SM2135_SendData -> need write data structure to sm2135
 *          R_Value -> R value,rang from 0 to 256
 *          G_Value -> G value,rang from 0 to 256
 *          B_Value -> B value,rang from 0 to 256
 *          C_Value -> C value,rang from 0 to 256
 *          W_Value -> W value,rang from 0 to 256
 *          Mode -> output mode}
 * @attention sm2135 send mode must be SM2135_White_Mode or SM2135_RGB_Mode 
 *              at same moment 
 * @return: OPERATE_LIGHT
 * @retval: LIGHT_INVALID_PARM -> invalid parm
 * @retval: LIGHT_COM_ERROR  -> send data error
 */
OPERATE_LIGHT ty_light_driver_sm2135e_rgbcw(IN SM2135_COLOR_T *pColorData);




#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __SM2135_H__ */

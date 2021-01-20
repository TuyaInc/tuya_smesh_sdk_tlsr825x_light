/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors:   
 * @file name: ty_light_save_3in1_soc_flash.h
 * @Description: soc flash include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-05-06 10:00:26
 * @LastEditTime: 2019-10-18 10:55:03
 */

#ifndef __TY_LIGHT_SAVE_3IN1_SOC_FLASH_H__
#define __TY_LIGHT_SAVE_3IN1_SOC_FLASH_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "ty_light_basis_types.h"

/**
 * @brief: soc data save
 * @param {IN SOC_FLASH_SAVE_TYPE_E eDataType -> save type(meaning data kind)}
 * @param {IN u32 uiAddr -> this type data address offset}
 * @param {IN u8 *pData -> save data}
 * @param {IN u16 usLen -> save data len}
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_save_3in1_soc_flash_write(IN u32 uiAddr, IN u8 *pData, IN u16 usLen);

/**
 * @brief: soc flash save data read
 * @param {IN SOC_FLASH_SAVE_TYPE_E eDataType -> read data type(meaning data kind)}
 * @param {IN u32 uiAddr -> this type data address offset}
 * @param {IN u16 ucLen -> read data len}
 * @param {OUT u8 *pData -> read data}
 * @retval: read data cnt
 */
OPERATE_LIGHT ty_light_save_3in1_soc_flash_read(IN u32 uiAddr, IN u16 usLen, OUT u8 *pData);

/**
 * @brief: soc flash erase all
 * @param {none}
 * @retval: OPERATE_RET
 */
OPERATE_LIGHT ty_light_save_3in1_soc_flash_erase_sector(u32 addr);

/**
 * @berief: read app data from flash
 * @param {none}
 * @return: OPERATE_LIGHT
 * @retval: none
 */
OPERATE_LIGHT ty_light_save_3in1_soc_flash_open_appdata(void);

/**
 * @berief: write app data in flash
 * @param {none}
 * @return: OPERATE_LIGHT
 * @retval: none
 */
OPERATE_LIGHT ty_light_save_3in1_soc_flash_close_appdata(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __SOC_FLASH_H__ */

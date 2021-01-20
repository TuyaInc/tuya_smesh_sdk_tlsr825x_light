/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: app_light_prompt.h
 * @Description: app light prompt include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-26 13:55:40
 * @LastEditTime: 2019-10-18 10:53:39
 */

#ifndef __APP_LIHGT_PROMPT_H__
#define __APP_LIHGT_PROMPT_H__


#include "ty_light_basis_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief: when distribution network, select indicator light indication mode
 * @param {type} 
 * @retval: none
 */
void app_light_ctrl_prompt_start(void);

/**
 * @brief: after the model in the network, the lights turn off
 * @param {type} 
 * @retval: none
 */
void app_light_ctrl_prompt_stop(void);

/**
 * @brief: start favorite function
 * @param {none} 
 * @attention: breath display will as the parm 
 *             -- NetColor, usNetBright, usNetTemper in configuration.
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_ctrl_favorite_start(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_CONTROL_H__ */

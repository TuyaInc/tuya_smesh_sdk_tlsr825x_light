/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: ty_light_scene.h
 * @Description: light scene include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-26 13:55:40
 * @LastEditTime: 2019-10-18 10:53:39
 */

#ifndef __TY_LIHGT_REMOTE_H__
#define __TY_LIHGT_REMOTE_H__


#include "ty_light_basis_types.h"

enum{
    VD_GROUP_ADDR_SUB = 0x81,
    VD_REMOTE_LIGHTNESS = 0x83,
    VD_FAVORITE_SAVE_CMD = 0x87,
};
enum{
	FAVORITE_DATA_SAVE = 0x01,
	FAVORITE_DATA_ACTION = 0x02,
};
/**
 * @brief: remote init
 * @param {none}
 * @return: none
 */
void ty_light_remote_init(void);

/**
 * @brief: handle remote dp data
 * @param {u8 * par} dp data
 * @param {int par_len}
 * @return: none
 */
void ty_light_remote_dp_data(u8 * par, int par_len);
/**
 * @brief: remind remote join network success 
 * @param {u8 * par} dp data
 * @param {int par_len}
 * @return: none
 */
void ty_light_join_network_success(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_CONTROL_H__ */

/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: ty_light_cfg_init.h
 * @Description: light init process include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-05-09 10:49:13
 * @LastEditTime: 2019-05-15 16:54:29
 */

#ifndef __TY_LIHGT_CFG_INIT_H__
#define __TY_LIHGT_CFG_INIT_H__


#include "ty_light_basis_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief Light way type enum 
 *          1~5 ways
 */
typedef enum {
    LIGHT_C = 1,
    LIGHT_CW,
    LIGHT_RGB,
    LIGHT_RGBC,
    LIGHT_RGBCW,
    LIGHT_MAX,
}CTRL_LIGHT_WAY_E;


typedef enum
{
    DEFA_MODE = 0,
    PMEM_MODE,
    USER_MODE,
    MAX_MODE,
}LIGHT_PMEM_E;
#pragma pack(1)
/**
 * @brief Light bMemory Customized
 * the configuration need to set firstly ,when use the control frame! 
 */
typedef struct 
{
    bool   usFlag;
    u8     usMemory;
    u16    usHue;
    u16    usSat;
    u16    usValue;
    u16    usBright;
    u16    usTemper;
}LIGHT_MEM_CFG_T;
#pragma pack()
extern LIGHT_MEM_CFG_T tLightMemCfgData;

/**
 * @brief: get connect mode cfg
 * @param {none}
 * @retval: u8 connect mode
 */
u8 ty_light_cfg_init_get_connect_mode(void);

/**
 * @brief: get color max limit
 * @param {none}
 * @retval: u8
 */
u8 ty_light_cfg_init_get_colormax(void);

/**
 * @brief: get color min limit
 * @param {none}
 * @retval: u8
 */
u8 ty_light_cfg_init_get_colormin(void);

/**
 * @brief: get brightmode
 * @param {none}
 * @retval: CTRL_BRIGHT_MODE_E
 */
CTRL_BRIGHT_MODE_E ty_light_cfg_init_get_brightmode(void);


/**
 * @brief: get lightway
 * @param {none}
 * @retval: CTRL_LIGHT_WAY_E
 */
CTRL_LIGHT_WAY_E ty_light_cfg_init_get_lightway(void);

/**
 * @brief: get switchmode
 * @param {none}
 * @retval: CTRL_SWITCH_MODE_E
 */
CTRL_SWITCH_MODE_E ty_light_cfg_init_get_switchmode(void);

/**
 * @brief: get Memory flag
 * @param {none}
 * @retval: bool
 */
bool ty_light_cfg_init_get_bMemory(void);

/**
 * @brief: get cold and warm light min value
 * @param {none}
 * @retval: min light value
 */
u8 ty_light_cfg_init_get_cwmin(void);

/**
 * @brief: get cold and warm light max value
 * @param {none}
 * @retval: max light value
 */
u8 ty_light_cfg_init_get_cwmax(void);

/**
 * @brief: the color of distribution network
 * @param {none}
 * @retval: CTRL_DEF_COLOR_E
 */
CTRL_DEF_COLOR_E ty_light_cfg_init_get_netcolor(void);

/**
 * @brief: the color of distribution network success
 * @param {none}
 * @retval: CTRL_DEF_COLOR_E
 */
CTRL_DEF_COLOR_E ty_light_cfg_init_get_defcolor(void);

/**
 * @brief: the bright of distribution network success
 * @param {none}
 * @retval: u16
 */
u16 ty_light_cfg_init_get_defbright(void);

/**
 * @brief: the temper of distribution network success
 * @param {none}
 * @retval: u16
 */
u16 ty_light_cfg_init_get_deftemper(void);

/**
 * @brief: the bright of distribution network
 * @param {none}
 * @retval: u16
 */
u16 ty_light_cfg_init_get_netbright(void);

/**
 * @brief: the temper of distribution network
 * @param {none}
 * @retval: u16
 */
u16 ty_light_cfg_init_get_nettemper(void);

/**
 * @brief: power amplification and reduction
 * @param {none}
 * @retval: u8
 */
u8 ty_light_cfg_init_get_cw_powergain(void);

/**
 * @brief: power amplification and reduction
 * @param {none}
 * @retval: u8
 */
u8 ty_light_cfg_init_get_rgb_powergain(void);
/**
 * @brief: set light ctrl data to default according lightCfgData
 * @param {none} 
 * @retval: none
 */
void ty_light_cfg_init_ctrl_data_reset(void);

/**
 * @brief: light hardware init
 *          get oem json set, and init hardware
 * @param {none} 
 * @return: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_cfg_init_light_params_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_INIT_H__ */

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

#ifndef __TY_LIHGT_SCENE_H__
#define __TY_LIHGT_SCENE_H__


#include "ty_light_basis_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

enum{
    VD_GROUP_ADDR_INQUIRE = 0x82,
	VD_SCENE_SYNC_CMD = 0x84,
};

/// scene head(scene Num) length
#define SCNE_HRAD_LENGTH            2
/// scene unit length
#define SCENE_UNIT_LENGTH           26
/// scene max unit number
#define SCENE_MAX_UNIT              8
/// scene data min length, formu -> 2+ 26 = 28
#define SCENE_MIN_LENGTH            (SCNE_HRAD_LENGTH + SCENE_UNIT_LENGTH)
/// scene data max length, formu -> 2 + 26*8  = 210
#define SCENE_MAX_LENGTH            (SCNE_HRAD_LENGTH + SCENE_UNIT_LENGTH * SCENE_MAX_UNIT)


#define FRIST_DEVICE_SCENE_SYNC_TIME_DELAY 30*1000
#define SECOEND_DEVICE_SCENE_SYNC_TIME_DELAY 300*1000


/**
 * @brief Light control scene change mode enum 
 *          SCENE_STATIC    -> scene hold on no change 
 *          SCENE_JUMP      -> scene change by directly
 *          SCENE_SHADOW    -> scene change by gradually 
 */
#pragma pack(1)
typedef enum {
    SCENE_STATIC = 0,
    SCENE_JUMP,
    SCENE_SHADE,
    SCENE_MAX,
}CTRL_SCENE_MODE_E;
#pragma pack()

#pragma pack(1)
typedef struct 
{
    CTRL_SCENE_MODE_E ChangeMode;
    u32 uiSpeed;
    u32 uiTimes;    /* unit:ms */
}LIGHT_SCENE_CTRL_T;
#pragma pack()

/**
 * @brief Light control data structure
 * storage the light control data(normal issued by app)
 */
#pragma pack(1)
typedef struct 
{
    char cScene[SCENE_MAX_LENGTH + 1];
    bool bSceneFirstSet;
}LIGHT_CTRL_SCENE_DATA_T;
#pragma pack()
//---------------------
extern LIGHT_CTRL_SCENE_DATA_T tLightCtrlSceneData;





/**
 * @description: get scene unit
 * @param {none}
 * @return: scene unit
 */
u8 ty_light_scene_ctrl_get_unit(void);

/**
 * @description: get scene num
 * @param {none}
 * @return: scene unit num
 */
u8 ty_light_scene_ctrl_get_scenenum(void);

/**
 * @description: get scene command data
 * @param {u8 *data} scene command data
 * @param {u8 *data_len} 
 * @return: none
 */
void ty_light_scene_cmd_dataget(u8 *data, u8 *data_len);

/**
 * @description:set scene synchronize group address
 * @param {IN u16 usGroupId} group address
 * @return: none
 */
void ty_light_scene_cmd_set_groupid(IN u16 usGroupId);

/**
 * @description:get scene synchronize group address
 * @param {none} 
 * @return:group address / 0xFFFF
 */
u16 ty_light_scene_cmd_get_groupid(void);

/**
 * @description:get scene change time
 * @param {IN u8 usTTL} transmit times
 * @return: none
 */
void ty_light_scene_cmd_set_ttl(IN u8 usTTL);

/**
 * @description: control scene change time synchronize
 * @param {uint_num} scene uint num
 * @return: none
 */
void ty_light_scene_cmd_sync(u8 uint_num);

/**
 * @description: handle light scene data 
 * @param {u8 * par} scene data
 * @param {int par_len}
 * @return: none
 */
void ty_light_scene_cmd_data_set(u8 * par, int par_len);

/**
 * @brief: Light scene change stop
 * @param {none}
 * @retval: none
 */
void ty_light_scene_ctrl_change_stop(void);

/**
 * @brief: scene change proc
 *          get scene ctrl data and change to RGBCW value form,
 *          start to scene unitX(X:1~8);
 * @param {OUT bool *pCycleEnable -> need to cycle}
 * @param {OUT u32 *pCycleTime_ms -> unit change time, ms}
 * @attention: this func need to called period, period time is transfered by pCycletime,
 *              when pCycleEnable flag equal to TRUE;
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_scene_ctrl_change(OUT bool *pCycleEnable, OUT u32 *pCycleTimeMs);

/**
 * @brief: Light scenen change start
 * @param {IN u32 uiChangeTimeMs -> change time, unit:ms}
 * @retval: none
 */
void ty_light_scene_ctrl_change_start(IN u32 uiChangeTimeMs);

/**
 * @brief: light HSVBT data analysize
 * @param {IN char *pHSVBT -> HSVBT string } 
 * @param {OUT *pValH -> Hue}
 * @param {OUT *pValS -> Sat}
 * @param {OUT *pValV -> Value}
 * @param {OUT *pValB -> Brightness}
 * @param {OUT *pValT -> Temperature}
 * @retval: none
 */
void ty_light_scene_hsvbt_analysize(IN char *pHSVBT, OUT u16 *pValH, OUT u16 *pValS, OUT u16 *pValV, OUT u16 *pValB, OUT u16 *pValT);

/**
 * @description: start light scene, set flag
 * @param {none}
 * @return: none
 */
void ty_light_scene_ctrl_start(void);

/**
 * @description: take a start from the head to start light scene, set flag
 * @param {bool bFirstSet} 1 -> take a start from the head; 0 -> continue from last time
 * @return: none
 */
void ty_light_scene_ctrl_firstset(bool bFirstSet);

/**
 * @description: scene data init
 * @param {u8 *cScene} scene data
 * @param {u8 cSecne_len}
 * @return: none
 */
void ty_light_scene_init_dataset(u8 *cScene, u8 cSecne_len);

/**
 * @brief: get light scene data
 * @param {OUT char *pSceneData -> scene data return} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_scene_ctrl_get_scenedata(OUT char *pSceneData);

/**
 * @description: scene id data init
 * @param {u8 *cScene} scene id
 * @return: none
 */
void ty_light_scene_init_id_dataset(u8 cSecne_id);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_CONTROL_H__ */

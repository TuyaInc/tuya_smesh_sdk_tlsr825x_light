/********************************************************************************************************
 * @file     ty_scene.h 
 *
 * @brief    for TLSR chips
 *
 * @author   telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *           The information contained herein is confidential and proprietary property of Telink 
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *           Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this 
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/

#pragma once

#include "tuya_element_cfg.h"

//----------------------------------- op code
// op cmd 0xxxxxxx (SIG)

// op cmd 10xxxxxx xxxxxxxx (SIG)
#define SCENE_GET                       0x4182
#define SCENE_RECALL                    0x4282
#define SCENE_RECALL_NOACK              0x4382
#define SCENE_STATUS                    0x5E
#define SCENE_REG_GET                   0x4482
#define SCENE_REG_STATUS                0x4582
#define SCENE_STORE                     0x4682
#define SCENE_STORE_NOACK               0x4782
#define SCENE_DEL                       0x9E82
#define SCENE_DEL_NOACK                 0x9F82

//------op parameters

#define SCENE_CNT_MAX   (16)


typedef uint8_t  scene_ret_t;
#define SCENE_ST_SUCCESS         0
#define SCENE_ST_REG_FULL        1
#define SCENE_ST_NOT_FOUND       2


typedef enum{
    TY_SCENE_DELETE = 0x00,
    TY_SCENE_STORE,
}scene_set_op_t;

enum{
    SCENE_ID_INVALID    = 0,
};

typedef struct{
    scene_ret_t st;
    uint16_t current_id;
    uint16_t id[SCENE_CNT_MAX];
}scene_reg_status_t;

typedef struct{
    uint16_t id;
    uint8_t tid;
    uint8_t transit_t;
    uint8_t delay;       // unit 5ms
}scene_recall_t;

typedef struct{
    uint8_t st;
    uint16_t current_id;
    uint16_t target_id;
    uint8_t remain_t;
}scene_status_t;

typedef struct{
    uint16_t current_id;
    uint16_t id[SCENE_CNT_MAX];
}tuya_scene_reg_status_t;

typedef scene_ret_t (*scene_reg_get_cb)(tuya_scene_reg_status_t *scene_reg_status);
typedef scene_ret_t (*scene_set_cb)(uint16_t scene_id, scene_set_op_t op);
typedef scene_ret_t (*scene_recall_cb)(uint16_t scene_id);


void tuya_scene_get_init_cb(scene_reg_get_cb cb);
void tuya_scene_set_init_cb(scene_set_cb cb);
void tuya_scene_recall_init_cb(scene_recall_cb cb);



// -----------

#if SCENE_SERVER_EN
int mesh_cmd_sig_scene_reg_get(uint8_t *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_scene_set(uint8_t *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_scene_get(uint8_t *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_scene_recall(uint8_t *par, int par_len, mesh_cb_fun_par_t *cb_par);
#endif

#if SCENE_CLIENT_EN
int mesh_cmd_sig_scene_reg_status(uint8_t *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_scene_status(uint8_t *par, int par_len, mesh_cb_fun_par_t *cb_par);

int tuya_access_cmd_scene_reg_get(uint16_t adr_dst);
int tuya_access_cmd_scene_store(uint16_t adr_dst, uint16_t id, int ack);
int tuya_access_cmd_scene_del(uint16_t adr_dst, uint16_t id, int ack);
int tuya_access_cmd_scene_get(uint16_t adr_dst);
int tuya_access_cmd_scene_recall(uint16_t adr_dst, uint16_t id, int ack);

#endif



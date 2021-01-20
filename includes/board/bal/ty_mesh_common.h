/*
 * ty_mesh_common.h
 *
 *  Created on: 2020-08-11
 *      Author: Joey
 */

#ifndef __TY_MESH_COMMON_H
#define __TY_MESH_COMMON_H

#include "tuya_element_cfg.h"

#ifdef USED_NV1
#define BAL_NV_SAVE(id, buf, len)  NV_USER4_ITEM_SAVE(id, buf, len)
#define BAL_NV_LOAD(id, buf, len)  NV_USER4_ITEM_LOAD(id, buf, len)
#else
#define BAL_NV_SAVE(id, buf, len)  NV_USER1_ITEM_SAVE(id, buf, len)
#define BAL_NV_LOAD(id, buf, len)  NV_USER1_ITEM_LOAD(id, buf, len)
#endif


void tuya_mesh_app_init(void);

void tuya_mesh_main_run(void);

void tuya_mesh_state_callback(mesh_state_t stat);

void tuya_mesh_factory_reset(void);

extern void mesh_app_init(void);
extern void mesh_main_run(void);
extern void mesh_factory_reset(void);
extern void mesh_state_callback(mesh_state_t stat);





#endif

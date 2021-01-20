/*
 * ty_mesh_group.h
 *
 *  Created on: 2020-08-11
 *      Author: Joey
 */

#ifndef __TY_MESH_GROUP_H
#define __TY_MESH_GROUP_H

int group_addr_sub_add(uint16_t mesh_addr, uint16_t group_addr);
int group_addr_sub_del(uint16_t mesh_addr, uint16_t group_addr);
uint16_t* group_addr_sub_list_get();
uint16_t* model_group_addr_sub_list_get(uint16_t ele_idx, uint32_t model_id);



#endif

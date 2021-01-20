/*
 * tuya_light_model.h
 *
 *  Created on: 2020-08-11
 *      Author: Joey
 */
#ifndef PACKED
#define PACKED __attribute__((packed))
#endif
#include "tuya_sigmesh_hal.h"

typedef struct{
    uint16_t present;
    uint16_t target;
    uint8_t remain_t;
}PACKED ty_mesh_cmd_lightness_st_t;
 
    typedef struct{
    uint16_t lightness;
    uint8_t tid;
    uint8_t transit_t;
    uint8_t delay;      // unit 5ms
}PACKED ty_mesh_cmd_lightness_set_t;

typedef struct{
    uint16_t present_lightness;
    uint16_t present_temp;
    uint16_t target_lightness;
    uint16_t target_temp;
    uint8_t remain_t;
}PACKED ty_mesh_cmd_light_ctl_st_t;

typedef struct{
    uint16_t present_temp;
    int16_t present_delta_uv;
    uint16_t target_temp;
    int16_t target_delta_uv;
    uint8_t remain_t;
}PACKED ty_mesh_cmd_light_ctl_temp_st_t;
 
typedef struct{
    uint16_t lightness;     // lightness ctl is same with lightness
    uint16_t temp;          // 0x0320 -- 0x4E20  :800--20000
    int16_t delta_uv;
    uint8_t tid;
    uint8_t transit_t;
    uint8_t delay;      // unit 5ms
}PACKED ty_mesh_cmd_light_ctl_set_t;
 
typedef struct{
    uint16_t temp;
    int16_t delta_uv;
    uint8_t tid;
    uint8_t transit_t;
    uint8_t delay;
}PACKED ty_mesh_cmd_light_ctl_temp_set_t;

typedef struct{
    uint16_t lightness;
    uint16_t hue;
    uint16_t sat;
    uint8_t remain_t;
}PACKED ty_mesh_cmd_light_hsl_st_t;

typedef struct{
    uint16_t lightness;
    uint16_t hue;
    uint16_t sat;
    uint8_t tid;
    uint8_t transit_t;
    uint8_t delay;      // unit 5ms
}PACKED ty_mesh_cmd_light_hsl_set_t;









































/*************************************************************************
	> File Name: ty_light_basis_hw_timer.h
	> Author: jiangjj
	> Mail: 
	> Created Time: Fri 26 jul 2019 10:26:19 CST
 ************************************************************************/
#ifndef __TY_LIGHT_BASIS_HW_TIMER_H__
#define __TY_LIGHT_BASIS_HW_TIMER_H__

#include "ty_light_basis_types.h"

/**
 * @brief: user hardware time start
 * @param {IN u32 uiCycleUs -> hardware timer period, unit:us}
 * @param {IN void* pCallback -> callback proc}
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_basis_hw_timer_start(IN u32 uiCycleUs, IN void* pCallback);

/**
 * @brief: user hardware time stop
 * @param {none}
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_basis_hw_timer_stop(void);

#endif

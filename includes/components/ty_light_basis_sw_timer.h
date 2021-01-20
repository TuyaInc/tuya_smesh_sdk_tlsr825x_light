/*************************************************************************
	> File Name: ty_light_basis_sw_timer.h
	> Author: jiangjj
	> Mail: 
	> Created Time: Fri 26 jul 2019 10:26:19 CST
 ************************************************************************/
#ifndef __TY_LIGHT_BASIS_SW_TIMER_H__
#define __TY_LIGHT_BASIS_SW_TIMER_H__

#include "ty_light_basis_types.h"

//user define
#define 	MAX_TIMER_NUM							20   //timer max number


/**
 * @brief software timer use id enum
 */
typedef enum {
    CLEAR_RESET_CNT_SW_TIMER = 0,
    BLINK_SW_TIMER,
    SCENE_SW_TIMER,
    AUTOSAVE_SW_TIMER,
    BREATH_SW_TIMER,
    COUNTDOWN_SW_TIMER,
    LOWPOWER_SW_TIMER,
    SCENE_AUTO_RESTART_TIMER,
    CCT_DELAY_SHUT_DOWN_TIMER,
    CCT_DELAY_RESET_TIMER,
    SW_TIMER_MAX
}SW_TIMER_ID_E;

#define SCENE_SYNC_TIMER_ID 16
#define SCENE_SYNC_UINT_ID 17
#define FAVOTIE_SAVE_TIMER 18


typedef int (*hal_timer_callback_t)(void);


#pragma pack(1)

typedef struct hal_time_event_t {
    u8                      timeid;
	hal_timer_callback_t    cb;
	u32                     t;
	u32                     interval;
} hal_time_event_t;


// timer table managemnt
typedef struct hal_sw_timer_t {
	hal_time_event_t	timer[MAX_TIMER_NUM];  //timer0 - timer3
	u8					currentNum;  //total valid timer num
} hal_sw_timer_t;
#pragma pack()

/**
 * @brief: user software time start
 * @param {IN u8 ucTimerID -> software timer ID}
 * @param {IN u32 uiCyclyeMs -> timerout time,unit:ms} 
 * @param {IN void* pCallback -> timerout handler} 
 * @attention: soft time must can reload in anytime.cd 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_basis_sw_timer_start(IN u8 ucTimerID, IN u32 uiCycleMs, IN void* pCallback);

/**
 * @brief: user software time stop
 * @param {IN u8 ucTimerID -> software timer_id}
 * @return: OPERATE_LIGHT
 * @retval: none
 */
OPERATE_LIGHT ty_light_basis_sw_timer_stop(IN u8 ucTimerID);

/**
 * @description: check timer if time out
 * @param {IN u8 ucTimerID} software timer ID -> SW_TIMER_ID_E
						typedef enum {
							CLEAR_RESET_CNT_SW_TIMER = 0,
							BLINK_SW_TIMER,
							SCENE_SW_TIMER,
							AUTOSAVE_SW_TIMER,
							BREATH_SW_TIMER,
							COUNTDOWN_SW_TIMER,
							LOWPOWER_SW_TIMER,
							SCENE_AUTO_RESTART_TIMER,
							CCT_DELAY_SHUT_DOWN_TIMER,
							CCT_DELAY_RESET_TIMER,
							SW_TIMER_MAX
						}SW_TIMER_ID_E;
 * @return: LIGHT_OK or LIGHT_INVALID_PARM
 */
OPERATE_LIGHT ty_light_basis_sw_timer_check(IN u8 ucTimerID);

/**
 * @description: handler timer when time out
 * @param {none}
 * @return: none
 */
void ty_light_basis_sw_timer_handler(void);

#endif 

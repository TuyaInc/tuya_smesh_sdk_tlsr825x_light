/*************************************************************************
	> File Name: app_rssi.h
	> Author: 
	> Mail: 
	> Created Time: Wed 27 Mar 2019 14:52:08 CST
 ************************************************************************/

#ifndef _APP_RSSI_H
#define _APP_RSSI_H

#include "board.h"

/**
* @description: rssi init
* @param {none}
* @return: none 
**/
extern void app_rssi_init(void);

/**
* @description: run rssi test
* @param {u8 *adv} ADV packet
* @param {u8 adv_len} ADV packet len
* @param {u8 *mac} mac address
* @param {int rssi} rssi value
* @return: none 
**/
extern void app_rssi_run(u8 *adv, u8 adv_len, u8 *mac, int rssi);



#endif

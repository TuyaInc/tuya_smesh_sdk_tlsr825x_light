/*************************************************************************
	> File Name: app_uart.h
	> Author: 
	> Mail: 
	> Created Time: Tue 26 Feb 2019 22:29:40 CST
 ************************************************************************/

#ifndef _APP_UART_H
#define _APP_UART_H

#include "board.h"

/**
* @description: uart init
* @param {none}
* @return: none
**/
extern void app_uart_init(void);

/**
* @description: uart run in a loop, handle uart data
* @param {none}
* @return: none
**/
extern void app_uart_run(void);

#endif

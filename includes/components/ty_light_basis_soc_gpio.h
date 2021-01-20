/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors:   
 * @file name:  ty_light_basis_soc_gpio.h
 * @Description: soc gpio include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-16 16:46:43
 * @LastEditTime: 2019-10-21 14:27:20
 */

#ifndef __TY_LIGHT_BASIS_SOC_GPIO_H__
#define __TY_LIGHT_BASIS_SOC_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "ty_light_basis_types.h"

#define     TYBT3L      0
#define     TYBT8C      1

#pragma pack(1)
typedef struct{
    u8 num;
    u32 pin;
}num2pin_t;

typedef struct{
    u32 pin;
    hal_gpio_func_e func;
    hal_gpio_in_out_mode_e mode;
    hal_gpio_up_down_resistor_level_e cfg;
    u8 level;
    u16 frequency;
    u8 positive_duty_cycle;
}gpio_config_t;
#pragma pack()

#define PIN_NUM_ERROR       0xFFFF
#define MAX_GPIO_PIN_NUM    40

/************************* platform specific interface ****************/
/**
 * @description: get port number from pin index
 * @param {pin_num} pin index of the module
 * @return: actual pin number of pin index
 */
u32 ty_light_basis_soc_gpio_get_pin(u8 pin_num);

/************************* platform specific interface*****************/


/************************* common interface **************************/
/**
 * @description: GPIO output init
 * @param {u8 pin_num} pin index of the module
 * @param {bool pin_lv} high level or low level
 * @return: LIGHT_INVALID_PARM or LIGHT_OK
 */
OPERATE_LIGHT ty_light_basis_soc_gpio_output_init(u8 pin_num, bool pin_lv);

/**
 * @description: GPIO input init
 * @param {u8 pin_num} pin index of the module
 * @param {u8 pull_value} hal_gpio_up_down_resistor_level_e
                            typedef enum{
                                GPIO_RESISTOR_FLOAT = 0x00,
                                GPIO_RESISTOR_PULLUP_1M,
                                GPIO_RESISTOR_PULLUP_10K,
                                GPIO_RESISTOR_PULLDOWN_100K,
                            }hal_gpio_up_down_resistor_level_e;
 * @return: LIGHT_INVALID_PARM or LIGHT_OK
 */
OPERATE_LIGHT ty_light_basis_soc_gpio_input_init(u8 pin_num, u8 pull_value);

/**
 * @description: gpio output write
 * @param {type} none
 * @return: none
 */
void ty_light_basis_soc_gpio_output_write(u8 pin_num, u8 value);

/**
 * @description: GPIO input read
 * @param {u8 pin_num} pin index of the module
 * @return: the pin's level(1: high 0: low)
 */
u8 ty_light_basis_soc_gpio_input_read(u8 pin_num);
/************************* common interface end **********************/

/**
 * @brief: SOC general pin Reset proc
 * @param {u8 ucPin -> reset pin}
 * @retval: none
 */
void ty_light_basis_soc_gpio_pin_reset(u8 pin_num);

/**
 * @brief: SOC i2c ctrl set proc
 * @param {u8 ucPin -> set pin}
 * @retval: none
 */
void ty_light_basis_soc_gpio_pin_set(u8 pin_num);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __SOC_GPIO_H__ */

/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: ty_light_driver_soc_pwm.h
 * @Description: soc PWM include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-24 20:42:29
 * @LastEditTime: 2019-08-28 21:50:23
 */

#ifndef __TY_LIGHT_DRIVER_SOC_PWM_H__
#define __TY_LIGHT_DRIVER_SOC_PWM_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "ty_light_basis_types.h"

/**
 * @berief: PWM SOC Init
 * @param {IN u16 frequency --> PWM frequency,unit:HZ}
 * @param {IN u16 positive_duty_cycle --> PWM Init Duty(positive duty),range:0 ~ 20000}
 * @param {IN u8 channel_num --> PWM channel num}
 * @param {IN u8 *gpio_list --> PWM gpio set}
 * @param {IN bool CCT_flag --> CCT drive flag}
 * @return: OPERATE_LIGHT
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_driver_soc_pwm_init(IN u16 usFrequency, IN u16 usPositiveDutyCycle, IN u8 ucChannelNum, IN u8 *pGpioList,IN bool bCCTFlag);

/**
 * @berief: PWM SOC SET Duty
 * @param {IN u8 channel -> pwm send out channel num}
 * @param {IN u16 duty ->  pwm send duty,rang: 0~20000}
 * @attention channel --> according to the initlize order
 * @return: none
 * @retval: none
 */
void ty_light_driver_soc_pwm_set_duty(IN u8 ucChannel, IN u16 usDuty);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __SOC_PWM_H__ */

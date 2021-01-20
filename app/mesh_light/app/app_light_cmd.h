/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: app_light_cmd.h
 * @Description: app light cmd include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-26 13:55:40
 * @LastEditTime: 2019-10-18 10:53:39
 */

#ifndef __APP_LIHGT_CMD_H__
#define __APP_LIHGT_CMD_H__


#include "ty_light_basis_types.h"
#include "base_oem_config.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define APP_DATA_AUTO_SAVE_DELAY_TIME       5000


#include "ty_light_save_user_flash.h"

/**
 * @brief: set light switch data, adapte control data issued by system
 *          to control data format.
 * @param {IN bool bONOFF -> switch data, TRUE will turn on} 
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_switch_set(IN bool bONOFF);

/**
 * @brief: set light mode data
 * @param {IN LIGHT_MODE_E Mode} 
 * @attention:Mode value is below:
 *                                  WHITE_MODE = 0,
 *                                  COLOR_MODE = 1,
 *                                  SCENE_MODE = 2,
 *                                  MUSIC_MODE = 3,
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_mode_set(IN LIGHT_MODE_E Mode);

/**
 * @brief: set light bright data, adapte control data issued by system
 *          to control data format.
 * @param {IN u16 usBright} 
 * @attention: acceptable range:10~1000
 * @attention: set bright value, will auto set the Mode to WHITE_MODE !
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_bright_set(IN u16 usBright);

/**
 * @brief: set light temrperature data, adapte control data issued by system
 *          to control data format.
 * @param {IN u16 usTemperature} 
 * @attention: acceptable range:0~1000
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_temperature_set(IN u16 usTemperature);

/**
 * @brief: set light hsl, transform hsl to hsv for control light
 * @param {IN u16 usHue}  hue, range:0 ~ 360
 * @param {IN u16 usSat}  saturation, range:0 ~ 1000
 * @param {IN u16 usLightness} Lightness, range:0 ~ 1000
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_hsl_set(IN u16 usHue,IN u16 usSat,IN u16 usLightness);

/**
 * @brief: set light hsv data
 * @param {IN u16 usHue}  hue, range:0 ~ 360
 * @param {IN u16 usSat}  saturation, range:0 ~ 1000
 * @param {IN u16 usValue} value, range:0 ~ 1000
 * @retval: OPERATE_LIGHT -> LIGHT_OK meaning need to call app_light_ctrl_proc() function!
 */
OPERATE_LIGHT app_light_ctrl_data_hsv_set(IN u16 usHue,IN u16 usSat,IN u16 usValue);

/**
 * @brief: get light switch data
 * @param {OUT bool *pONOFF -> switch data return} 
 * @retval: OPERATE_LIGHT
 */
bool app_light_ctrl_data_switch_get(void);

/**
 * @brief: get light mode data
 * @param {OUT LIGHT_MODE_E *pMode -> mode data return} 
 * @retval: OPERATE_LIGHT
 */
u8 app_light_ctrl_data_mode_get_value(void);

/**
 * @berief: get light way data
 * @param :none
 * @retval: u8 ->typedef enum {
                    LIGHT_C = 1,
                    LIGHT_CW,
                    LIGHT_RGB,
                    LIGHT_RGBC,
                    LIGHT_RGBCW,
                    LIGHT_MAX,
                }CTRL_LIGHT_WAY_E;
 */
u8 app_light_cfg_light_way_get_value(void);

/**
 * @brief: geta light bright data
 * @param {OUT u16 *pBright -> bright data return} 
 * @retval: OPERATE_LIGHT
 */
u16 app_light_ctrl_data_bright_get(void);

/**
 * @brief: get light temrperature data
 * @param {OUT u16 *pTemperature -> temperature data return} 
 * @retval: OPERATE_LIGHT
 */
u16 app_light_ctrl_data_temperature_get(void);

/**
 * @brief: get light RGB data & original data
 * @param {OUT COLOR_ORIGIN_T *ptOriginalColor -> color original data return} 
 * @retval: OPERATE_LIGHT
 */
void app_light_ctrl_data_hsl_get(OUT u16 *hue,OUT u16 *sat,OUT u16 *lightness);

/**
 * @brief: get light RGB data & original data
 * @param {OUT COLOR_ORIGIN_T *ptOriginalColor -> color original data return} 
 * @retval: OPERATE_LIGHT
 */
void app_light_ctrl_data_hsv_get(OUT u16 *hue,OUT u16 *sat,OUT u16 *value);


/**
 * @brief: set light countdown value
 * @param {IN u32 CountDownSec -> unit:second} 
 * @attention: countdown lave time will return with 
 *              switch status will return with calling
 *              vLightCtrlDataSwitchRespone function when countdown active.
 * @retval: OPERATE_LIGHT -> LIGHT_OK set countdown OK.
 */
OPERATE_LIGHT app_light_ctrl_data_countdown_set(IN u32 CountDownSec);

/**
* @brief: set light realtime control data
* @param {IN bool bMusicFlag}
* @param {IN char *pRealTimeData}
* @attention: data format: please reference to DP protocol
* @retval: OPERATE_LIGHT -> LIGHT_OK need to call app_light_real_time_ctrl_proc function.
*/
OPERATE_LIGHT app_light_ctrl_data_realtime_adjust_set(IN bool bMusicFlag, IN char *pRealTimeData);


/**
 * @brief: Light ctrl data save
 * @param {none}
 * @attention: this function directly save ctrl data.
 * @retval: none
 */
OPERATE_LIGHT app_light_ctrl_data_auto_save(IN LIGHT_FLASH_SAVE_TYPE_E eDataType);

/**
 * @brief: Light ctrl data read
 * @param {none}
 * @attention: this function directly read ctrl data.
 * @retval: none
 */
OPERATE_LIGHT app_light_ctrl_data_auto_read(IN LIGHT_FLASH_SAVE_TYPE_E eDataType);
/**
 * @brief: light ctrl data auto save proc
 * @param {IN u32 uiDelayTimeMs -> ctrl data auto save delay time,uint:ms}
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_ctrl_data_auto_save_start(IN u32 uiDelayTimeMs);

/**
 * @berief: get light countdown time
 * @param {OUT u32 *CountdownTime -> Countdown time return} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_ctrl_data_countdown_get(OUT u32 *CountdownTime);

/**
 * @berief: get light if disturb
 * @param {OUT bool light_not_disturb} 
 * @retval: OPERATE_LIGHT
 */
bool app_light_ctrl_data_not_disturb_get(void);

/**
 * @berief: set light if disturb
 * @param {IN bool bNotDisturb} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT app_light_ctrl_data_not_disturb_set(IN bool bNotDisturb);

/**
 * @brief: get light countdown data for vendor
 * @param {OUT u8 *data}
 * @attention: data format: please reference to DP protocol
 * @retval: OPERATE_LIGHT -> LIGHT_OK need to call app_light_real_time_ctrl_proc function.
 */
void app_tuya_vendor_light_countdown_data(u8 *data);

/**
 * @brief: get light memory data for vendor
 * @param {OUT u8 *data}
 * @attention: data format: please reference to DP protocol
 * @retval: OPERATE_LIGHT -> LIGHT_OK need to call app_light_real_time_ctrl_proc function.
 */
void app_tuya_vendor_light_lightmem_data(u8 *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_CONTROL_H__ */

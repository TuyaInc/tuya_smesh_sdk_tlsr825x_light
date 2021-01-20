/*
 * @Author: jinlu
 * @email: jinlu@tuya.com
 * @LastEditors:   
 * @file name: ty_light_save_user_flash.h
 * @Description: light production flash read/write include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-26 13:55:40
 * @LastEditTime: 2019-08-10 13:01:32
 */

#ifndef __TY_LIGHT_SAVE_USER_FLASH_H__
#define __TY_LIGHT_SAVE_USER_FLASH_H__

#include "ty_light_basis_types.h"
#include "base_oem_config.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#define LIGHT_SCENE_MAX_LENGTH    210

#define SINGLE_SCENE_LEN 35
#define ALL_SCENE_MAX 8
u8 AllScene[ALL_SCENE_MAX][SINGLE_SCENE_LEN];

#pragma pack(1)
 typedef enum
 {
	TYPE_APP_DATA = 0,
	TYPE_MDEV_DATA,
    TYPE_MEM_DATA,
    TYPE_FAV_FST_DATA,
    TYPE_FAV_SEC_DATA,
    TYPE_FAV_THD_DATA,
    TYPE_FAV_FUR_DATA,
	TYPE_USER_DATA,
}LIGHT_FLASH_SAVE_TYPE_E;

//--------------LIGHT_CFG_3IN1_SAVE == 0
typedef struct 
{
	u8 head;       //0x5A
	u8 keyid;
	u16 size;      
	u8 rsv;
} data_head_info_t;

typedef struct 
{
    u32 all_data_offset;      //Total offset, latest blank address
    u32 app_data_offset;
    u32 mdev_data_offset;
    u32 mem_data_offset;
    u32 fav_fst_data_offset;
    u32 fav_sec_data_offset;
    u32 fav_thd_data_offset;
    u32 fav_fur_data_offset;
    u32 user_data_offset;
} data_offset_info_t;
//-------------------------

typedef enum
{
    LIGHT_MODE_WHITE = 0,
    LIGHT_MODE_COLOR,
    LIGHT_MODE_SCENE,
    LIGHT_MODE_MUSIC,
    LIGHT_MODE_MAX,
}LIGHT_MODE_FLASH_E;

typedef struct 
{
    u16 usRed;         /* color R setting */
    u16 usGreen;
    u16 usBlue;
}COLOR_RGB_FLASH_T;

typedef struct 
{
    u16 usHue;
    u16 usSat;
    u16 usValue;
    char   cColorStr[13];
}COLORT_ORIGIN_FLASH_T;

/**
 * @brief Light save data structure
 */
typedef struct 
{
    int                  bPower;
    LIGHT_MODE_FLASH_E      eMode;
    u16                usBright;
    u16                usTemper;
    COLOR_RGB_FLASH_T       tColor;
    COLORT_ORIGIN_FLASH_T   tColorOrigin;
#if (1 == LIGHT_CFG_3IN1_SAVE)
    char                  cScene[LIGHT_SCENE_MAX_LENGTH + 1];
#else
    u8 tSceneId;
#endif
}LIGHT_APP_DATA_FLASH_T;

/**
 * @brief Light bMemory Customized
 * the configuration need to set firstly ,when use the control frame! 
 */
typedef struct 
{
    int    usFlag;
    u8     usNotDisturb;
    u8     usMemory;
    u16    usHue;
    u16    usSat;
    u16    usValue;
    u16    usBright;
    u16    usTemper;
    u16    usSyncGroupAddr;
}LIGHT_MEM_USER_FLASH_T;


/**
 * @brief Light save data structure
 */
typedef enum {
    FUC_TEST1 = 0,
    AGING_TEST,
    FUC_TEST2,
}TEST_MODE_E;

/**
 * @brief Light mdev test save structure
 */
typedef struct {
    TEST_MODE_E  eTestMode;
    u16     usAgingTestedTime;
	u8     ucResetCnt;
}LIGHT_MDEV_TEST_DATA_FLASH_T;

/**
 * @brief Light save favorite data structure
 */
typedef struct 
{
    int                  bPower;
    LIGHT_MODE_FLASH_E          eMode;
    u16                usBright;
    u16                usTemper;
    u16                usRed;         /* color R setting */
    u16                usGreen;
    u16                usBlue;
    u8                 sence;
	u16                usHue;     /* hue, range:0 ~ 360 */
    u16                usSat;     /* saturation, range:0 ~ 1000 */
    u16                usValue;   /* value, range:0 ~ 1000 */
}LIGHT_FAVORITE_DATA_FLASH_T;

/**
 * @brief Light custumer save structure
 */
typedef struct {
}LIGHT_CUST_DATA_FLASH_T;


//@attention!!!

#pragma pack()

#define FALSH_SCENE_DATA_START_ADDR 0x79000
#define FLASH_3IN1_DATA_START_ADDR  0x7D000


#define RESET_CNT_OFFSET            FLASH_3IN1_DATA_START_ADDR
#define LIGHT_APP_DATA_OFFSET       (RESET_CNT_OFFSET + 2)
#define PROD_TEST_DATA_OFFSET       (LIGHT_APP_DATA_OFFSET + sizeof(LIGHT_APP_DATA_FLASH_T) + 1)
#define LIGHT_MEM_USER_OFFSET       (PROD_TEST_DATA_OFFSET + sizeof(LIGHT_MDEV_TEST_DATA_FLASH_T) + 1)

#define LIGHT_FAV_FIRST_DATA_OFFSET        (LIGHT_MEM_USER_OFFSET + sizeof(LIGHT_FAVORITE_DATA_FLASH_T) + 1)
#define LIGHT_FAV_SECOND_DATA_OFFSET       (LIGHT_FAV_FIRST_DATA_OFFSET + sizeof(LIGHT_FAVORITE_DATA_FLASH_T) + 1)
#define LIGHT_FAV_THIRD_DATA_OFFSET        (LIGHT_FAV_SECOND_DATA_OFFSET + sizeof(LIGHT_FAVORITE_DATA_FLASH_T) + 1)
#define LIGHT_FAV_FOURTH_DATA_OFFSET       (LIGHT_FAV_THIRD_DATA_OFFSET + sizeof(LIGHT_FAVORITE_DATA_FLASH_T) + 1)

#define LIGHT_CUST_USER_OFFSET      0x7DC00

#define FLASH_3IN1_DATA_SIZE_MAX  (sizeof(LIGHT_MEM_USER_FLASH_T) + sizeof(LIGHT_APP_DATA_FLASH_T) + sizeof(LIGHT_MDEV_TEST_DATA_FLASH_T) + (sizeof(LIGHT_FAVORITE_DATA_FLASH_T))*5 +9)

OPERATE_LIGHT ty_light_save_user_flash_open_appdata(void);

/**
 * @berief: write app data in flash
 * @param {none}
 * @return: OPERATE_LIGHT
 * @retval: none
 */
OPERATE_LIGHT ty_light_save_user_flash_close_appdata(void);

/**
 * @brief: save light application data
 * @param {IN LIGHT_APP_DATA_FLASH_T *pData -> save data} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_save_user_flash_write_data(IN LIGHT_FLASH_SAVE_TYPE_E eDataType,IN u8 *pData);

/**
 * @brief: read light application data
 * @param {IN LIGHT_APP_DATA_FLASH_T *pData -> save data} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_save_user_flash_read_data(IN LIGHT_FLASH_SAVE_TYPE_E eDataType,OUT u8 *pData);

OPERATE_LIGHT ty_light_save_user_flash_offset_init(void);

/**
 * @brief: save 3in1 light application data
 * @param {IN LIGHT_APP_DATA_FLASH_T *pData -> save data} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_save_user_flash_write_3in1_data(IN LIGHT_FLASH_SAVE_TYPE_E eDataType,IN u8 *pData);

/**
 * @brief: read 3IN1 light application data
 * @param {IN LIGHT_APP_DATA_FLASH_T *pData -> save data} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_save_user_flash_read_3in1_data(IN LIGHT_FLASH_SAVE_TYPE_E eDataType,OUT u8 *pData);
/**
 * @brief: erase all user flash data
 * @param {none} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_save_user_flash_3in1_erase_all(void);

/**
 * @brief: erase all user flash data
 * @param {none} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_save_user_flash_erase_all(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __USER_FLASH_H__ */

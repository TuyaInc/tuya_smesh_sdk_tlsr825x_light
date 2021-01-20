/**
 * @Author: jinlu
 * @file name: ty_light_json_config.h
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 */
#ifndef __TY_LIGHT_JSON_CONFIG_H__
#define __TY_LIGHT_JSON_CONFIG_H__

#include "ty_light_basis_types.h"

typedef enum {
    TYPE_HEX = 0,
    TYPE_STRING,
}VALUE_TYPE;

typedef enum{
    CMOD_C = 0x01,
    CMOD_CW = 0x02,
    CMOD_RGB = 0x03,
    CMOD_RGBC = 0x04,
    CMOD_RGBCW = 0x05
}CMODE_T;

typedef enum{
    DMOD_PWM = 0x00,
    DMOD_IIC_SM16726  = 0x01,
    DMOD_IIC_SM2135 = 0x02
}DMODE_T;

typedef enum{
    GWCM_OLD = 0x00,
    GWCM_LOW_POWER = 0x01,
    GWCM_SPCL_MODE = 0x02,
    GWCM_OLD_PROD = 0x03,
    GWCM_SPCL_AUTOCFG = 0x05
}WFCFG_T;

typedef enum{
    COLOR_C = 0x00,
    COLOR_W = 0x01,
    COLOR_R = 0x02,
    COLOR_G = 0x03,
    COLOR_B = 0x04,
    COLOR_RGB = 0x05
}COLOR_T;

typedef enum{
    CW_TYPE = 0,
    CCT_TYPE = 1
}CWTYPE_T;

typedef enum{
    CHANGE_GRADUALLY = 0,
    CHANGE_DIRECTLY = 1 
}ONOFFMODE_T;

typedef enum{
    MEM_SAVE_NOT = 0,
    MEM_SAVE = 1 
}MEMORY_T;

#pragma pack(1)

typedef struct {
//common
    char Jsonver[5];          // json version
    char category[4];         // ble(sigmesh) dedicated

//device function 
    char  module[15];         // module choosed for the light
    char  cmod[6];            // Color model: 1 -> C; 2 -> CW; 3 -> RGB; 4 -> RGBC; 5 ->RGBCW;
    u8 dmod;               // Color driver mode: 0->pwm; 1->sm16726b; 2->sm2135;
    u8 cwtype;             // Color temperature drive mode: 0 -> CW; 1: -> CCT;
    u8 onoffmode;          // Is there a gradient when switching: 0 -> turn on gradually; 1 -> turn ondirectly;
    u8 pmemory;            // Is there a power-off memory: 1 -> save app data; 0 -> don't save
    u8 notdisturb;         // 1 -> do not disturb  0 -> disturb
    u8 ctrl_pin;           // CTRL pin: 
    u8 ctrl_lv;            // CTRL pin level
    u8 rhythm;             // rhythm
    
//light config
    char  defcolor;           // light color after connected
    u8 defbright;          // light brightness after connected
    u8 deftemp;            // light default temperature
    u8 cwmaxp;             // Maximum Power configuration of Cold and warm Light mixing
    u8 brightmin;          // Minimum brightness: 1~100
    u8 brightmax;          // Maximum brightness: 1~100
    u8 colormax;           // Minimum brightness: 1~100
    u8 colormin;           // Maximum brightness: 1~100

//connection config
    char  wfcfg[10];           // Low power / flash, value: spcl,prod
    u8 rstnum;             // number of times required to reset by switching on and off
    u8 remdmode;           // light reset pairing mode
    char  rstcor;             // light color while connecting
    u8 rstbr;              // light brightness while connecting
    u8 rsttemp;            // light brightness while connecting
    u16 remdtime;          // light reset pairing reminde time

//pwm config
    u32  pwmhz;              // PWM frequency
    u8 r_pin;              // Red color control pin
    u8 r_lv;               // Red color control pin level
    u8 g_pin;              // Green color control pin
    u8 g_lv;               // Green color control pin level
    u8 b_pin;              // Blue  color control pin
    u8 b_lv;               // Blue  color control pin level
    u8 c_pin;              // Cold white color control pin
    u8 c_lv;               // Cold white color control pin level
    u8 w_pin;              // Warm white color control pin
    u8 w_lv;               // Warm white color control pin level

//IIC pin config
    u8 iicr;               // iic Red channel
    u8 iicg;               // iic Green channel
    u8 iicb;               // iic Blue channel
    u8 iicc;               // iic Cold white channel
    u8 iicw;               // iic Warm white channel
    u8 iicscl;             // iic SCL pin
    u8 iicsda;             // iic SDA pin
    u8 wampere;            // 2135 white current
    u8 campere;            // 2135 color current

    u8 ucGammaRedbuf;
    u8 ucGammaGreenbuf;
    u8 ucGammaBluebuf;

    u8 gammakr;
    u8 gammakg;
    u8 gammakb;

    u8 gammawr;
    u8 gammawg;
    u8 gammawb;

    u8 prodagain;
    u8 ctime;
    u8 wtime;
    u8 rgbtime;

    bool  colorpfun;  
    u8 colormaxp;   

    
}DEVICE_CONFIG_T;
#pragma pack()

#define GET_ARRAY_LEN(x)    (sizeof(x)/sizeof(x[0]))

#define MAX_CONFIG_STR_SIZE 512

/**
 * @brief: set device configuration data.
 * @param {IN u16 ucLen -> oem cfg len} 
 * @param {IN u8 *pConfig -> oem cfg data} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_json_config_data_set(IN u16 ucLen, IN const u8 *pConfig);

/**
 * @brief: load device configuration data.
 * @param {none} 
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_json_config_load(void);

/**
 * @brief: get json version string
 * @param {out u8 *pJsonVer -> json version string }
 * @retval: OPERATE_LIGHT 
 */
u16 ty_light_json_config_get_jsonver(void);

/**
 * @brief: get json config data :category
 * @param {OUT u8 *pCategory -> category}
 * @retval: OPERATE_LIGHT
 */
u16 ty_light_json_config_get_category(void);

/**
 * @description: get json config data: remind time
 * @param {type} none
 * @return:
 */
u16 ty_light_json_config_get_remdtime(void);

/**
 * @brief: get color mode -- production is RGBCW/RGBC/RGB/CW/C
 * @param {type} none
 * @return: 
 *          0x01 -> C, 
 *          0x02 -> CW
 *          0x03 -> RGB
 *          0x04 -> RGBC
 *          0x05 -> RGBCW
 *          -1   -> not find light way,error
 */
u8 ty_light_json_config_get_color_mode(void);

/**
 * @brief: get json config data: driver mode
 * @param {type} none
 * @return: 
 *          0x00 -> PWM
 *          0x01 -> SM16726B
 *          0x02 -> SM2135
 *          -1   -> configure not load, error
 */
u8 ty_light_json_config_get_driver_mode(void);

/**
 * @brief: get json config data: cwtype
 * @param {type} none
 * @return: 
 *          0x00 -> CW drive
 *          0x01 -> CCT drive
 *          -1   -> configure not load, error
 */
u8 ty_light_json_config_get_cw_type(void);

/**
 * @brief: get json config data: onoffmode
 * @param {type} none
 * @return: 
 *          0x00 -> turn on/off change gradually 
 *          0x01 -> turn on/off change directly
 *          -1   -> configure not load, error
 */
u8 ty_light_json_config_get_onoff_mode(void);

/**
 * @brief: get memory cfg -- if not save app control data
 * @param {type} none
 * @return: 
 *          0x00 -> don't save app data in flash
 *          0x01 -> save app data in flash
 *          -1   -> configure not load, error
 */
u8 ty_light_json_config_get_pmemory_cfg(void);

/**
 * @brief: dot not disturb cfg -- if not save app control data
 * @param {type} none
 * @return: 
 *          0x00 -> disturb
 *          0x01 -> dot not disturb
 */
u8 ty_light_json_config_get_notdisturb(void);

/**
 * @brief: get ctrl pin number
 * @param {type} none
 * @return: ctrl pin number,  -1   -> configure not load, error
 */
u8 ty_light_json_config_get_ctrl_pin(void);

/**
 * @brief: get ctrl pin level
 * @param {type} none
 * @return: ctrl pin level,  -1 -> configure not load, error
 */
u8 ty_light_json_config_get_ctrl_pinLevel(void);

/**
 * @brief: get rhythm flag
 * @param {type} none
 * @return: rhythm flag
 */
u8 ty_light_json_config_get_rhythm(void);

/**
 * @brief: get json config data: color set when connected.
 * @param {type} none
 * @return: 
 *          0x00 -> default bright is C when connected.
 *          0x01 -> default bright is W when connected.
 *          0x02 -> default bright is R when connected.
 *          0x03 -> default bright is G when connected.
 *          0x04 -> default bright is B when connected.
 *          -1   -> configure not load, error
 */
u8 ty_light_json_config_get_defcolor(void);

/**
 * @brief: get json config data: brightness when connected.
 * @param {type} none
 * @return: brightness, -1   -> configure not load, error
 */
u8 ty_light_json_config_get_defbrightness(void);

/**
 * @brief: get json config data: deftemp when connected.
 * @param {type} none
 * @return: default temperature when connected.  -1   -> configure not load, error
 */
u8 ty_light_json_config_get_deftemper(void);

/**
 * @brief: get json config data: white max power
 * @param {type} none
 * @return: white max power, -1   -> configure not load, error
 */
u8 ty_light_json_config_get_cw_maxpower(void);

/**
 * @brief: get the CW minimum brightness 
 * @param {type} none
 * @return: CW minimum brightness, -1   -> configure not load, error
 */
u8 ty_light_json_config_get_whitemin(void);

/**
 * @brief: get the CW maxinum brightness
 * @param {type} none
 * @return: CW maxinum brightness,-1   -> configure not load, error
 */
u8 ty_light_json_config_get_whitemax(void);

/**
 * @brief: get the RGB minimum brightness 
 * @param {type} none
 * @return: RGB mininum brightness,-1   -> configure not load, error
 */
u8 ty_light_json_config_get_colormin(void);


/**
 * @brief: get the RGB maxinum brightness 
 * @param {type} none
 * @return: RGB maxinum brightness,-1   -> configure not load, error
 */
u8 ty_light_json_config_get_colormax(void);

/**
 * @brief: get wifi pair mode config
 * @param {type} none
 * @return: 
 *          GWCM_OLD -> 
 *          GWCM_LOW_POWER ->
 *          GWCM_SPCL_MODE ->
 *          GWCM_OLD_PROD ->
 *          -1   -> not find, error
 */
u8 ty_light_json_config_get_wificfg(void);

/**
 * @brief: get pairing reset number.
 * @param {type} none
 * @return: reset number,-1   -> configure not load, error
 */
u8 ty_light_json_config_get_resetnum(void);

/**
 * @brief: get color set when pairing.
 * @param {type} none
 * @return: 
 *          0x00 -> default bright is C when pairing.
 *          0x01 -> default bright is W when pairing.
 *          0x02 -> default bright is R when pairing.
 *          0x03 -> default bright is G when pairing.
 *          0x04 -> default bright is B when pairing.
 *          -1   -> not find, error
 */
u8 ty_light_json_config_get_resetcolor(void);

/**
 * @brief: get reset brightness when pairing
 * @param {type} none
 * @return: brightness when pairing,-1   -> not find, error
 */
u8 ty_light_json_config_get_resetbrightness(void);

/**
 * @brief: get reset temperature(reset color is white) when pairing
 * @param {type} none
 * @return: reset temperature(reset color is white) when pairing,-1   -> not find, error
 */
u8 ty_light_json_config_get_resettemper(void);

/**
 * @brief: get pwm frequency 
 * @param {type} none
 * @return: pwm frequency,-1   -> not find, error 
 */
u16 ty_light_json_config_get_pwmhz(void);

/**
 * @brief: get red pin number
 * @param {type} none
 * @return: red pin number,-1   -> not find, error
 */
u8 ty_light_json_config_get_redpin(void);

/**
 * @brief: get red pin effective level.
 * @param {type} none
 * @return: red pin effective level,-1   -> not find, error
 */
u8 ty_light_json_config_get_redpinlevel(void);

/**
 * @brief: get green pin number
 * @param {type} none
 * @return: green pin number,-1   -> not find, error
 */
u8 ty_light_json_config_get_greenpin(void);

/**
 * @brief: get blue pin number
 * @param {type} none
 * @return: blue pin number,-1   -> not find, error
 */
u8 ty_light_json_config_get_bluepin(void);

/**
 * @brief: get cold white pin number
 * @param {type} none
 * @return: cold white pin number,-1   -> not find, error
 */
u8 ty_light_json_config_get_coldpin(void);

/**
 * @brief: get cold white pin pin effective level.
 * @param {type} none
 * @return: cold white pin effective level,-1   -> not find, error
 */
u8 ty_light_json_config_get_coldpinlevel(void);

/**
 * @brief: get warm white pin number
 * @param {type} none
 * @return: warm white pin number
 */
u8 ty_light_json_config_get_warmpin(void);

/**
 * @brief: get iic r pin index
 * @param {type} none
 * @return: r pin index,-1   -> not find, error
 */
u8 ty_light_json_config_get_i2c_redindex(void);

/**
 * @brief: get iic g pin index
 * @param {type} none
 * @return: g pin index,-1   -> not find, error
 */
u8 ty_light_json_config_get_i2c_greenindex(void);

/**
 * @brief: get iic b pin index
 * @param {type} none
 * @return: b pin index,-1   -> not find, error
 */
u8 ty_light_json_config_get_i2c_blueindex(void);

/**
 * @brief: get iic c pin index
 * @param {type} none
 * @return: c pin index,-1   -> not find, error
 */
u8 ty_light_json_config_get_i2c_coldindex(void);

/**
 * @brief: get iic w pin index
 * @param {type} none
 * @return: w pin index,-1   -> not find, error
 */
u8 ty_light_json_config_get_i2c_warmindex(void);

/**
 * @brief: get iic scl pin number
 * @param {type} none
 * @return: scl pin number,-1   -> not find, error
 */
u8 ty_light_json_config_get_i2c_sclpin(void);


/**
 * @brief: get iic sda pin number
 * @param {type} none
 * @return: sda pin number,-1   -> not find, error
 */
u8 ty_light_json_config_get_i2c_sdapin(void);


/**
 * @brief: get sm2135 white mode current
 * @param {type} none
 * @return: sm2135 white mode current,-1   -> not find, error
 */
u8 ty_light_json_config_get_sm2135_white_ampere(void);

/**
 * @brief: get sm2135 color mode current
 * @param {type} none
 * @return: sm2135 color mode current,-1   -> not find, error
 */
u8 ty_light_json_config_get_sm2135_color_ampere(void);

/**
 * @brief: get remind mode when pairing
 * @param {type} none
 * @return: get remind mode when pairing
 */
u8 ty_light_json_config_devicecfg_get_remdmode(void);


/**
 * @brief: get production test1 repeat flag
 * @param {type} none
 * @return: production test1 repeat flag
 */
u8 ty_light_json_config_get_mdev_again(void);

/**
 * @brief: get production test1 cold aging time
 * @param {type} none
 * @return: production test1 cold aging time
 */
u16 ty_light_json_config_get_mdev_cold_agingtime(void);

/**
 * @brief: get production test1 warm aging time
 * @param {type} none
 * @return: production test1 warm aging time
 */
u16 ty_light_json_config_get_mdev_warm_agingtime(void);

/**
 * @brief: get production test1 rgb aging time
 * @param {type} none
 * @return: sm2135 white mode current
 */
u16 ty_light_json_config_get_mdev_rgb_agingtime(void);

/**
 * @brief: get json config data: rgb mix max power
 * @param {type} none
 * @return: rgb max power
 */
u8 ty_light_json_config_get_rgb_maxpower(void);

/**
 * @brief: get json config data: rgb max power funtion enable state
 * @param {type} none
 * @return: rgb max power funtion enable state
 */
bool ty_light_json_config_get_rgb_maxpower_enable(void);


#endif



/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors:   
 * @file name: ty_light_basis_tools.h
 * @Description: light common tool include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-26 13:55:40
 * @LastEditTime: 2019-05-28 15:24:01
 */

#ifndef __TY_LIHGT_BASIS_TOOLS_H__
#define __TY_LIHGT_BASIS_TOOLS_H__

#include "ty_light_basis_types.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#define size_get(x) (sizeof(x)/sizeof(x[0]))

/**
 * @brief: convert dec hex to string
 * @param {IN u8 *hexbuf} hex data, like 0x7B; 
 * @param {IN u8 hexLen} dec hexbuf len; 
 * @param {OUT u8 *strbuf} string, like "123". 
 * @retval: LIGHT_OK or LIGHT_INVALID_PARM
 */
OPERATE_LIGHT ty_light_basis_tools_hex2str(IN u8 *hexbuf, IN u8 hexLen, OUT u8 *strbuf);

/**
 * @brief: get the max value in 5 numbers
 * @param { u32 a -> number1 }
 * @param { u32 b -> number2 } 
 * @param { u32 c -> number3 }
 * @param { u32 d -> number4 }
 * @param { u32 e -> number5 } 
 * @retval: Max value
 */
u32 ty_light_basis_tools_get_max_value(IN u32 a, IN u32 b, IN u32 c, IN u32 d, IN u32 e);

/**
 * @brief: get the absolute value
 * @param { s32 value -> calc value } 
 * @retval: absolute value
 */
u32 ty_light_basis_tools_get_abs_value(IN s32 value);

/**
 * @berief: val bit valid
 * @param { val bit } 
 * @return: valid or not valid
 * @retval: none
 */

u8 ty_light_basis_tools_bitvalid(IN u8 val, IN u8 bit);

/**
 * @brief: convert dec string to dec hex
 * @param {decStr} dec string , like: "123"; 
 * @param {strLen} dec string len; 
 * @param {dec} dec hex, like 0x7B. 
 * @retval: dec hex length, 0 ~ 4
 */
u8 ty_light_basis_tools_str2dec(IN char *decStr, IN u8 strLen, OUT u32* dec);

/**
 * @brief: ASSIC change to hex
 * @param { char AscCode -> ASSIC code } 
 * @return: hex value
 * @retval: HEX
 */
u8 ty_light_basis_tools_asc2hex(IN char AscCode);

/**
 * @brief: ASSIC change to hex
 * @param { u8 H -> high 4 bit } 
 * @param { u8 L -> low 4 bit } 
 * @retval: u8
 */
u8 ty_light_basis_tools_str2uchar(IN u8 H, IN u8 L);

/**
 * @brief: four unsigned char merge into unsigned short
 * @param { u8 HH -> USHORT Hight hight 4bit }
 * @param { u8 HL -> USHORT Hight low 4bit   }
 * @param { u8 LH -> USHORT low hight 4bit   }
 * @param { u8 LL -> USHORT low low 4bit     }
 * @retval: u16
 */
u16 ty_light_basis_tools_str2ushort(IN u8 HH, IN u8 HL, IN u8 LH, IN u8 LL);

/**
 * @brief: HSV change to RGB
 * @param {IN u16 h -> range 0~360 }
 * @param {IN u16 s -> range 0~1000}
 * @param {IN u16 v -> range 0~1000}
 * @param {OUT u16 *r -> R result,rang from 0~1000}
 * @param {OUT u16 *g -> G result,rang from 0~1000}
 * @param {OUT u16 *b -> B result,rang from 0~1000}
 * @retval: none
 */
void ty_light_basis_tools_hsv2rgb(IN u16 h, IN u16 s, IN u16 v, OUT u16 *r, OUT u16 *g, OUT u16 *b);

/**
 * @brief: convert hsl to rgb
 * @param {IN u16 h} Hue : 0~65535
 * @param {IN u16 s} Saturation : 0~65535
 * @param {IN u16 l} Lightness : 0~65535
 * @param {OUT u16 *r} red value
 * @param {OUT u16 *g} green value
 * @param {OUT u16 *b} blue value
 * @retval: rgb value
 */
void ty_light_basis_tools_hsl2rgb(IN u16 h, IN u16 s, IN u16 v, OUT u16 *r, OUT u16 *g, OUT u16 *b);

/**
 * @brief: convert HSV to HSL
 * @param {float} h Hue component，0~359
 * @param {float} s Saturation component，0~1000
 * @param {float} v Value component，0~1000
 * @returns {object} HSL color value，65535
 */
void ty_light_basis_tools_hsv2hsl(IN u16 Hsv_h, IN u16 Hsv_s, IN u16 Hsv_v, OUT u16 *Hsl_h, OUT u16 *Hsl_s, OUT u16 *Hsl_l);

/**
 * @brief: convert HSL to HSV
 * @param {float} h Hue component，65535
 * @param {float} s Saturation component，65535
 * @param {float} l Lightness component，65535
 * @returns {object} HSL color value，360 1000 1000
 */
void ty_light_basis_tools_hsl2hsv(IN u16 Hsl_h, IN u16 Hsl_s, IN u16 Hsl_l, OUT u16 *Hsv_h, OUT u16 *Hsv_s, OUT u16 *Hsv_v);


/**
 * @brief: RGB change to HSV
 * @param {IN u16 R -> R,rang from 0~1000}
 * @param {IN u16 G -> G,rang from 0~1000}
 * @param {IN u16 B -> B,rang from 0~1000}
 * @param {OUT u16 H -> result, range 0~360}
 * @param {OUT u16 S -> result, range 0~1000}
 * @param {OUT u16 V -> result, range 0~1000}
 * @retval: none
 */
void ty_light_basis_tools_rgb2hsv(IN u16 R, IN u16 G, IN u16 B, OUT u16 *H, OUT u16 *S, OUT u16 *V);

/**
 * @berief: RGB change to HSV
 * @param {IN u16 R -> R,rang from 0~1000}
 * @param {IN u16 G -> G,rang from 0~1000}
 * @param {IN u16 B -> B,rang from 0~1000}
 * @param {OUT u16 H -> result, range 0~360}
 * @param {OUT u16 S -> result, range 0~1000}
 * @param {OUT u16 L -> result, range 0~1000}
 * @return: none 
 * @retval: none
 */
void ty_light_basis_tools_rgb2hsl(IN u16 R, IN u16 G, IN u16 B, OUT u16 *H, OUT u16 *S, OUT u16 *L);

/**
 * @brief: compare two string 
 * @param {IN char* str1 -> string 1}
 * @param {IN char* str2 -> string 2}
 * @retval: 0: if these two string is not same,
 *          1: these two string is all same
 */
bool ty_light_basis_tools_string_compare(IN char* str1, IN char* str2);

/**
 * @brief: change number to str
 * @param {IN char cMode -> change format( 0 -> %d, 4-> %4x)}
 * @param {IN u32 uiNum -> number(unsigned int)}
 * @param {IN u8 len -> buf len(sizeof)}
 * @param {OUT char *cStr -> string}
 * @retval: string
 */
void ty_light_basis_tools_num2str(IN char cMode, IN u32 uiNum, IN u8 len, OUT char *cStr);

/**
 * @brief: compress scene data,from string (max 2+26*8=210 bytes) to hex (8+8+64*8)bits = 66 bytes).
 * compressed data format:
 * scene id + mode + uint*8
 * @param {input_str} scene data string format
 * @param {output_buf} Output buffer address, compressed data.
 * @param {output_buf_len} output compressed data length
 * @retval: 1:success / 0:failed
 */
OPERATE_LIGHT ty_light_basis_tools_scene_data_compress(const IN char* input_str, OUT u8* output_buf, OUT u32 *output_buf_len);

/**
 * @brief: decompress scene data to string format
 * @param {input_buf} compressed scene data
 * @param {input_buf_len} compressed scene data length
 * @param {output_str} decompressed scene data,string format
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_basis_tools_scene_data_decompress(const IN u8* input_buf, const IN u32 input_buf_len, OUT char* output_str);

/**
 * @brief: decompress music data to string format
 * @param {const IN u8* input_buf} compressed music data
 * @param {const IN u32 input_buf_len} compressed music data length
 * @param {OUT char* output_str} decompressed music data,string format
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT ty_light_basis_tools_music_data_decompress(const IN u8* input_buf, const IN u32 input_buf_len, OUT char* output_str);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_TOOLS_H__ */

#ifndef __TY_LIHGT_GRADUAL_MAX_POWER_H__
#define __TY_LIHGT_GRADUAL_MAX_POWER_H__

#include "ty_light_basis_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief Bright 5ways value structure
 * Used in light gradually change calculation process
 */
#pragma pcak(1)

typedef struct 
{
    u16 usRed;
    u16 usGreen;
    u16 usBlue;
    u16 usWhite;
    u16 usWarm;
}BRIGHT_DATA_T;

#pragma pack()

/*********************************************************************************
  *Function: ty_light_gradual_max_power_cw_powergain_set
  *Description：cw power gain process
  *Input:   Bright:
            Temper:
  *Output: value: cw value after process
  *Return:  none
  *Others: none
**********************************************************************************/
void ty_light_gradual_max_power_cw_powergain_set(IN u16 Bright, IN u16 Temper, OUT BRIGHT_DATA_T *value);

/*********************************************************************************
  *Function: ty_light_gradual_max_power_rgb_powergain_set
  *Description：rgb power gain process
  *Input:  none
  *Output: value: cw value after process
  *Return:  none
  *Others: none
**********************************************************************************/
void ty_light_gradual_max_power_rgb_powergain_set(OUT BRIGHT_DATA_T *value);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  




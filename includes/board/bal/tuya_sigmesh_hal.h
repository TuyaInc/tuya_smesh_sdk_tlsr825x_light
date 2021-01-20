/*
 * tuya_sigmesh_hal.h
 *
 *  Created on: 2020-05-28
 *      Author: Joey
 */

#pragma once

#ifndef _PACKED_
#define _PACKED_  __attribute__ ((packed))
#endif

#include "../../proj/common/types.h"
#include "hal_log.h"
#include "mesh_node.h"
#include "mesh_common.h"
#include "app_provison.h"
#include "tuya_sigmesh_model.h"

#define MESH_UNASSIGNED_ADDR                    0x0000
#define MESH_IS_UNICAST_ADDR(addr)              ((addr) != MESH_UNASSIGNED_ADDR && ((addr) & 0x8000) == 0x0000)

typedef enum{
    NODE_POWER_ON_UNPROVISION        = 0x00,
    NODE_POWER_ON_IN_MESH  = 0x01,

    NODE_PROVISION_SUCCESS,
    NODE_KICK_OUT,
    NODE_MESH_RESET,
    NODE_RECOVER_IN_MESH,

    TY_OTA_START,
    TY_OTA_SUCCESS,
    TY_OTA_FAIL,
    TY_GROUP_SUB_ADD,
    TY_GROUP_SUB_DEL,
}mesh_state_t;

typedef struct{
    void (*oem_init)(void);
    void (*app_init)(void);
    void (*run)(void);
    void (*mesh_state)(mesh_state_t dev_state);

    void (*scan_cb)(uint8_t *adv, uint8_t adv_len, uint8_t *mac, int rssi);
    
    void (*factory_reset)(void);
}AppVendorS;

typedef struct
{
    uint8_t ele_idx;
    uint8_t sig_model_cnt;
    uint8_t vd_model_cnt;
    uint16_t *sig_model_list;//point to sig_model_list[]
    uint32_t *vd_model_list;//point to vd_model_list[]
} _PACKED_ mesh_element_t;

typedef struct
{
    uint8_t ele_cnt;
    mesh_element_t *element;    
} _PACKED_ mesh_element_list_t;

typedef struct
{
    union {
        struct {
            uint16_t rfu: 8;
            uint16_t is_fmkey: 1; //0-uuid is mac+pid+category; 1-uuid is mac+firmware key+category
            uint16_t key_share: 1; //model key share & group key bind share
            uint16_t pub_addr_need: 1;
            uint16_t rfu2: 5;
        } pid;
        uint16_t pid_val;
    };
} _PACKED_ compo_data_pid_t;

typedef struct
{
    uint8_t ttl;
    uint32_t relay: 2; //!< relay
    uint32_t proxy: 2; //!< Mesh Proxy Service & Proxy feature, binding with node identity
    uint32_t fn: 2; //!< friend feature
    uint32_t lpn: 2; //!< low power feature
    uint8_t net_trans_count: 3; //!< :3 transmit (net_trans_count + net_trans_count_base + 1) times
    uint8_t net_trans_steps: 5; //!< :5 retransmission interval = (net_trans_steps + 1) * 10ms

    uint8_t uuid[16]; // mac + pid + category
    
    uint16_t company_id; // composition data head: cid
    compo_data_pid_t product_id; // composition data head: pid
    uint16_t version_id; // composition data head: vid
} _PACKED_ node_info_t;

typedef struct{
    uint8_t KeyRefresh   :1;
    uint8_t IVUpdate     :1;
    uint8_t RFU          :6;
}ty_mesh_ctl_fri_update_flag_t;


// fast prov
typedef struct{
    uint8_t  net_key[16];
    uint16_t  key_index;
    uint8_t  flags;
    uint8_t  iv_index[4];
    uint16_t  unicast_address;
}_PACKED_ fast_prov_net_data_t;

typedef struct{
    uint8_t net_app_idx[3];
    uint8_t app_key[16];
}_PACKED_ fast_prov_app_key_t;

typedef enum {
    TUYA_RF_POWER_P10p46dBm = 63, //  10.46 dbm
    TUYA_RF_POWER_P10p29dBm = 61, //  10.29 dbm
    TUYA_RF_POWER_P10p01dBm = 58, //  10.01 dbm
    TUYA_RF_POWER_P9p81dBm  = 56, //   9.81 dbm
    TUYA_RF_POWER_P9p48dBm  = 53, //   9.48 dbm
    TUYA_RF_POWER_P9p24dBm  = 51, //   9.24 dbm
    TUYA_RF_POWER_P8p97dBm  = 49, //   8.97 dbm
    TUYA_RF_POWER_P8p73dBm  = 47, //   8.73 dbm
    TUYA_RF_POWER_P8p44dBm  = 45, //   8.44 dbm
    TUYA_RF_POWER_P8p13dBm  = 43, //   8.13 dbm
    TUYA_RF_POWER_P7p79dBm  = 41, //   7.79 dbm
    TUYA_RF_POWER_P7p41dBm  = 39, //   7.41 dbm
    TUYA_RF_POWER_P7p02dBm  = 37, //   7.02 dbm
    TUYA_RF_POWER_P6p60dBm  = 35, //   6.60 dbm
    TUYA_RF_POWER_P6p14dBm  = 33, //   6.14 dbm
    TUYA_RF_POWER_P5p65dBm  = 31, //   5.65 dbm
    TUYA_RF_POWER_P5p13dBm  = 29, //   5.13 dbm
    TUYA_RF_POWER_P4p57dBm  = 27, //   4.57 dbm
    TUYA_RF_POWER_P3p94dBm  = 25, //   3.94 dbm
    TUYA_RF_POWER_P3p23dBm  = 23, //   3.23 dbm
} TUYA_RF_Power;

typedef enum{
    POWER_BOOT = 0x00,
    TIMER_WAKE_UP,
    PIN_WAKE_UP,
    OTA_BOOT,
}tuya_boot_type;


typedef void (*tuya_mesh_data_revc_cb)(uint16_t src_addr, uint16_t dst_addr, uint32_t opcode, uint8_t *data, uint16_t data_len, uint8_t ttl);

/**
 * @brief init the mesh data recv callback
 * @param[in]  cb  callback in app
 * @return none
 */
void tuya_mesh_data_recv_cb_init(tuya_mesh_data_revc_cb cb);


/**
 * @brief send mesh data to unicast/group addr
 * @param[in]  src_addr  unicast addr of node,if use 0,it will use node primary element addr
 * @param[in]  dst_addr  destinat addr
 * @param[in]  opcode  mesh op code
 * @param[in]  data  data point
 * @param[in]  appkey_idx  use which appkey index
 * @param[in]  is_rsp  if is response msg or
 * @return tx result
 */
int tuya_mesh_data_send(uint16_t src_addr, uint16_t dst_addr, uint32_t opcode, uint8_t *data, uint16_t data_len, uint16_t appkey_idx, uint8_t is_rsp);


/**
 * @brief init the configuration information of the node
 * @param[in]  node_info  node config information
 * @return none
 */
void tuya_mesh_node_init(node_info_t *node_info);

/**
 * @brief Used to know if the device have been provisioned successfully
 * @return prov state
 */
uint8_t get_if_prov_success(void);

/**
 * @brief Used to get the primary element addr of the node
 * @return mesh addr
 */
uint16_t get_primary_ele_addr(void);

/**
 * @brief Used when power on/off to reset the device
 * @return power cnt
 */
uint8_t tuya_hal_get_power_cnt(void);

/**
 * @brief Call this function to get the reason for reboot.
 * @return the reason for reboot.
 */
tuya_boot_type tuya_hal_get_boot_type(void);


uint8_t tuya_get_adv_send_fifo_cnt(void);

void tuya_adv_send_fifo_reset(void);

uint8_t tuya_get_ble_connect_state(void);

void tuya_ble_disconnect(uint8_t update_blt_state);

/**
 * @brief set node primary element unicast addr
 * @param[in]  addr  unicast addr
 * @param[in]  save  if save in flash
 * @return none
 */
void tuya_primary_ele_addr_set(uint16_t addr, int save);

/**
 * @brief update the device key of the node
 * @param[in]  key  device key
 * @return none
 */
void tuya_mesh_devkey_update(uint8_t *key);

/**
 * @brief This function can reset the device,and the device can't recover to provision state.
 * @return none
 */
void tuya_mesh_network_reset(void);

/**
 * @brief This function can reset the device to the unprovision staate,and the device 
 *  can recover to the provision state in some case.Such as repower on,or the device
 *  doesn't be provision for 5s
 * @param[in]  The time that the device will be recovered if it is not been provision
 * @return none
 */
void tuya_mesh_network_reset_with_restore(uint16_t restore_time_s);

/**
 * @brief Used to configure whether the device needs to be repowered when the device beed reset
 *  with tuya_mesh_network_reset(void);
 * @param[in]  if need repower
 * @return none
 */
void tuya_mesh_network_reset_repower_cfg(uint8_t enable);

void tuya_mesh_uuid_set(uint8_t *uuid);
int tuya_group_addr_sub_set(uint16_t op, uint16_t node_adr, uint16_t ele_adr, uint16_t sub_adr, uint32_t md_id, int sig_model);
uint16_t* tuya_group_addr_sub_list_get(void);
uint16_t* tuya_model_group_addr_sub_list_get(uint16_t ele_idx, uint32_t model_id);
void tuya_mesh_rf_power_set(TUYA_RF_Power level);
void tuya_gatt_adv_beacon_enable(uint8_t enable);
void tuya_ble_hal_gatt_scan(bool enable);

void tuya_ble_advertise_send_cb_init(app_ble_adv_send_cb_t cb);


// fast prov
void tuya_fast_prov_net_data_set(fast_prov_net_data_t *prov_data, fast_prov_app_key_t *app_key);
int tuya_fast_prov_msg_recv_callback(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
void tuya_mesh_adv_txrx_to_self_en(u8 en); // telink need
void mesh_fast_prov_proc(void);
void mesh_fast_prov_init(void);

extern int tuya_dev_msg_recv_callback(unsigned char *par, int par_len, mesh_cb_fun_par_t *cb_par);




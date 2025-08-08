#ifndef __UC_MID_BLE_H__
#define __UC_MID_BLE_H__

#include "stdint.h"

// 数据包结构
typedef struct{
    void *payload;
    size_t size;
} uc_data_t;

// 事件回调函数
typedef void (*uc_mid_ble_event_callback)(
    int event,
    uc_data_t *data
);

// 初始化
int uc_mid_ble_init(void);

// 发送数据
int uc_mid_ble_send_data(uc_data_t *data);

// 注册事件回调函数
int uc_mid_ble_register_event_callback(uc_mid_ble_event_callback callback);

// 设置设备名称
int uc_mid_ble_set_device_name(const char *name, size_t size);

// 设置设备广播数据
int uc_mid_ble_set_adv_data(const char *data, size_t size);

// 设置设备广播扫描响应数据
int uc_mid_ble_set_adv_scan_rsp_data(const char *data, size_t size);

// 设置设备广播间隔
int uc_mid_ble_set_adv_interval(uint16_t min, uint16_t max);


#endif


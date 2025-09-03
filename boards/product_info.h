#ifndef __PRODUCT_INFO_H__
#define __PRODUCT_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MOVA_FW_NAME                                  "ROOT_PUMP_V3"
#define MOVA_DEVICE_NAME                              "V3"
#define MOVA_PRODUCT_ID                               0x01

/*
* 主固件版本类型:
* 'V' 正式版本
* 'T' 测试版本
*/
#define MOVA_FIRMWARE_TYPE                            'T'

/*
* 蓝牙协议版本号
*/
#define MOVA_BLE_PROTOCOL_VERSION                      100
#define MOVA_OTA_UPGRADE_TYPE                          4

/**
 * 正式版本从100-900
*/
#define MOVA_FIRMWARE_VERSION                          100
#define MOVA_FW_VERSION_STR                            "100"

/**
 * 资源版本号定义(major*100 + minor)
 * major版本表示固件支持的资源版本. 
 * minor表示资源差异化的版本.
*/
#define MOVA_RES_MAJOR_VERSION                         (100)
#define MOVA_RES_MINOR_VERSION                         (01)

/**
 * 算法采测版本号:
 * R分支,初始版本从100定义,与主版本号无直接联系
*/
#define ALGO_TEST_FIRMWARE_TYPE                        'R'
#define ALGO_TEST_FIRMWARE_VERSION                     100

//产品外观色
#define PRODUCT_COLOR_BLACK                            "00"
#define PRODUCT_COLOR_GOLDEN                           "01"
#define PRODUCT_COLOR_BLUE                             "02"
#define PRODUCT_SUB_ID_INDEX                           48

#ifdef __cplusplus
}
#endif

#endif


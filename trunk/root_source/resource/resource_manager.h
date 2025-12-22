#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include <stdio.h> // IWYU pragma: keep
#include <stdlib.h> // IWYU pragma: keep

/// 图片资源
#define IMG_RES_MAGIC (0x52534547) /// 资源包魔数

/// 图片资源包最大数量
#define IMG_RES_MAX_COUNT (1000)

/// 图片资源类型
typedef enum {
    IMG_RES_TYPE_BASE,  /// 基础资源
    IMG_RES_TYPE_UI,    /// UI资源
    IMG_RES_TYPE_ALGO,  /// 算法资源
    IMG_RES_TYPE_MAX
} img_res_type_e;

/// 图片资源语言类型
typedef enum {
    IMG_RES_LANG_TYPE_EN,  /// 英文
    IMG_RES_LANG_TYPE_ZH,  /// 中文
    IMG_RES_LANG_TYPE_MAX
} img_res_lang_type_e;

/// 描述资源ID和数据包size映射关系.
typedef struct img_res_obj_info {
    uint32_t img_id;          /// 图片资源ID.(1~n)
    uint32_t offset_addr;     /// 记录每个图片数据包的偏移地址
} img_res_obj_info_t;

/// 图片资源包头描述
typedef struct img_res_head_desc {
    uint32_t magic;     /// 资源包魔数
    uint16_t crc;       /// 资源包CRC校验值, 暂未启用
    uint16_t type;      /// 资源包类型，暂未启用
    uint32_t lang_id;   /// 语言ID
    uint32_t version;   /// 资源版本
    uint32_t res_count; /// 资源个数.
    uint32_t reserve;   /// 预留
    img_res_obj_info_t *obj_info;
} img_res_head_desc_t;

/// 图片资源包整体描述.
typedef struct img_res_head_wrapper {
    img_res_head_desc_t head; /// 资源包头描述
    uint8_t md5_head[16];     /// 资源包头哈希
    uint8_t md5_body[16];     /// 资源包数据哈希
    uint8_t res_body[];       /// 资源包数据集合.
} img_res_head_wrapper_t;

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif 



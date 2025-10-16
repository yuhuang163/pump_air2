#ifndef __SYSTEM_PERSIST_DATA_H__
#define __SYSTEM_PERSIST_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep
#include "app_algo.h"

#define ROOT_WRITE_INFO_LEN                         64
#define ROOT_BREAST_PUMP_TIME_VALID                 30000   /// 30s
#define ROOT_BREAST_PUMP_BODY_DATA_NUM              60      /// 按天存储，每天最多 ROOT_BREAST_PUMP_BODY_DATA_NUM / 7
#define USER_BIND_UID_LEN                           8
#define USER_BIND_RANDOM_LEN                        6
#define USER_BIND_AES_KEY_LEN                       16

#define DEVICE_UNIQUE_DID_LEN                       10
#define DEVICE_UNIQUE_CODE_LEN                      16
#define DEVICE_UNIQUE_SN_LEN                        18
#define DEVICE_FW_VERSION_LEN                       18

typedef enum  { 
    SYS_DAY_MONDAY = 0,
    SYS_DAY_TUESDAY = 1,
    SYS_DAY_WEDNESDAY = 2,
    SYS_DAY_THURSDAY = 3,
    SYS_DAY_FRIDAY = 4,
    SYS_DAY_SATURDAY = 5,
    SYS_DAY_SUNDAY = 6,
    SYS_DAY_MAX,
} sys_day_type_e;

typedef enum  { 
    SYS_MODE_STATE_NORMAL = 0,
    SYS_MODE_STATE_NON_DELETABLE = 1,
    SYS_MODE_STATE_NON_MOVE = 2,
    SYS_MODE_STATE_NON_DELETABLE_MOVE = 3,
    SYS_MODE_STATE_NON_DISPLAY = 4,
    SYS_MODE_STATE_MAX,
} sys_mode_state_e;

typedef enum {
    SYS_STAT_DATA_BURIAL,
    BREAST_PUMP_FINISH_REMIND,        /// 吸奶完成提醒
    MODE_CHANGE_REMIND,        /// 模式切换提醒
    BREAST_PUMP_TIMING_REMIND,       /// 泵奶定时提醒
    ALL_VOICE_SW,              /// 全部语音开关
    INTERVAL_30S_REMIND,       /// 30S间隔提醒
    AI_LEARN_SW,               /// AI学习开关
    SYS_STAT_MAX,
} sys_state_bit_map_e;

/// 工厂部署标志位
typedef struct {
    uint8_t deploy_flag;       /// 工厂模式部署标志
    uint8_t age_flag;
    uint8_t age_display;       /// 老化展示标志
    uint8_t fsensor_cali_flag; /// fsensor校准标志
} factory_flag_info_t;

typedef struct {
    uint8_t did[DEVICE_UNIQUE_DID_LEN];
    uint8_t code[DEVICE_UNIQUE_CODE_LEN];
    uint8_t sn[DEVICE_UNIQUE_SN_LEN];
    uint8_t body_color;
    uint8_t region_id;   /// 区域ID, 中版，俄版，欧版，美版...
    uint8_t product_id;  /// 产品ID
} factory_dev_info_t;

/// 工厂老化标志
typedef struct {
    uint8_t complete;
    uint8_t display; 
} factory_aging_flag_t;   

/// 泵奶模式
typedef enum {
    BREAST_PUMP_MODE_INVALID = 0,
    BREAST_PUMP_STIMULATION_MODE,          /// 刺激模式
    BREAST_PUMP_PUMP_MODE,        /// 吸乳模式
    BREAST_PUMP_MIX_MODE,               /// 混合模式
    BREAST_PUMP_CUSTOM_MODE,         /// 个性化模式
    BREAST_PUMP_PP_MILK_TRACK_MODE,                   /// PP追奶模式
    BREAST_PUMP_MILK_BOOST_MODE,              /// 2+8奶量提升模式
    BREAST_PUMP_LACTATION_START_MODE,            /// 开奶模式
    BREAST_PUMP_MODE_MAX,
    BREAST_PUMP_NORMAL_MODE_MAX = BREAST_PUMP_MIX_MODE - BREAST_PUMP_MODE_INVALID,
    BREAST_PUMP_UP_MODE_MAX = BREAST_PUMP_LACTATION_START_MODE - BREAST_PUMP_MIX_MODE,
    BREAST_PUMP_NORMAL_MODE_DEFAULT = BREAST_PUMP_STIMULATION_MODE, /// 默认常规模式
    BREAST_PUMP_UP_MODE_DEFAULT = BREAST_PUMP_CUSTOM_MODE, /// 默认追奶模式
} breast_pump_mode_e;

/// 泵奶时长
typedef enum {
    BREAST_PUMPING_TIME_ERROR = 0,
    BREAST_PUMPING_ONE_MIN = 60,
    BREAST_PUMPING_ONE_MIN_HALF = 90,
    BREAST_PUMPING_TWO_MIN = 120,
    BREAST_PUMPING_TWO_MIN_HALF = 150,
    BREAST_PUMPING_THREE_MIN = 180,
    BREAST_PUMPING_THREE_MIN_HALF = 210,
    BREAST_PUMPING_FOUR_MIN = 240,
    BREAST_PUMPING_FOUR_MIN_HALF = 270,
    BREAST_PUMPING_FIVE_MIN = 300,
    BREAST_PUMPING_DEFAULT_TIME = BREAST_PUMPING_TWO_MIN,
} sys_breast_pumping_time_e;

/// 泵奶力度
typedef enum {
    SYS_BREAST_PUMP_LEVEL_1 = 111,
    SYS_BREAST_PUMP_LEVEL_2,
    SYS_BREAST_PUMP_LEVEL_3,
    SYS_BREAST_PUMP_LEVEL_4,
    SYS_BREAST_PUMP_LEVEL_5,
    SYS_BREAST_PUMP_LEVEL_MAX,
} sys_breast_pump_level_e;

/// 泵奶方向
typedef enum { 
    BREAST_PUMPING_TEETH_CLOCKWISE= 0,  ///顺时针
    BREAST_PUMPING_TEETH_ANTI_CLOCKWISE = 1  ///逆时针
} sys_breast_pump_order;

typedef struct {
    uint16_t stimulation[STIMULATION_MODE_LEN];     /// 刺激
    uint16_t pump[PUMP_MODE_LEN]; /// 吸乳
    uint16_t mix[MIX_MODE_LEN];               /// 混合
    uint16_t custom[CUSTOM_MODE_LEN];    /// 个性化默认
    uint16_t pp_milk_track[PP_MODE_LEN];     /// PP追奶
    uint16_t milk_boost[MILK_BOOST_MODE_LEN];         /// 2+8奶量提升
    uint16_t lactation_start[LACTATION_START_MODE_LEN]; /// 开奶
} breast_pump_level_info_t;

/// 描述泵奶信息
typedef struct
{
    uint8_t is_remind_area_change;        /// 换区提示
    uint8_t is_remind_mode_change;        /// 模式切换提醒
    uint8_t is_remind_over_pressure;      /// 过压音频提醒
    uint8_t is_over_pressure_led;         /// 过压LED提示
    uint8_t is_remind_breast_pump_finish; /// 泵奶完成提醒
    uint8_t is_remind_exception;          /// 是否提醒泵奶异常
    // breast_pump_level_info_t level_info;
    uint16_t breast_pump_time;                  /// 泵奶时长 sys_breast_pumping_time_e 
} breast_pump_cfg_t;

/**
 * 语音提醒主题枚举定义
*/
typedef enum {
    SOUND_THEMES_0 = 0,
    SOUND_THEMES_GIRL,
    SOUND_THEMES_BOY,
    SOUND_THEMES_CHILD,
    SOUND_THEMES_OFFICIAL,  ///官方下载
    SOUND_THEMES_USER,       ///用户自定义
    SOUND_THEMES_MAX,
    SOUND_THEMES_DEFAULT = SOUND_THEMES_0,
} sound_themes_type_e;

typedef struct {
    uint8_t file_id;
    /// char filename[ROOT_FILENAME_LEN];
} audio_item_info_t;

typedef struct {
    /// 语音提醒开关.
    uint8_t cur_themes_type;
    uint8_t last_themes_type;
    audio_item_info_t sound_info[SOUND_THEMES_MAX];
} audio_packet_info_t;

typedef enum {
    THEME_PAKCET_ORIGINAL, 
    THEME_PACKET_OFFICIAL,
    THEME_PACKET_PUSH,   
    THEME_PACKET_GIFT,
    THEME_PACKET_CUSTOME,
    THEME_PACKET_MAX,
} theme_type_e;

typedef struct{
    uint8_t file_id;
    // char filename[ROOT_FILENAME_LEN];
} theme_item_info_t;

typedef struct {
    uint8_t cur_theme_type;  
    uint8_t last_theme_type;
    theme_item_info_t theme_info[THEME_PACKET_MAX];
} theme_packet_info_t;

/// 用户绑定信息
typedef struct {
    uint8_t user_id[USER_BIND_UID_LEN];     /// root id, eg. TF123456;
    uint8_t random[USER_BIND_RANDOM_LEN];   /// 随机数，绑定时由App 下发，用于计算aes key;
    uint8_t aes_key[USER_BIND_AES_KEY_LEN]; /// 对称密钥，数据加解密
} user_bind_info_t;

/// 描述系统配置
typedef struct {
    int8_t  time_zone;             /// 时区
    uint8_t ap_mode_onoff;         /// 飞航模式开关.
    uint8_t slient_upgrade_onoff;  /// 静默升级开关
    uint8_t sys_ui_style;          /// 系统UI风格
    uint8_t log_switch;            /// 系统UART日志开关
    uint8_t language;              /// 系统语言类型
    uint8_t sound_mode_onoff;      /// 语音提醒开关.
    uint8_t microseism_onoff;      /// 微震开关
    uint8_t ai_mode_onoff;         /// ai 模式开关.
} sys_config_info_t;

/// 描述系统配置
typedef struct {
    uint8_t guide_deployed;       /// 向导部署标志
    uint8_t battery_health;       /// 电池健康
    uint32_t log_filter;          /// 日志过滤
    uint32_t breast_pump_header_life;   /// 泵奶头寿命
    uint32_t sync_sys_time;       /// 保存第一次同步系统时间时 本地经历的最终时间点 用于计算泵奶记录的差值.
} sys_state_info_t;

/// 描述系统信息
typedef struct sys_persist_info {
    sys_config_info_t config;
    sys_state_info_t state;
    breast_pump_cfg_t breast_pump_cfg;
    user_bind_info_t bind_info;
    audio_packet_info_t audio_packet;    /// 语音包
    theme_packet_info_t theme_packet;    /// 主题包
} sys_persist_info_t;

/// 描述板子信息
typedef struct sys_board_config {
    const char *git_hash;
    const char *device_name;
    const char *buildtime;
    uint16_t firmware_type;
    uint16_t firmware_verno;
    uint16_t res_major_verno;
    uint16_t res_minor_verno;
    uint16_t product_id;
} sys_board_config_t;

/// 描述板子信息
typedef struct {
    char device_name[32];
    uint16_t sys_connect_type;
    uint16_t sys_local_protocol_id;
    uint16_t sys_peer_app_protocol_id;
    uint16_t sys_product_id;
    uint16_t ui_res_id;
    uint16_t hw_version;
    uint16_t sw_version;
    uint16_t sw_version_type;
    uint8_t dfu_type;  /// 0:dfu  1:fast dfu
} sec_sys_ver_t;

/// 工厂校准数据结构, 恢复出厂不擦除.
typedef struct factory_data_store {
    factory_flag_info_t flag_info;
    factory_dev_info_t dev_info;
    imu_cali_data_t imu;
} factory_data_store_t;

/// 泵奶数据头
typedef struct {
    uint8_t num;        /// 存储数量.
    uint8_t head;  
    uint8_t tail;
    uint8_t resv;
    uint16_t crc[ROOT_BREAST_PUMP_BODY_DATA_NUM + 1];  
} breast_pump_data_head_t;

/// 泵奶数据体
typedef struct {
    algo_breast_pump_data_t body;
} breast_pump_data_body_t;

#ifdef __cplusplus
}
#endif

#endif 


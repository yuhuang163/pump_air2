# 资源打包工具使用说明

## 📁 文件说明

### 1. `resource_config.json` - 资源配置文件
定义了资源ID到图片文件的映射关系，包含以下字段：
- **meta**: 元数据配置
  - `version`: 配置文件版本
  - `resource_type`: 资源类型 (IMG_RES_TYPE_UI/BASE/ALGO)
  - `language`: 语言类型 (IMG_RES_LANG_TYPE_ZH/EN)
  - `compressed`: 是否压缩
  
- **resources**: 资源列表
  - `resource_id`: 资源ID (对应枚举值)
  - `resource_name`: 资源名称 (对应枚举名)
  - `description`: 资源描述
  - `file`: 相对路径的图片文件
  - `x_offset`: X坐标偏移
  - `y_offset`: Y坐标偏移

### 2. `pack_resources.py` - 打包脚本
根据配置文件将图片资源打包成二进制资源包，符合以下数据结构：
```c
img_res_head_wrapper_t {
    img_res_head_desc_t head;           // 资源包头描述 (48字节)
    uint8_t md5_head[16];               // 头部MD5
    uint8_t md5_body[16];               // 数据体MD5
    img_res_obj_info_t obj_info[n];     // 资源信息数组
    uint8_t padding[];                  // 填充到32KB
    uint8_t res_body[];                 // 资源数据体
}
```

### 3. `unpack_resources.py` - 解包验证脚本
用于验证打包后的资源包是否正确，可以解析并显示资源包信息。

## 🚀 使用方法

### 1. 修改配置文件
编辑 `resource_config.json`，将对应的资源ID映射到实际的图片文件：

```json
{
  "resource_id": 1,
  "resource_name": "RESOURCE_ID_COMMON_PUMP_MODE_1",
  "description": "常规模式1",
  "file": "img_baj/your_actual_file.baj",  // 修改这里
  "x_offset": 0,
  "y_offset": 0
}
```

### 2. 执行打包
```bash
cd overlay/resources
python3 pack_resources.py resource_config.json resource_pack.bin
```

### 3. 验证打包结果
```bash
python3 unpack_resources.py resource_pack.bin
```

## 📝 配置文件填写指南

### 资源ID对照表
根据 `resource_id_define.h` 定义，当前支持以下资源ID：

| ID | 资源名称 | 描述 | 当前占位文件 |
|----|---------|------|------------|
| 1 | RESOURCE_ID_COMMON_PUMP_MODE_1 | 常规模式1 | pp_mode_360x360_a4_q99_422.baj |
| 2 | RESOURCE_ID_COMMON_PUMP_MODE_2 | 常规模式2 | pp_mode_360x360_a4_q99_422.baj |
| 3 | RESOURCE_ID_COMMON_PUMP_MODE_3 | 常规模式3 | pp_mode_360x360_a4_q99_422.baj |
| 4 | RESOURCE_ID_CUSTOM_PUMP_MODE_1 | 自定义模式1 | pp_mode_360x360_a4_q99_422.baj |
| 5 | RESOURCE_ID_CUSTOM_PUMP_MODE_2 | 自定义模式2 | pp_mode_360x360_a4_q99_422.baj |
| 6 | RESOURCE_ID_CUSTOM_PUMP_MODE_3 | 自定义模式3 | pp_mode_360x360_a4_q99_422.baj |
| 7 | RESOURCE_ID_CUSTOM_PUMP_MODE_4 | 自定义模式4 | pp_mode_360x360_a4_q99_422.baj |
| 8 | RESOURCE_ID_PUMP_FREQUENCY_LOW | 低频 | home21_270x270_a4_q99_422.baj |
| 9 | RESOURCE_ID_PUMP_FREQUENCY_MEDIUM | 中频 | home22_360x360_a4_q99_422.baj |
| 10 | RESOURCE_ID_PUMP_FREQUENCY_HIGH | 高频 | home23_180x180_a4_q99_422.baj |
| 11 | RESOURCE_ID_TRAVEL_LOCK_TIP | 旅行锁提示 | home31_270x270_a4_q99_422.baj |
| 12 | RESOURCE_ID_PUMP_COMPLETE_TIP | 泵奶完成提示 | home32_360x360_a4_q99_422.baj |
| 13 | RESOURCE_ID_BATTERY_LOW_TIP | 电池电量低提示 | 18_296x296_a4_q99_422.baj |
| 14 | RESOURCE_ID_BATTERY_EMPTY_TIP | 电池电量空提示 | 18_296x296_a4_q99_422.baj |
| 15-19 | RESOURCE_ID_BATTERY_LEVEL_5/4/3/2/1_TIP | 电池电量5-1格 | charge_1_360x360_a4_q99_422.baj |
| 20-25 | RESOURCE_ID_BATTERY_CHARGING_LEVEL_x_TIP | 充电电量提示 | charge_1_360x360_a4_q99_422.baj |
| 26 | RESOURCE_ID_BLE_DISCONNECTED_TIP | 蓝牙断开提示 | user_qrcode_360x360_a4_q99_422.baj |
| 27 | RESOURCE_ID_BLE_CONNECTED_TIP | 蓝牙连接提示 | user_qrcode_360x360_a4_q99_422.baj |
| 28 | RESOURCE_ID_LOGO | 品牌logo | logo_360x360_a4_q99_422.baj |

### 文件命名规范
建议图片文件名包含尺寸信息，格式为：`name_WIDTHxHEIGHT_xxx.baj`
- 例如：`mode1_360x360_a4_q99_422.baj` 表示360x360像素的图片
- 脚本会自动从文件名解析宽度和高度

## 🔧 技术细节

### 数据结构布局
```
+----------------------------+ 0x00000
| img_res_head_desc_t (36B)  | 资源包头描述
|  - magic (4B)              |
|  - crc16 (2B)              |
|  - flags (2B)              |
|  - type (2B)               |
|  - lang_id (2B)            |
|  - version (2B)            |
|  - res_count (2B)          |
|  - total_size (4B)         |
|  - reserved[4] (16B)       |
+----------------------------+ 0x00024 (36)
| MD5_head (16B)             | 头部MD5哈希
+----------------------------+ 0x00034 (52)
| MD5_body (16B)             | 数据体MD5哈希
+----------------------------+ 0x00044 (68)
| obj_info[0] (20B)          | 资源1信息
|  - img_id (2B)             |
|  - crc16 (2B)              |
|  - width (2B)              |
|  - height (2B)             |
|  - x_offset (2B)           |
|  - y_offset (2B)           |
|  - offset_addr (4B)        |
|  - data_size (4B)          |
+----------------------------+
| obj_info[1] (20B)          | 资源2信息
| ...                        |
| obj_info[n-1] (20B)        | 资源n信息
+----------------------------+
| Padding (0xFF填充)         | 填充到32KB
+----------------------------+ 0x08000 (32KB)
| Resource Data 1            | 资源1数据
| Resource Data 2            | 资源2数据
| ...                        |
| Resource Data n            | 资源n数据
+----------------------------+
```

### 数据结构详细说明

#### img_res_head_desc_t (36字节)
| 字段 | 类型 | 大小 | 说明 |
|------|------|------|------|
| magic | uint32_t | 4B | 魔数，固定为0x52534547 ("GESR") |
| crc16 | uint16_t | 2B | 头部CRC16校验值（从flags开始计算） |
| flags | uint16_t | 2B | 标志位（bit0:是否压缩，其他保留） |
| type | uint16_t | 2B | 资源包类型（0=BASE, 1=UI, 2=ALGO） |
| lang_id | uint16_t | 2B | 语言ID（0=EN, 1=ZH） |
| version | uint16_t | 2B | 版本号（0xMMmm，如0x0100=v1.0） |
| res_count | uint16_t | 2B | 资源数量 |
| total_size | uint32_t | 4B | 整个资源包总大小（字节） |
| reserved | uint32_t[4] | 16B | 预留字段 |

#### img_res_obj_info_t (20字节)
| 字段 | 类型 | 大小 | 说明 |
|------|------|------|------|
| img_id | uint16_t | 2B | 图片资源ID（对应枚举值） |
| crc16 | uint16_t | 2B | 单个资源CRC16校验 |
| width | uint16_t | 2B | 图片宽度（像素） |
| height | uint16_t | 2B | 图片高度（像素） |
| x_offset | uint16_t | 2B | X坐标偏移（UI布局用） |
| y_offset | uint16_t | 2B | Y坐标偏移（UI布局用） |
| offset_addr | uint32_t | 4B | 资源数据相对32KB后的偏移 |
| data_size | uint32_t | 4B | 资源数据大小（字节） |

### CRC16/MD5计算
- **Head CRC16**: 对 head_desc 字段（跳过magic和crc16本身）计算
- **Head MD5**: 对整个 head_desc 计算
- **Body MD5**: 对 obj_info数组 + res_body 数据计算
- **单个资源CRC16**: 对每个资源数据单独计算

## ❓ 常见问题

### Q: 如何添加新资源？
A: 在 `resource_config.json` 的 `resources` 数组中添加新条目，确保 `resource_id` 与头文件枚举对应。

### Q: 支持哪些图片格式？
A: 当前支持 `.baj` 格式，可根据需要扩展支持其他格式。

### Q: 资源包最大支持多少资源？
A: 理论上最大支持 `(32KB - 48 - 32) / 20 = 1636` 个资源。

### Q: 如何验证打包是否正确？
A: 运行 `unpack_resources.py` 解包验证，检查魔数、版本、CRC/MD5等信息。

## 📞 联系方式
如有问题请联系项目维护者。


# 🚀 快速开始 - 资源打包工具

## 📦 已生成的文件

```
overlay/resources/
├── 📄 resource_config.json      (6.7K)  - 资源配置文件 ⭐
├── 🐍 pack_resources.py         (9.5K)  - 打包脚本 ⭐
├── 🐍 unpack_resources.py       (9.9K)  - 解包验证脚本
├── 📘 README_PACK.md            (7.4K)  - 详细文档
├── 📗 SUMMARY.md                (5.7K)  - 开发总结
├── 📙 QUICK_START.md            (本文件) - 快速参考
├── 🔧 example_usage.sh          (1.2K)  - 使用示例
└── 📦 resource_pack.bin         (472K)  - 生成的资源包（示例）
```

**⭐ = 你需要关注的核心文件**

## ⚡ 3分钟快速上手

### 第1步：修改配置文件

打开 `resource_config.json`，找到你要修改的资源ID，更新文件路径：

```json
{
  "resource_id": 28,
  "resource_name": "RESOURCE_ID_LOGO",
  "description": "品牌logo",
  "file": "img_baj/logo_360x360_a4_q99_422.baj",  // ← 修改这里
  "x_offset": 0,
  "y_offset": 0
}
```

### 第2步：打包资源

```bash
cd overlay/resources
python3 pack_resources.py resource_config.json resource_pack.bin
```

### 第3步：验证结果

```bash
python3 unpack_resources.py resource_pack.bin
```

看到 `✅ 所有资源验证通过!` 就说明打包成功了！

## 📋 资源ID快速对照表

| ID | 名称 | 说明 | 当前占位文件 |
|----|------|------|-------------|
| 1-7 | PUMP_MODE_x | 泵奶模式 | pp_mode_360x360 |
| 8-10 | PUMP_FREQUENCY_x | 频率设置 | home2x_xxx |
| 11-12 | xxx_TIP | 提示信息 | home3x_xxx |
| 13-14 | BATTERY_xxx_TIP | 电池提示 | 18_296x296 |
| 15-25 | BATTERY_LEVEL_x | 电量显示 | charge_1_360x360 |
| 26-27 | BLE_xxx_TIP | 蓝牙状态 | user_qrcode_360x360 |
| 28 | LOGO | 品牌LOGO | logo_360x360 |

**💡 提示：** 所有文件当前都是占位文件，请根据你的实际UI资源替换！

## 🎯 常用命令

### 打包资源
```bash
python3 pack_resources.py resource_config.json output.bin
```

### 验证资源包
```bash
python3 unpack_resources.py output.bin
```

### 提取单个资源
```bash
python3 unpack_resources.py output.bin <资源ID> output.baj
# 例如：提取LOGO（ID=28）
python3 unpack_resources.py resource_pack.bin 28 logo.baj
```

### 查看帮助
```bash
python3 pack_resources.py
python3 unpack_resources.py
```

### 运行完整示例
```bash
./example_usage.sh
```

## 📐 文件命名规范

建议图片文件命名包含尺寸信息，脚本会自动解析：

```
✅ 推荐: mode1_360x360_a4_q99_422.baj  → 自动识别为360x360
✅ 推荐: battery_296x296_high.baj     → 自动识别为296x296
❌ 不推荐: image001.baj                → 无法识别尺寸，使用默认值
```

格式：`任意名称_宽度x高度_任意后缀.baj`

## 🔍 验证检查项

打包和验证时会自动检查以下项目：

- ✅ 魔数：0x52534547 ("GESR")
- ✅ 版本兼容性：v1.0
- ✅ 头部CRC16校验
- ✅ 头部MD5哈希
- ✅ 数据体MD5哈希
- ✅ 每个资源的CRC16
- ✅ 数据完整性

## 📊 数据结构（技术参考）

```
资源包 = 固定32KB头部 + 资源数据体
         └─ 头部描述(36B) + MD5x2(32B) + 资源信息数组(20B×N) + 填充
```

### 头部结构 (36字节)
- magic: 0x52534547 (4B)
- crc16: 校验和 (2B)
- flags: 标志位 (2B) - bit0=压缩
- type: 资源类型 (2B) - 0=BASE, 1=UI, 2=ALGO
- lang_id: 语言 (2B) - 0=EN, 1=ZH
- version: 版本 (2B) - 0x0100=v1.0
- res_count: 资源数 (2B)
- total_size: 总大小 (4B)
- reserved: 预留 (16B)

### 资源信息 (20字节/个)
- img_id: 资源ID (2B)
- crc16: 校验和 (2B)
- width: 宽度 (2B)
- height: 高度 (2B)
- x_offset: X偏移 (2B)
- y_offset: Y偏移 (2B)
- offset_addr: 数据偏移 (4B)
- data_size: 数据大小 (4B)

## 🛠️ 常见问题

### Q: 如何添加新资源？
在 `resource_config.json` 的 `resources` 数组中添加新条目，确保 `resource_id` 唯一。

### Q: 支持压缩吗？
数据结构支持（flags bit0），但当前脚本未实现，需要扩展。

### Q: 最多支持多少资源？
理论最大：`(32KB - 68) / 20 ≈ 1636` 个资源。

### Q: 文件不存在怎么办？
打包时会显示警告但继续处理其他资源。建议修正配置后重新打包。

### Q: 如何集成到构建系统？
在Makefile中添加：
```makefile
resource_pack.bin: resource_config.json
	cd overlay/resources && python3 pack_resources.py resource_config.json resource_pack.bin
```

## 📚 详细文档

- **README_PACK.md** - 完整的使用文档和技术细节
- **SUMMARY.md** - 开发总结和后续工作指南
- **resource_id_define.h** - C语言资源ID枚举定义
- **resource_img_define.h** - C语言数据结构定义

## ✨ 下一步

1. ✏️ 修改 `resource_config.json` 中的文件路径
2. 🔨 运行打包脚本生成资源包
3. ✅ 验证资源包正确性
4. 🚀 集成到你的固件构建流程
5. 📦 部署到设备

---

**提示：** 如果你是第一次使用，建议先运行 `./example_usage.sh` 看看效果！


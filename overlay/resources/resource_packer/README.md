# 资源打包工具

## 📁 目录结构

```
overlay/resources/
├── resource_config.json       # 资源配置文件
├── resource_id_define.h       # 资源ID定义头文件
├── resource_pack.bin          # 生成的资源包（打包后）
├── img_baj/                   # 资源文件目录
│   └── *.baj                  # 图片资源文件
└── resource_packer/           # 资源打包工具 ⭐
    ├── pack_resources.py      # 打包脚本（核心）
    ├── pack.sh                # 快速打包脚本
    ├── README.md              # 本文档
    ├── QUICK_START.md         # 快速开始指南
    ├── README_PACK.md         # 详细文档
    └── CHANGELOG.md           # 更新日志
```

## 🚀 快速开始

### 1. 最简单的方式

```bash
cd /path/to/overlay/resources/resource_packer
./pack.sh
```

这会使用默认配置文件打包资源。

### 2. 指定配置文件

```bash
./pack.sh ../resource_config.json ../output.bin
```

### 3. 使用Python脚本

```bash
python3 pack_resources.py ../resource_config.json ../resource_pack.bin ../resource_id_define.h
```

## 📝 配置文件说明

配置文件位于 `../resource_config.json`，格式如下：

```json
{
  "meta": {
    "version": "1.0",
    "description": "资源包配置文件",
    "resource_type": "IMG_RES_TYPE_UI",
    "language": "IMG_RES_LANG_TYPE_ZH",
    "compressed": false
  },
  "resources": [
    {
      "resource_id": "RESOURCE_ID_LOGO",  // 使用宏名称
      "description": "品牌logo",
      "file": "img_baj/logo.baj",         // 相对于resources目录
      "x_offset": 0,
      "y_offset": 0
    }
  ]
}
```

### 配置字段说明

- **resource_id**: 资源ID，支持宏名称（如 `"RESOURCE_ID_LOGO"`）或数字（如 `28`）
- **description**: 资源描述
- **file**: 资源文件路径，相对于 `resources/` 目录
- **x_offset/y_offset**: 坐标偏移（用于UI布局）

## 🔧 工具说明

### pack_resources.py

核心打包脚本，功能：
- ✅ 读取JSON配置文件
- ✅ 自动解析头文件获取资源ID
- ✅ 支持使用宏名称或数字ID
- ✅ 自动计算CRC16/MD5校验
- ✅ 自动从文件名解析图片尺寸
- ✅ 生成符合C结构体定义的二进制资源包

**使用方法**：
```bash
python3 pack_resources.py <config.json> [output.bin] [header.h]
```

**参数**：
- `config.json` - 资源配置文件（必需）
- `output.bin` - 输出文件（可选，默认 resource_pack.bin）
- `header.h` - 资源ID头文件（可选，自动查找）

### pack.sh

快速打包脚本，自动处理路径，更方便使用。

**使用方法**：
```bash
./pack.sh [config.json] [output.bin]
```

## 📊 数据结构

打包后的资源包符合以下C结构体定义：

```c
typedef struct img_res_head_wrapper {
    img_res_head_desc_t head;            // 36字节
    uint8_t md5_head[16];                // 16字节
    uint8_t md5_body[16];                // 16字节
    img_res_obj_info_t obj_info[n];     // 20字节 × n
    uint8_t padding[];                   // 填充到32KB
    uint8_t res_body[];                  // 资源数据体
} __attribute__((packed)) img_res_head_wrapper_t;
```

**布局**：
- 固定32KB头部（包含所有元数据和资源信息）
- 资源数据体（从32KB偏移后开始）

## 💡 使用技巧

### 1. 文件命名规范

建议资源文件命名包含尺寸信息：
```
mode1_360x360_a4_q99_422.baj
battery_296x296_high.baj
logo_180x180.baj
```

脚本会自动从文件名解析宽度和高度。

### 2. 路径管理

配置文件中的路径都相对于 `resources/` 目录：
```json
{
  "file": "img_baj/logo.baj"  // ✅ 正确
  // 不要用绝对路径
}
```

### 3. 资源ID管理

**推荐使用宏名称**：
```json
{
  "resource_id": "RESOURCE_ID_LOGO"  // ✅ 推荐：直观易懂
}
```

也支持数字ID：
```json
{
  "resource_id": 28  // ✅ 也可以，但不如宏名称直观
}
```

### 4. 集成到构建系统

在 Makefile 中添加：
```makefile
resource_pack.bin: resource_config.json
	cd overlay/resources/resource_packer && ./pack.sh
```

## 🐛 常见问题

### Q1: 打包失败，提示文件不存在

**A**: 检查配置文件中的路径：
```bash
# 查看配置
cat ../resource_config.json

# 检查文件是否存在
ls -la ../img_baj/
```

### Q2: 提示"无法解析资源ID"

**A**: 确保：
1. 头文件存在：`ls -la ../resource_id_define.h`
2. 资源ID拼写正确
3. 或者直接使用数字ID

### Q3: 生成的资源包很大

**A**: 这是正常的，资源包包含：
- 固定32KB头部
- 所有图片的原始数据

可以检查每个资源的大小，删除不需要的资源。

## 📚 更多文档

- **QUICK_START.md** - 3分钟快速上手
- **README_PACK.md** - 完整技术文档
- **CHANGELOG.md** - 版本更新历史

## ⚡ 常用命令速查

```bash
# 进入工具目录
cd /path/to/overlay/resources/resource_packer

# 快速打包（使用默认配置）
./pack.sh

# 指定配置文件打包
./pack.sh ../my_config.json

# 查看配置
cat ../resource_config.json

# 编辑配置
vim ../resource_config.json

# 查看生成的资源包
ls -lh ../resource_pack.bin

# 查看资源文件列表
ls -lh ../img_baj/
```

## 📞 技术支持

如有问题，请查看：
1. 本文档的常见问题部分
2. `README_PACK.md` 详细技术文档
3. `CHANGELOG.md` 了解最新变化

---

**版本**: v1.2  
**最后更新**: 2025-12-22


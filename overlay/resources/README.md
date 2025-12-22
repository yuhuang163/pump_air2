# 资源管理目录

## 📁 目录结构

```
overlay/resources/
├── README.md                  # 本文档（目录说明）
├── resource_config.json       # 资源配置文件 ⭐
├── resource_id_define.h       # 资源ID定义头文件
├── resource_pack.bin          # 生成的资源包（打包后生成）
│
├── img_baj/                   # BAJ格式资源文件目录
│   ├── logo_360x360.baj
│   ├── mode1_360x360.baj
│   └── ...
│
├── img_bin/                   # BIN格式资源文件
├── img_bin_png/               # PNG格式资源文件
├── img/                       # 其他格式资源文件
│
└── resource_packer/           # 资源打包工具 🔧
    ├── README.md              # 工具使用说明
    ├── pack.sh                # 快速打包脚本 ⭐
    ├── pack_resources.py      # 核心打包脚本
    ├── QUICK_START.md         # 快速开始指南
    ├── README_PACK.md         # 详细技术文档
    ├── CHANGELOG.md           # 版本更新日志
    └── SUMMARY.md             # 开发总结
```

## 🚀 快速开始

### 1. 打包资源（最简单）

```bash
cd /path/to/overlay/resources/resource_packer
./pack.sh
```

### 2. 修改资源配置

编辑 `resource_config.json`：
```bash
vim resource_config.json
```

### 3. 查看打包结果

```bash
ls -lh resource_pack.bin
```

## 📝 配置文件说明

`resource_config.json` 是资源配置的核心文件，定义了所有资源的映射关系。

**示例**：
```json
{
  "meta": {
    "resource_type": "IMG_RES_TYPE_UI",
    "language": "IMG_RES_LANG_TYPE_ZH"
  },
  "resources": [
    {
      "resource_id": "RESOURCE_ID_LOGO",
      "description": "品牌logo",
      "file": "img_baj/logo_360x360.baj",
      "x_offset": 0,
      "y_offset": 0
    }
  ]
}
```

## 🔧 工具使用

所有打包工具都在 `resource_packer/` 目录下：

### 快速打包
```bash
cd resource_packer
./pack.sh
```

### 指定配置文件
```bash
cd resource_packer
./pack.sh ../my_config.json ../output.bin
```

### 查看工具文档
```bash
cd resource_packer
cat README.md
```

## 📚 文档索引

| 文档 | 说明 | 位置 |
|------|------|------|
| **README.md** | 目录说明（本文档） | `./README.md` |
| **resource_packer/README.md** | 工具使用说明 | `resource_packer/README.md` |
| **resource_packer/QUICK_START.md** | 3分钟快速上手 | `resource_packer/QUICK_START.md` |
| **resource_packer/README_PACK.md** | 完整技术文档 | `resource_packer/README_PACK.md` |
| **resource_packer/CHANGELOG.md** | 版本更新历史 | `resource_packer/CHANGELOG.md` |

## 💡 工作流程

```
1. 准备资源文件
   ↓
2. 编辑 resource_config.json
   ↓
3. 运行 resource_packer/pack.sh
   ↓
4. 生成 resource_pack.bin
   ↓
5. 集成到固件
```

## 🎯 资源文件要求

### 文件命名规范

建议使用包含尺寸信息的命名：
```
name_WIDTHxHEIGHT_xxx.baj
```

**示例**：
- ✅ `logo_360x360_a4_q99_422.baj`
- ✅ `mode1_270x270.baj`
- ⚠️ `image001.baj` (无法自动解析尺寸)

### 文件存放位置

- **BAJ格式**: `img_baj/`
- **BIN格式**: `img_bin/`
- **PNG格式**: `img_bin_png/`
- **其他格式**: `img/`

## 🔍 常见任务

### 添加新资源

1. 将资源文件放入对应目录（如 `img_baj/`）
2. 编辑 `resource_config.json`，添加新条目
3. 运行 `tools/pack.sh` 重新打包

### 修改现有资源

1. 替换资源文件（保持文件名不变）
2. 运行 `tools/pack.sh` 重新打包

### 删除资源

1. 从 `resource_config.json` 删除对应条目
2. （可选）删除资源文件
3. 运行 `tools/pack.sh` 重新打包

### 查看资源列表

```bash
# 方式1：查看配置文件
cat resource_config.json

# 方式2：查看资源目录
ls -lh img_baj/
```

## ⚙️ 集成到构建系统

在项目的 Makefile 中添加：

```makefile
# 资源打包目标
resource_pack.bin: overlay/resources/resource_config.json
	@echo "打包资源..."
	@cd overlay/resources/resource_packer && ./pack.sh
	@echo "资源打包完成"

# 清理资源包
clean_resources:
	rm -f overlay/resources/resource_pack.bin

.PHONY: resource_pack.bin clean_resources
```

## 📊 技术规格

- **资源包格式**: 二进制
- **头部大小**: 固定32KB
- **最大资源数**: 约1636个
- **支持的ID格式**: 宏名称或数字
- **校验方式**: CRC16 + MD5
- **版本**: v1.2

## 🐛 故障排查

### 打包失败

1. 检查配置文件格式是否正确
2. 确认所有资源文件存在
3. 查看错误信息

### 文件找不到

- 确保路径相对于 `resources/` 目录
- 检查文件名大小写
- 使用 `ls` 命令确认文件存在

### 更多帮助

查看详细文档：
```bash
cd resource_packer
cat README.md
```

## 📞 获取帮助

- 查看 `resource_packer/README.md` - 工具详细说明
- 查看 `resource_packer/QUICK_START.md` - 快速开始指南
- 查看 `resource_packer/README_PACK.md` - 技术文档

---

**版本**: v1.2  
**最后更新**: 2025-12-22  
**维护**: 资源打包工具团队


# 资源打包工具 - 更新日志

## v1.1 - 2025-12-22

### ✨ 新特性

#### 支持使用宏名称作为资源ID

现在配置文件中的 `resource_id` 字段支持两种格式：

1. **宏名称**（推荐）：
   ```json
   {
     "resource_id": "RESOURCE_ID_LOGO",
     "description": "品牌logo",
     "file": "img_baj/logo.baj"
   }
   ```

2. **数字ID**（兼容旧格式）：
   ```json
   {
     "resource_id": 28,
     "description": "品牌logo",
     "file": "img_baj/logo.baj"
   }
   ```

### 🎯 优势

- **更直观**：使用 `RESOURCE_ID_LOGO` 比数字 `28` 更容易理解
- **更安全**：与头文件自动对照，避免ID错误
- **更方便**：修改头文件枚举顺序时，配置文件无需改动
- **自动解析**：脚本自动从 `resource_id_define.h` 解析枚举定义

### 📖 使用方法

#### 自动模式（推荐）
脚本会自动在配置文件同目录查找 `resource_id_define.h`：
```bash
python3 pack_resources.py resource_config.json resource_pack.bin
```

#### 手动指定头文件
```bash
python3 pack_resources.py resource_config.json resource_pack.bin path/to/resource_id_define.h
```

### 🔧 技术实现

- 添加了 `parse_resource_id_header()` 函数，解析C枚举定义
- 支持十六进制（0x01）和十进制（1）枚举值
- 兼容带行尾注释的枚举项
- 自动处理逗号和空行

### 📝 配置文件更新

`resource_config.json` 已更新为使用宏名称格式：
```json
{
  "resources": [
    {
      "resource_id": "RESOURCE_ID_COMMON_PUMP_MODE_1",
      "description": "常规模式1",
      "file": "img_baj/pp_mode_360x360_a4_q99_422.baj",
      "x_offset": 0,
      "y_offset": 0
    }
  ]
}
```

**注意：** `resource_name` 字段现在是可选的，如果使用宏名称作为ID，可以省略此字段。

### ✅ 验证

所有测试通过：
- ✅ 宏名称解析正确
- ✅ 数字ID仍然兼容
- ✅ 打包结果与之前一致
- ✅ 所有验证通过（CRC16/MD5）

---

## v1.0 - 2025-12-22

### 初始版本

#### 核心功能
- 资源打包（JSON配置 → 二进制资源包）
- 资源解包验证
- CRC16/MD5校验
- 完整文档和示例

#### 文件清单
- `pack_resources.py` - 打包脚本
- `unpack_resources.py` - 解包验证脚本
- `resource_config.json` - 资源配置文件
- `README_PACK.md` - 详细文档
- `QUICK_START.md` - 快速开始
- `SUMMARY.md` - 开发总结
- `example_usage.sh` - 使用示例

#### 数据结构
符合 `img_res_head_wrapper_t` 定义：
- 固定32KB头部
- CRC16/MD5完整性校验
- 支持最多1636个资源

---

## 后续计划

### 可能的扩展功能
- [ ] 资源压缩支持
- [ ] 多语言资源包（EN/ZH切换）
- [ ] 增量更新
- [ ] 自动化测试脚本
- [ ] GUI配置工具
- [ ] 资源使用报告生成


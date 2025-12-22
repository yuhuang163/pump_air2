# 资源打包工具 - 开发总结

## 📦 已创建的文件

### 1. 配置文件
- **`resource_config.json`** - 资源配置文件
  - 定义了28个资源ID到图片文件的映射关系
  - 使用现有的`img_baj`目录下的文件作为占位
  - 你可以根据实际需求修改`file`字段指向正确的资源文件

### 2. Python脚本
- **`pack_resources.py`** - 资源打包脚本（主要工具）
  - 读取配置文件并打包成二进制资源包
  - 自动计算CRC16/MD5校验值
  - 自动从文件名解析图片尺寸
  - 按照C数据结构打包（符合`img_res_head_wrapper_t`定义）
  
- **`unpack_resources.py`** - 资源解包验证脚本
  - 解析并验证打包后的资源包
  - 检查魔数、版本、CRC16、MD5等
  - 可以提取单个资源用于调试

### 3. 文档
- **`README_PACK.md`** - 详细使用文档
  - 工具使用方法
  - 数据结构说明
  - 资源ID对照表
  - 常见问题解答
  
- **`SUMMARY.md`** - 本文档（开发总结）

### 4. 示例脚本
- **`example_usage.sh`** - 快速开始示例
  - 演示完整的打包、验证、提取流程

## 🎯 资源ID映射说明

配置文件中已经为所有28个资源ID创建了映射关系：

| 资源ID范围 | 类型 | 占位文件 |
|-----------|------|---------|
| 1-7 | 泵奶模式 | pp_mode_360x360_a4_q99_422.baj |
| 8-10 | 频率设置 | home21/22/23_xxx.baj |
| 11-12 | 提示信息 | home31/32_xxx.baj |
| 13-14 | 电池提示 | 18_296x296_a4_q99_422.baj |
| 15-25 | 电池电量显示 | charge_1_360x360_a4_q99_422.baj |
| 26-27 | 蓝牙连接状态 | user_qrcode_360x360_a4_q99_422.baj |
| 28 | 品牌LOGO | logo_360x360_a4_q99_422.baj |

**⚠️ 注意：这些都是占位文件，你需要根据实际UI设计替换为正确的资源文件！**

## 🚀 快速开始

### 基本使用流程

```bash
cd overlay/resources

# 1. 修改配置文件（根据你的实际资源）
vim resource_config.json

# 2. 打包资源
python3 pack_resources.py resource_config.json resource_pack.bin

# 3. 验证打包结果
python3 unpack_resources.py resource_pack.bin

# 4. 提取单个资源（可选，用于调试）
python3 unpack_resources.py resource_pack.bin 28 extracted_logo.baj
```

### 或者运行示例脚本

```bash
./example_usage.sh
```

## 📊 数据结构符合性

打包脚本严格按照以下C头文件定义实现：

### 数据结构来源
- `trunk/root_source/resource/resource_img_define.h` - 定义了资源包数据结构
- `overlay/resources/resource_id_define.h` - 定义了资源ID枚举

### 结构布局
```
资源包布局 (总大小: 32KB头部 + 资源数据体)
├── img_res_head_desc_t (36字节)
│   ├── magic: 0x52534547 ("GESR")
│   ├── crc16: 头部校验和
│   ├── flags: 压缩标志
│   ├── type: 资源类型 (UI/BASE/ALGO)
│   ├── lang_id: 语言ID (ZH/EN)
│   ├── version: 版本号 (0x0100)
│   ├── res_count: 资源数量
│   ├── total_size: 总大小
│   └── reserved[4]: 预留字段
├── MD5_head (16字节)
├── MD5_body (16字节)
├── obj_info数组 (每个20字节)
│   └── 包含：ID、CRC16、宽高、偏移、大小等
├── 填充 (0xFF填充到32KB)
└── 资源数据体 (从32KB偏移开始)
```

## ✅ 验证结果

已通过以下验证：
- ✅ 魔数正确 (0x52534547)
- ✅ 版本兼容 (v1.0)
- ✅ 头部CRC16校验通过
- ✅ 头部MD5验证通过
- ✅ 所有28个资源CRC16校验通过
- ✅ 资源数据完整性验证通过

当前生成的测试资源包：
- 文件大小：482,861 字节 (471.54 KB)
- 头部大小：32,768 字节 (固定32KB)
- 数据体大小：450,093 字节
- 资源数量：28个

## 🔧 后续工作

### 你需要做的事情：

1. **更新资源文件映射**
   - 打开`resource_config.json`
   - 根据实际的UI设计文件，修改每个资源ID对应的`file`字段
   - 确保文件路径正确（相对于`overlay/resources/`目录）

2. **调整坐标偏移（如果需要）**
   - 如果UI布局需要特定的坐标偏移，修改`x_offset`和`y_offset`字段
   - 默认都是(0,0)

3. **文件命名规范**
   - 建议图片文件名包含尺寸信息：`name_WIDTHxHEIGHT_xxx.baj`
   - 脚本会自动从文件名解析宽度和高度
   - 例如：`mode1_360x360_a4_q99_422.baj`

4. **重新打包**
   - 修改配置后重新运行打包脚本
   - 验证新的资源包是否正确

5. **集成到构建系统**
   - 可以将打包命令集成到你的Makefile或构建脚本中
   - 在固件编译前自动打包资源

### 可选扩展功能：

- 支持资源压缩（需要实现压缩/解压缩逻辑）
- 支持多语言资源包（创建不同的配置文件）
- 支持增量更新（只打包修改过的资源）
- 生成资源使用报告（资源大小统计等）

## 📝 资源配置文件示例

```json
{
  "resource_id": 1,
  "resource_name": "RESOURCE_ID_COMMON_PUMP_MODE_1",
  "description": "常规模式1",
  "file": "img_baj/mode1_360x360.baj",  // 修改这里指向实际文件
  "x_offset": 0,
  "y_offset": 0
}
```

## 🐛 问题排查

### 如果打包失败：
1. 检查配置文件JSON格式是否正确
2. 确认资源文件路径存在
3. 检查文件权限是否可读

### 如果验证失败：
1. 检查魔数是否正确（应该是"GESR"）
2. 查看CRC16/MD5校验信息
3. 使用解包工具查看详细信息

### 调试技巧：
- 使用`python3 unpack_resources.py resource_pack.bin`查看资源包详情
- 提取单个资源对比原始文件
- 检查打包脚本的输出日志

## 📞 联系信息

如有问题或需要功能扩展，请联系开发团队。

---

**生成时间：** 2025-12-22  
**工具版本：** v1.0  
**状态：** ✅ 已完成并验证通过


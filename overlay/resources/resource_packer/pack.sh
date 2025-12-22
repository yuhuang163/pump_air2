#!/bin/bash
###
### 资源打包脚本
### 使用方法: ./pack.sh [config.json] [output.bin]
###

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PARENT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

### 默认参数
CONFIG_FILE="${1:-$PARENT_DIR/resource_config.json}"
OUTPUT_FILE="${2:-$PARENT_DIR/resource_pack.bin}"
HEADER_FILE="$PARENT_DIR/resource_id_define.h"

### 显示信息
echo "=================================================="
echo "🚀 资源打包工具"
echo "=================================================="
echo "配置文件: $CONFIG_FILE"
echo "输出文件: $OUTPUT_FILE"
echo "头文件:   $HEADER_FILE"
echo ""

### 检查配置文件
if [ ! -f "$CONFIG_FILE" ]; then
    echo "❌ 错误: 配置文件不存在: $CONFIG_FILE"
    echo ""
    echo "使用方法:"
    echo "  ./pack.sh [config.json] [output.bin]"
    echo ""
    echo "示例:"
    echo "  ./pack.sh                                    # 使用默认配置"
    echo "  ./pack.sh ../resource_config.json           # 指定配置文件"
    echo "  ./pack.sh ../config.json ../output.bin      # 指定输入输出"
    exit 1
fi

### 执行打包
python3 "$SCRIPT_DIR/pack_resources.py" \
    "$CONFIG_FILE" \
    "$OUTPUT_FILE" \
    "$HEADER_FILE"

### 显示结果
if [ $? -eq 0 ]; then
    echo ""
    echo "=================================================="
    echo "✅ 打包成功！"
    echo "=================================================="
    echo "资源包位置: $OUTPUT_FILE"
    if [ -f "$OUTPUT_FILE" ]; then
        SIZE=$(ls -lh "$OUTPUT_FILE" | awk '{print $5}')
        echo "文件大小: $SIZE"
    fi
    echo ""
    echo "💡 提示:"
echo "  - 查看配置: cat $CONFIG_FILE"
echo "  - 修改资源: vim $CONFIG_FILE"
echo "  - 查看文档: cat resource_packer/README.md"
else
    echo ""
    echo "❌ 打包失败！请检查错误信息"
    exit 1
fi


#!/bin/bash
###
### 资源打包工具使用示例
### 本脚本位于 tools/ 目录，用于演示如何使用打包工具
###

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PARENT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "======================================"
echo "资源打包工具使用示例"
echo "======================================"
echo "工具目录: $SCRIPT_DIR"
echo "资源目录: $PARENT_DIR"
echo ""

### 1. 打包资源
echo "步骤 1: 打包资源..."
python3 "$SCRIPT_DIR/pack_resources.py" \
    "$PARENT_DIR/resource_config.json" \
    "$PARENT_DIR/resource_pack.bin" \
    "$PARENT_DIR/resource_id_define.h"

### 2. 查看生成的文件
echo ""
echo "======================================"
echo "生成的文件:"
echo "======================================"
ls -lh "$PARENT_DIR/resource_pack.bin" 2>/dev/null || echo "未找到资源包文件"

echo ""
echo "✅ 打包完成！"
echo ""
echo "接下来你可以："
echo "  1. 修改 resource_config.json 中的资源映射"
echo "  2. 重新运行打包:"
echo "     cd tools && ./example_usage.sh"
echo "  或者:"
echo "     cd tools"
echo "     python3 pack_resources.py ../resource_config.json ../resource_pack.bin"
echo ""
echo "  3. 查看文档:"
echo "     - README_PACK.md    - 完整文档"
echo "     - QUICK_START.md    - 快速开始"
echo "     - CHANGELOG.md      - 更新日志"


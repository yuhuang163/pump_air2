#!/bin/bash

# 脚本：将目录下所有文件名中的"追奶"替换为"boost_lactation"
# 用法：./rename_files.sh

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "开始重命名文件..."
echo "目录: $DIR"

# 计数器
count=0

# 遍历所有以"追奶_"开头的文件
for file in "$DIR"/追奶_*.png; do
    if [ -f "$file" ]; then
        # 获取文件名（不含路径）
        filename=$(basename "$file")
        # 替换"追奶"为"boost_lactation"
        new_filename="${filename/追奶/boost_lactation}"
        # 构建新的完整路径
        new_file="$DIR/$new_filename"
        
        # 检查新文件名是否已存在
        if [ -f "$new_file" ]; then
            echo "警告: $new_filename 已存在，跳过 $filename"
        else
            # 重命名文件
            mv "$file" "$new_file"
            echo "重命名: $filename -> $new_filename"
            ((count++))
        fi
    fi
done

echo "完成！共重命名 $count 个文件。"


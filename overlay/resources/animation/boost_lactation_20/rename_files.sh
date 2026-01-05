#!/bin/bash

# 脚本：将目录下所有文件名改为 boost_lactation_XXXXX.png 格式
# 用法：./rename_files.sh

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "开始重命名文件..."
echo "目录: $DIR"

# 计数器
count=0

# 遍历所有PNG文件
for file in "$DIR"/*.png; do
    if [ -f "$file" ]; then
        # 获取文件名（不含路径）
        filename=$(basename "$file")
        
        # 如果已经是正确的文件名格式，跳过
        if [[ $filename =~ ^boost_lactation_[0-9]{5}\.png$ ]]; then
            continue
        fi
        
        # 提取序号部分（文件名最后的下划线和数字部分，如 _00000）
        # 使用正则表达式匹配最后的 _数字.png 部分
        if [[ $filename =~ _([0-9]+)\.png$ ]]; then
            sequence="${BASH_REMATCH[1]}"
            # 将序号格式化为5位数字（如 00000, 00001）
            # 使用 10# 前缀强制将数字解释为十进制，避免八进制问题
            sequence=$(printf "%05d" $((10#$sequence)))
            new_filename="boost_lactation_${sequence}.png"
            new_file="$DIR/$new_filename"
            
            # 如果目标文件已存在且不是当前文件，使用临时文件名
            if [ -f "$new_file" ] && [ "$file" != "$new_file" ]; then
                # 使用临时文件名避免冲突
                temp_file="$DIR/temp_${sequence}_$$.png"
                mv "$file" "$temp_file"
                # 如果目标文件存在，先备份或删除（这里选择删除，因为可能是错误序号的文件）
                if [ -f "$new_file" ]; then
                    rm "$new_file"
                fi
                mv "$temp_file" "$new_file"
                echo "重命名（处理冲突）: $filename -> $new_filename"
            else
                # 重命名文件
                mv "$file" "$new_file"
                echo "重命名: $filename -> $new_filename"
            fi
            ((count++))
        else
            echo "警告: 无法解析文件名格式: $filename"
        fi
    fi
done

echo "完成！共重命名 $count 个文件。"


#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
PNG图片转TXT数据文件工具
将PNG图片文件转换为十六进制格式的文本文件，用于嵌入式系统
"""

import os
import sys
import argparse
import struct


def get_png_resolution(png_data):
    """
    从PNG二进制数据中解析分辨率
    
    Args:
        png_data: PNG文件的二进制数据
        
    Returns:
        tuple: (width, height) 如果成功，否则返回 (None, None)
    """
    try:
        # PNG文件格式：前8字节是签名，然后是IHDR chunk
        # IHDR chunk的数据部分：前4字节是宽度，接下来4字节是高度
        if len(png_data) < 24:
            return None, None
        
        # 检查PNG签名
        if png_data[0:8] != b'\x89PNG\r\n\x1a\n':
            return None, None
        
        # 解析宽度和高度（大端序，4字节）
        width = struct.unpack('>I', png_data[16:20])[0]
        height = struct.unpack('>I', png_data[20:24])[0]
        
        return width, height
    except Exception:
        return None, None


def png_to_txt(png_path, output_dir=None, output_filename=None, include_resolution=True):
    """
    将PNG文件转换为TXT格式的数据文件
    
    Args:
        png_path: PNG图片文件路径
        output_dir: 输出目录，如果为None则使用PNG文件所在目录
        output_filename: 输出文件名，如果为None则使用PNG文件名（替换扩展名为.txt）
    """
    # 检查输入文件是否存在
    if not os.path.exists(png_path):
        print(f"错误: 文件不存在 - {png_path}")
        return False
    
    # 读取PNG文件（二进制模式）
    try:
        with open(png_path, 'rb') as f:
            png_data = f.read()
    except Exception as e:
        print(f"错误: 无法读取文件 - {e}")
        return False
    
    # 确定输出路径
    if output_dir is None:
        output_dir = os.path.dirname(png_path)
    
    # 确保输出目录存在
    os.makedirs(output_dir, exist_ok=True)
    
    # 确定输出文件名
    if output_filename is None:
        base_name = os.path.splitext(os.path.basename(png_path))[0]
        
        # 如果启用分辨率信息，尝试从PNG数据中获取分辨率
        if include_resolution:
            width, height = get_png_resolution(png_data)
            if width is not None and height is not None:
                output_filename = f"{base_name}_{width}x{height}.txt"
            else:
                output_filename = f"{base_name}.txt"
        else:
            output_filename = f"{base_name}.txt"
    
    output_path = os.path.join(output_dir, output_filename)
    
    # 转换为十六进制格式（C数组格式，每行末尾添加逗号）
    bytes_per_line = 16
    lines = []
    
    for i in range(0, len(png_data), bytes_per_line):
        chunk = png_data[i:i + bytes_per_line]
        hex_values = [f"0x{b:02x}" for b in chunk]
        line = ','.join(hex_values) + ','
        lines.append(line)
    
    # 写入文件
    try:
        with open(output_path, 'w') as f:
            f.write('\n'.join(lines))
        # 显示分辨率信息
        width, height = get_png_resolution(png_data)
        if width is not None and height is not None:
            print(f"成功: 已转换 {len(png_data)} 字节 (分辨率: {width}x{height})")
        else:
            print(f"成功: 已转换 {len(png_data)} 字节")
        print(f"输出文件: {output_path}")
        return True
    except Exception as e:
        print(f"错误: 无法写入文件 - {e}")
        return False


def main():
    parser = argparse.ArgumentParser(
        description='将PNG图片转换为TXT格式的数据文件',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  %(prog)s input.png
  %(prog)s input.png -o output_dir
  %(prog)s input.png -o output_dir -n output.txt
        """
    )
    
    parser.add_argument(
        'png_file',
        help='输入的PNG图片文件路径'
    )
    
    parser.add_argument(
        '-o', '--output-dir',
        dest='output_dir',
        help='输出目录（默认：PNG文件所在目录）'
    )
    
    parser.add_argument(
        '-n', '--name',
        dest='output_filename',
        help='输出文件名（默认：PNG文件名_宽度x高度.txt）'
    )
    
    parser.add_argument(
        '--no-resolution',
        dest='include_resolution',
        action='store_false',
        default=True,
        help='不在文件名中包含分辨率信息'
    )
    
    args = parser.parse_args()
    
    success = png_to_txt(
        args.png_file,
        args.output_dir,
        args.output_filename,
        args.include_resolution
    )
    
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()


#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
资源打包工具
根据 resource_config.json 配置文件，将图片资源打包成二进制资源包
符合 img_res_head_wrapper_t 数据结构定义
"""

import os
import sys
import json
import struct
import hashlib
import re
from typing import List, Dict, Tuple
from pathlib import Path

# 常量定义（与C头文件保持一致）
IMG_RES_MAGIC = 0x52534547  # "GESR"
IMG_RES_HEADER_FIXED_SIZE = 32 * 1024  # 固定32KB
IMG_RES_VERSION_CURRENT = 0x0100  # v1.0
IMG_RES_MD5_SIZE = 16

# 资源类型枚举
IMG_RES_TYPE_BASE = 0
IMG_RES_TYPE_UI = 1
IMG_RES_TYPE_ALGO = 2

# 语言类型枚举
IMG_RES_LANG_TYPE_EN = 0
IMG_RES_LANG_TYPE_ZH = 1


def calc_crc16(data: bytes) -> int:
    """计算CRC16校验和 (CCITT标准)"""
    crc = 0xFFFF
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ 0x1021
            else:
                crc = crc << 1
            crc &= 0xFFFF
    return crc


def calc_md5(data: bytes) -> bytes:
    """计算MD5哈希"""
    return hashlib.md5(data).digest()


def parse_image_size_from_filename(filename: str) -> Tuple[int, int]:
    """从文件名解析图片尺寸 (例如: xxx_360x360_xxx.baj -> (360, 360))"""
    match = re.search(r'_(\d+)x(\d+)_', filename)
    if match:
        width = int(match.group(1))
        height = int(match.group(2))
        return width, height
    return 0, 0  # 默认值


class ResourcePacker:
    def __init__(self, config_file: str, output_file: str, base_dir: str = None):
        self.config_file = config_file
        self.output_file = output_file
        self.base_dir = base_dir or os.path.dirname(config_file)
        self.config = None
        self.resources = []
        
    def load_config(self):
        """加载配置文件"""
        print(f"📖 加载配置文件: {self.config_file}")
        with open(self.config_file, 'r', encoding='utf-8') as f:
            self.config = json.load(f)
        print(f"✅ 配置加载成功，共 {len(self.config['resources'])} 个资源")
        
    def load_resources(self):
        """加载所有资源文件"""
        print("\n📦 开始加载资源文件...")
        
        for res in self.config['resources']:
            res_id_raw = res['resource_id']
            
            # 解析资源ID（支持字符串宏名称或数字）
            if isinstance(res_id_raw, str) and not res_id_raw.isdigit():
                # 如果是字符串宏名称，提取数字部分或直接使用
                res_name = res_id_raw
                # 简单处理：尝试找到序号
                try:
                    # 从枚举推断ID（假设从1开始）
                    res_id = len(self.resources) + 1
                except:
                    res_id = len(self.resources) + 1
            else:
                # 如果是数字
                res_id = int(res_id_raw) if isinstance(res_id_raw, str) else res_id_raw
                res_name = res.get('resource_name', f'RESOURCE_{res_id}')
            
            file_path = os.path.join(self.base_dir, res['file'])
            
            if not os.path.exists(file_path):
                print(f"⚠️  警告: 资源文件不存在: {file_path}")
                continue
            
            # 读取资源数据
            with open(file_path, 'rb') as f:
                data = f.read()
            
            # 从文件名解析尺寸
            width, height = parse_image_size_from_filename(res['file'])
            if width == 0 or height == 0:
                print(f"⚠️  警告: 无法从文件名解析尺寸: {res['file']}, 使用默认值")
                width, height = 360, 360
            
            # 计算CRC16
            crc16 = calc_crc16(data)
            
            resource_info = {
                'img_id': res_id,
                'crc16': crc16,
                'width': width,
                'height': height,
                'x_offset': res.get('x_offset', 0),
                'y_offset': res.get('y_offset', 0),
                'data': data,
                'data_size': len(data),
                'offset_addr': 0,  # 后续计算
                'name': res_name,
                'description': res['description']
            }
            
            self.resources.append(resource_info)
            print(f"  ✓ [{res_id:02d}] {res_name}: {width}x{height}, {len(data)} bytes")
        
        print(f"\n✅ 共加载 {len(self.resources)} 个资源")
        
    def calculate_offsets(self):
        """计算每个资源的偏移地址"""
        print("\n🔢 计算资源偏移地址...")
        
        # 资源数据从固定32KB后开始
        current_offset = 0
        
        for res in self.resources:
            res['offset_addr'] = current_offset
            current_offset += res['data_size']
            print(f"  [{res['img_id']:02d}] offset=0x{res['offset_addr']:08X}, size={res['data_size']}")
        
        print(f"✅ 资源体总大小: {current_offset} bytes")
        return current_offset
        
    def pack(self):
        """打包资源"""
        print("\n" + "="*60)
        print("🚀 开始打包资源...")
        print("="*60)
        
        # 加载配置和资源
        self.load_config()
        self.load_resources()
        
        if not self.resources:
            print("❌ 错误: 没有可打包的资源")
            return False
        
        # 计算偏移
        body_size = self.calculate_offsets()
        
        # 按资源ID排序
        self.resources.sort(key=lambda x: x['img_id'])
        
        # 获取配置参数
        meta = self.config['meta']
        res_type = IMG_RES_TYPE_UI if meta['resource_type'] == 'IMG_RES_TYPE_UI' else IMG_RES_TYPE_BASE
        lang_id = IMG_RES_LANG_TYPE_ZH if meta['language'] == 'IMG_RES_LANG_TYPE_ZH' else IMG_RES_LANG_TYPE_EN
        flags = 1 if meta.get('compressed', False) else 0
        
        # 构建obj_info数组
        obj_info_data = b''
        for res in self.resources:
            # struct img_res_obj_info: 20字节
            obj_info = struct.pack('<HHHHHHII',
                res['img_id'],      # uint16_t img_id
                res['crc16'],       # uint16_t crc16
                res['width'],       # uint16_t width
                res['height'],      # uint16_t height
                res['x_offset'],    # uint16_t x_offset
                res['y_offset'],    # uint16_t y_offset
                res['offset_addr'], # uint32_t offset_addr
                res['data_size']    # uint32_t data_size
            )
            obj_info_data += obj_info
        
        # 计算总大小
        header_size = 48  # sizeof(img_res_head_wrapper_t) = 48字节 (不含data[])
        total_size = IMG_RES_HEADER_FIXED_SIZE + body_size
        
        # 构建head描述 (48字节)
        head_desc = struct.pack('<IHHHHHHI16s',
            IMG_RES_MAGIC,              # uint32_t magic
            0,                          # uint16_t crc16 (暂时为0，后续计算)
            flags,                      # uint16_t flags
            res_type,                   # uint16_t type
            lang_id,                    # uint16_t lang_id
            IMG_RES_VERSION_CURRENT,    # uint16_t version
            len(self.resources),        # uint16_t res_count
            total_size,                 # uint32_t total_size
            b'\x00' * 16                # uint32_t reserved[4]
        )
        
        # 计算head的CRC16
        head_crc16 = calc_crc16(head_desc[6:])  # 跳过magic和crc16字段本身
        
        # 重新打包head（包含正确的CRC16）
        head_desc = struct.pack('<IHHHHHHI16s',
            IMG_RES_MAGIC,
            head_crc16,
            flags,
            res_type,
            lang_id,
            IMG_RES_VERSION_CURRENT,
            len(self.resources),
            total_size,
            b'\x00' * 16
        )
        
        # 计算head的MD5
        head_md5 = calc_md5(head_desc)
        
        # 计算body的MD5 (obj_info + res_body)
        body_data = b''
        for res in self.resources:
            body_data += res['data']
        body_md5 = calc_md5(obj_info_data + body_data)
        
        # 构建wrapper (48字节)
        wrapper = head_desc + head_md5 + body_md5
        
        # 构建完整的头部区域 (32KB)
        header_region = wrapper + obj_info_data
        # 填充到32KB
        header_region += b'\xFF' * (IMG_RES_HEADER_FIXED_SIZE - len(header_region))
        
        # 写入输出文件
        print(f"\n💾 写入输出文件: {self.output_file}")
        with open(self.output_file, 'wb') as f:
            # 写入头部区域（32KB）
            f.write(header_region)
            # 写入资源数据体
            f.write(body_data)
        
        # 打印摘要信息
        print("\n" + "="*60)
        print("✅ 资源打包完成!")
        print("="*60)
        print(f"📊 打包信息:")
        print(f"  - 资源类型: {meta['resource_type']}")
        print(f"  - 语言: {meta['language']}")
        print(f"  - 版本: {hex(IMG_RES_VERSION_CURRENT)}")
        print(f"  - 资源数量: {len(self.resources)}")
        print(f"  - 头部大小: {len(header_region)} bytes (固定32KB)")
        print(f"  - 数据体大小: {len(body_data)} bytes")
        print(f"  - 总大小: {len(header_region) + len(body_data)} bytes")
        print(f"  - 输出文件: {self.output_file}")
        print(f"  - Head CRC16: 0x{head_crc16:04X}")
        print(f"  - Head MD5: {head_md5.hex()}")
        print(f"  - Body MD5: {body_md5.hex()}")
        
        return True


def main():
    """主函数"""
    if len(sys.argv) < 2:
        print("用法: python pack_resources.py <config.json> [output.bin]")
        print("示例: python pack_resources.py resource_config.json resource_pack.bin")
        sys.exit(1)
    
    config_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else "resource_pack.bin"
    
    if not os.path.exists(config_file):
        print(f"❌ 错误: 配置文件不存在: {config_file}")
        sys.exit(1)
    
    packer = ResourcePacker(config_file, output_file)
    success = packer.pack()
    
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()


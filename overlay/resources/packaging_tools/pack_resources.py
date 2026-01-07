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
import ast
from datetime import datetime
from typing import List, Dict, Tuple, Optional
from pathlib import Path

# 常量定义（与C头文件保持一致）
IMG_RES_MAGIC = 0x52534547  # "GESR"
IMG_RES_HEADER_FIXED_SIZE = 64 * 1024  # 默认值（优先从 resource_id_define.h 的 IMG_RES_HEADER_FIXED_SIZE 宏解析）
IMG_RES_VERSION_CURRENT = 0x0100  # v1.0
IMG_RES_MD5_SIZE = 16
IMG_RES_NAME_PREFIX_DEFAULT = "V0"  # 默认文件名前缀（优先从 resource_id_define.h 的 IMG_RES_NAME_PREFIX 宏解析）

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


def _strip_c_comments(text: str) -> str:
    """去掉C/C++风格注释（// 与 /* */）"""
    text = re.sub(r'/\*[\s\S]*?\*/', '', text)
    text = re.sub(r'//.*', '', text)
    return text


def _safe_eval_int(expr: str) -> int:
    """
    安全计算简单整型表达式（用于解析 #define IMG_RES_HEADER_FIXED_SIZE）。
    仅允许数字/十六进制、括号、以及常见位运算与算术运算。
    """
    expr = expr.strip()
    if not expr:
        raise ValueError("empty expression")

    # 去掉 C 数字后缀：U/L/UL 等
    expr = re.sub(r'(\b0x[0-9a-fA-F]+|\b\d+)([uUlL]+)\b', r'\1', expr)

    # 只允许特定字符集合，避免注入
    if re.search(r'[^0-9a-fA-FxX\(\)\s\+\-\*/%<>&\|\^~]', expr):
        raise ValueError(f"unsupported characters in expression: {expr!r}")

    node = ast.parse(expr, mode='eval')

    def _eval(n: ast.AST) -> int:
        if isinstance(n, ast.Expression):
            return _eval(n.body)
        if isinstance(n, ast.Constant) and isinstance(n.value, (int,)):
            return int(n.value)
        if isinstance(n, ast.UnaryOp) and isinstance(n.op, (ast.UAdd, ast.USub, ast.Invert)):
            v = _eval(n.operand)
            if isinstance(n.op, ast.UAdd):
                return +v
            if isinstance(n.op, ast.USub):
                return -v
            return ~v
        if isinstance(n, ast.BinOp) and isinstance(
            n.op,
            (
                ast.Add,
                ast.Sub,
                ast.Mult,
                ast.Div,
                ast.FloorDiv,
                ast.Mod,
                ast.LShift,
                ast.RShift,
                ast.BitOr,
                ast.BitAnd,
                ast.BitXor,
            ),
        ):
            a = _eval(n.left)
            b = _eval(n.right)
            if isinstance(n.op, ast.Add):
                return a + b
            if isinstance(n.op, ast.Sub):
                return a - b
            if isinstance(n.op, ast.Mult):
                return a * b
            if isinstance(n.op, (ast.Div, ast.FloorDiv)):
                return a // b
            if isinstance(n.op, ast.Mod):
                return a % b
            if isinstance(n.op, ast.LShift):
                return a << b
            if isinstance(n.op, ast.RShift):
                return a >> b
            if isinstance(n.op, ast.BitOr):
                return a | b
            if isinstance(n.op, ast.BitAnd):
                return a & b
            if isinstance(n.op, ast.BitXor):
                return a ^ b
        raise ValueError(f"unsupported expression AST: {ast.dump(n)}")

    return int(_eval(node))


def parse_resource_header(header_file: str) -> Tuple[Dict[str, int], Optional[int], Optional[str]]:
    """
    解析 resource_id_define.h：
    - enum resource_id_type_e: RESOURCE_ID_XXX -> 数值
    - #define IMG_RES_HEADER_FIXED_SIZE: 头部固定分区大小（字节）
    """
    with open(header_file, "r", encoding="utf-8", errors="ignore") as f:
        raw = f.read()

    raw = _strip_c_comments(raw)

    # 解析头部固定大小宏
    header_size = None
    m = re.search(r'^\s*#define\s+IMG_RES_HEADER_FIXED_SIZE\s+(.+?)\s*$', raw, flags=re.MULTILINE)
    if m:
        try:
            header_size = _safe_eval_int(m.group(1))
        except Exception as e:
            raise ValueError(f"Failed to parse IMG_RES_HEADER_FIXED_SIZE from {header_file}: {e}") from e

    # 解析文件名前缀宏（支持 "V3" 或 V3）
    name_prefix = None
    m = re.search(r'^\s*#define\s+IMG_RES_NAME_PREFIX\s+(.+?)\s*$', raw, flags=re.MULTILINE)
    if m:
        val = m.group(1).strip()
        # 去掉引号
        if (val.startswith('"') and val.endswith('"')) or (val.startswith("'") and val.endswith("'")):
            val = val[1:-1]
        name_prefix = val.strip() or None

    # 解析 enum
    enum_map: Dict[str, int] = {}
    enum_block = re.search(r'typedef\s+enum\s*\{([\s\S]*?)\}\s*resource_id_type_e\s*;', raw)
    if not enum_block:
        return enum_map, header_size, name_prefix

    current = -1
    for item in enum_block.group(1).split(","):
        item = item.strip()
        if not item:
            continue
        # 形如：NAME 或 NAME = 0x01
        if "=" in item:
            name, val = item.split("=", 1)
            name = name.strip()
            val = val.strip()
            try:
                current = int(val, 0)
            except Exception as e:
                raise ValueError(f"Failed to parse enum value {val!r} for {name} in {header_file}") from e
        else:
            name = item.strip()
            current = current + 1
        if name:
            enum_map[name] = current

    return enum_map, header_size, name_prefix


def parse_img_res_define_header(header_file: str) -> Tuple[Optional[int], Optional[int], Optional[str]]:
    """
    解析 resource_img_define.h：
    - IMG_RES_HEADER_FIXED_SIZE
    - IMG_RES_VERSION_CURRENT
    - IMG_RES_NAME_PREFIX
    """
    with open(header_file, "r", encoding="utf-8", errors="ignore") as f:
        raw = f.read()

    raw = _strip_c_comments(raw)

    header_size = None
    m = re.search(r'^\s*#define\s+IMG_RES_HEADER_FIXED_SIZE\s+(.+?)\s*$', raw, flags=re.MULTILINE)
    if m:
        header_size = _safe_eval_int(m.group(1))

    version_current = None
    m = re.search(r'^\s*#define\s+IMG_RES_VERSION_CURRENT\s+(.+?)\s*$', raw, flags=re.MULTILINE)
    if m:
        version_current = _safe_eval_int(m.group(1))

    name_prefix = None
    m = re.search(r'^\s*#define\s+IMG_RES_NAME_PREFIX\s+(.+?)\s*$', raw, flags=re.MULTILINE)
    if m:
        val = m.group(1).strip()
        if (val.startswith('"') and val.endswith('"')) or (val.startswith("'") and val.endswith("'")):
            val = val[1:-1]
        if (val.startswith('(') and val.endswith(')')):
            val = val[1:-1].strip()
        if (val.startswith('"') and val.endswith('"')) or (val.startswith("'") and val.endswith("'")):
            val = val[1:-1]
        name_prefix = val.strip() or None

    return header_size, version_current, name_prefix


def find_file_upwards(start_dir: str, relative_path: str, max_depth: int = 8) -> Optional[str]:
    """从 start_dir 向上查找 relative_path，找到即返回绝对路径。"""
    cur = Path(start_dir).resolve()
    for _ in range(max_depth + 1):
        cand = cur / relative_path
        if cand.exists():
            return str(cand)
        if cur.parent == cur:
            break
        cur = cur.parent
    return None


def make_versioned_output_path(
    base_output_file: str,
    name_prefix: str,
    version_hex: int,
    date_str: str,
    body_md5_hex: str,
) -> str:
    """
    生成带版本/日期/MD5标识的输出文件名（不使用传入文件名的 stem）。
    - 前缀: IMG_RES_NAME_PREFIX（例如 V3）
    - 版本: 使用 IMG_RES_VERSION_CURRENT 的 HEX（去掉前导0，例如 0x0100 -> 100）
    - 日期: YYYYMMDD
    - MD5: 使用 body_md5 的前8位（内容变化更敏感，便于区分产物）
    """
    p = Path(base_output_file)
    md5_short = (body_md5_hex or "")[:8]

    prefix = (name_prefix or IMG_RES_NAME_PREFIX_DEFAULT).strip()
    ver_str = f"{version_hex:X}"
    filename = f"{prefix}_RES_V{ver_str}_{date_str}_b{md5_short}.bin"
    return str(p.with_name(filename))


class ResourcePacker:
    def __init__(self, config_file: str, output_file: str, header_file: str = None, base_dir: str = None):
        self.config_file = config_file
        self.output_file = output_file
        self.header_file = header_file
        self.base_dir = base_dir or os.path.dirname(config_file)
        self.config = None
        self.resources = []
        self.id_map: Dict[str, int] = {}
        self.header_fixed_size: int = IMG_RES_HEADER_FIXED_SIZE
        self.name_prefix: str = IMG_RES_NAME_PREFIX_DEFAULT
        self.version_current: int = IMG_RES_VERSION_CURRENT
        
    def load_config(self):
        """加载配置文件"""
        print(f"📖 加载配置文件: {self.config_file}")
        with open(self.config_file, 'r', encoding='utf-8') as f:
            self.config = json.load(f)
        print(f"✅ 配置加载成功，共 {len(self.config['resources'])} 个资源")

        # 解析头文件（资源ID映射）
        header_path = self.header_file
        if not header_path:
            candidate = os.path.join(self.base_dir, "resource_id_define.h")
            if os.path.exists(candidate):
                header_path = candidate

        if header_path and os.path.exists(header_path):
            print(f"📎 解析头文件: {header_path}")
            self.id_map, header_size, name_prefix = parse_resource_header(header_path)
            if not self.id_map:
                print("⚠️  未解析到任何资源ID枚举映射（resource_id_type_e），后续宏名称将无法转换为数字ID")
        else:
            print("⚠️  未提供/未找到 resource_id_define.h，使用默认头部大小与顺序分配ID（不推荐）")

        # 从 resource_img_define.h 读取头部分区大小/版本/命名前缀（迁移后的单一来源）
        img_define_rel = "trunk/root_source/resource/resource_img_define.h"
        img_define_path = find_file_upwards(self.base_dir, img_define_rel)
        if img_define_path and os.path.exists(img_define_path):
            print(f"📎 解析图片资源定义: {img_define_path}")
            header_size2, version_current2, name_prefix2 = parse_img_res_define_header(img_define_path)
            if header_size2 is not None:
                self.header_fixed_size = int(header_size2)
            if version_current2 is not None:
                self.version_current = int(version_current2)
            if name_prefix2:
                self.name_prefix = str(name_prefix2)
            print(f"✅ 头部分区大小: {self.header_fixed_size} bytes")
            print(f"✅ 资源版本: {hex(self.version_current)}")
            print(f"✅ 文件名前缀: {self.name_prefix}")
        else:
            print(f"⚠️  未找到 {img_define_rel}，将使用默认头部大小/版本/前缀")

        # 关键：按 img_id 数值对 JSON 资源列表排序，确保“配置顺序=打包顺序=打印顺序”
        def _resolve_id_for_sort(res: Dict) -> int:
            rid = res.get("resource_id")
            if isinstance(rid, str) and not rid.isdigit():
                if self.id_map and rid in self.id_map:
                    return int(self.id_map[rid])
                return 0x7FFFFFFF
            try:
                return int(rid, 0) if isinstance(rid, str) else int(rid)
            except Exception:
                return 0x7FFFFFFF

        if isinstance(self.config, dict) and isinstance(self.config.get("resources"), list):
            self.config["resources"].sort(key=_resolve_id_for_sort)
            print("🔀 已按资源ID升序对配置资源列表排序（用于打包/输出）")
        
    def load_resources(self):
        """加载所有资源文件"""
        print("\n📦 开始加载资源文件...")
        
        for res in self.config['resources']:
            res_id_raw = res['resource_id']
            
            # 解析资源ID（支持字符串宏名称或数字）
            if isinstance(res_id_raw, str) and not res_id_raw.isdigit():
                # 如果是字符串宏名称，必须从头文件映射到数值
                res_name = res_id_raw.strip()
                if self.id_map and res_name in self.id_map:
                    res_id = self.id_map[res_name]
                else:
                    print(f"❌ 错误: 无法解析资源ID宏: {res_name}（请检查头文件 resource_id_define.h 是否包含该枚举）")
                    return False
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
        return True
        
    def calculate_offsets(self):
        """计算每个资源的偏移地址"""
        print("\n🔢 计算资源偏移地址...")
        
        # 资源数据从固定头部分区后开始（offset_addr 为资源体内偏移）
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
        if not self.load_resources():
            return False
        
        if not self.resources:
            print("❌ 错误: 没有可打包的资源")
            return False

        # 按资源ID排序（必须先排序再计算offset，否则offset会与body顺序不一致）
        self.resources.sort(key=lambda x: x['img_id'])

        # 计算偏移（offset_addr 与 body_data 顺序一致）
        body_size = self.calculate_offsets()
        
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
        total_size = self.header_fixed_size + body_size
        
        # 构建head描述 (48字节)
        head_desc = struct.pack('<IHHHHHHI16s',
            IMG_RES_MAGIC,              # uint32_t magic
            0,                          # uint16_t crc16 (暂时为0，后续计算)
            flags,                      # uint16_t flags
            res_type,                   # uint16_t type
            lang_id,                    # uint16_t lang_id
            self.version_current,       # uint16_t version
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
            self.version_current,
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
        
        # 构建完整的头部区域 (固定头部分区大小)
        header_region = wrapper + obj_info_data
        if len(header_region) > self.header_fixed_size:
            print(
                f"❌ 错误: 头部区域大小({len(header_region)} bytes)超过固定大小({self.header_fixed_size} bytes)，"
                "请减少资源数量或增大头文件中的 IMG_RES_HEADER_FIXED_SIZE"
            )
            return False
        # 填充到固定大小
        header_region += b'\xFF' * (self.header_fixed_size - len(header_region))
        
        # 输出文件名：追加版本/日期/body_md5前缀（body变化更敏感）
        date_str = datetime.now().strftime("%Y%m%d")
        final_output_file = make_versioned_output_path(
            self.output_file,
            self.name_prefix,
            self.version_current,
            date_str,
            body_md5.hex(),
        )
        # 机器可解析，供 pack.sh 获取最终输出文件名
        print(f"OUTPUT_FILE_FINAL={final_output_file}")

        # 写入输出文件
        print(f"\n💾 写入输出文件: {final_output_file}")
        with open(final_output_file, 'wb') as f:
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
        print(f"  - 版本: {hex(self.version_current)}")
        print(f"  - 资源数量: {len(self.resources)}")
        print(f"  - 头部大小: {len(header_region)} bytes (固定{self.header_fixed_size // 1024}KB)")
        print(f"  - 数据体大小: {len(body_data)} bytes")
        print(f"  - 总大小: {len(header_region) + len(body_data)} bytes")
        print(f"  - 输出文件: {final_output_file}")
        print(f"  - Head CRC16: 0x{head_crc16:04X}")
        print(f"  - Head MD5: {head_md5.hex()}")
        print(f"  - Body MD5: {body_md5.hex()}")
        
        return True


def main():
    """主函数"""
    if len(sys.argv) < 2:
        print("用法: python pack_resources.py <config.json> [output.bin] [header.h] [base_dir]")
        print("示例: python pack_resources.py resource_config.json resource_pack.bin resource_id_define.h")
        print("示例: python pack_resources.py V1/resource_config.json bin/output.bin V1/resource_id_define.h V1")
        sys.exit(1)
    
    config_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else "resource_pack.bin"
    header_file = sys.argv[3] if len(sys.argv) > 3 else None
    base_dir = sys.argv[4] if len(sys.argv) > 4 else None
    
    if not os.path.exists(config_file):
        print(f"❌ 错误: 配置文件不存在: {config_file}")
        sys.exit(1)
    
    # 如果没有指定 base_dir，使用配置文件所在目录
    if base_dir is None:
        base_dir = os.path.dirname(os.path.abspath(config_file))
    
    packer = ResourcePacker(config_file, output_file, header_file=header_file, base_dir=base_dir)
    success = packer.pack()
    
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()


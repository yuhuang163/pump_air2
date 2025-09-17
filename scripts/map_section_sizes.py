#!/usr/bin/env python3
"""
Map section size summarizer

- 统计并汇总 .text / .data / .bss / .rodata 大小
- 尽量避免因总行与子行同时计入而导致的重复统计：
  - 优先累计来自目标文件/静态库成员（包含 .o/.obj/.a( 等特征）的行
  - 若没有发现上述“叶子行”，则退回使用可能的总行数值

用法：
  python3 scripts/map_section_sizes.py <path/to/file.map> [--mode auto|leaf|total]

输出：
  按大小从大到小排序，显示字节与KB
"""

from __future__ import annotations

import argparse
import os
import re
from collections import defaultdict
from typing import Dict, Tuple


SECTION_BASES = [".text", ".data", ".bss", ".rodata"]


def parse_size_token(token: str) -> int:
    token = token.strip().lower()
    try:
        if token.startswith("0x"):
            return int(token, 16)
        return int(token, 10)
    except ValueError:
        return 0


def detect_base_section(section_token: str) -> str | None:
    token = section_token.strip()
    if not token.startswith('.'):
        return None
    # 只取基础节名：.text/.data/.bss/.rodata
    for base in SECTION_BASES:
        if token == base or token.startswith(base + '.') or token.startswith(base + ' '):
            return base
    return None


def looks_like_object_contribution(line: str) -> bool:
    # 判断该行是否看起来像来自目标文件/静态库成员的“叶子行”，以避免统计总行
    # 更严格：要求出现 .o/.obj 或 存在 .a(XXX.o)
    return bool(re.search(r"(\.o\b|\.obj\b|\.a\([^\)]+\.o\))", line))


def parse_map_file(path: str, mode: str = "auto") -> Dict[str, int]:
    sum_leaf: Dict[str, int] = defaultdict(int)
    sum_total_like: Dict[str, int] = defaultdict(int)

    # 提取：<section> <addr> <size> ...
    # 我们不强依赖列对齐，只要能拿到两个数字（第一个通常为地址，第二个为大小）
    section_lead = re.compile(r"^\s*(\.[A-Za-z0-9_\.]+)\b")
    number_finder = re.compile(r"0x[0-9A-Fa-f]+|\d+")

    with open(path, 'r', encoding='utf-8', errors='ignore') as f:
        pending_subsection: Tuple[str, bool] | None = None  # (base_section, expect_numbers_next_line)
        for raw_line in f:
            line = raw_line.rstrip("\n")

            m = section_lead.match(line)
            if m:
                base_or_sub = m.group(1)
                base = detect_base_section(base_or_sub)
                if base is None:
                    pending_subsection = None
                    continue

                nums = number_finder.findall(line)
                if len(nums) >= 2:
                    size_val = parse_size_token(nums[1])
                    if size_val > 0:
                        if looks_like_object_contribution(line):
                            sum_leaf[base] += size_val
                        else:
                            sum_total_like[base] += size_val
                        pending_subsection = None
                        continue

                # 处理两行形式：第一行只有名称（如 .text.func），下一行才有地址与大小
                if base_or_sub.count('.') >= 2 or base_or_sub.startswith(base + '.'):
                    pending_subsection = (base, True)
                else:
                    pending_subsection = None
                continue

            # 如果上一行是子节名，这一行尝试读取数值
            if pending_subsection is not None:
                base, _ = pending_subsection
                nums = number_finder.findall(line)
                if len(nums) >= 2:
                    size_val = parse_size_token(nums[1])
                    if size_val > 0:
                        if looks_like_object_contribution(line):
                            sum_leaf[base] += size_val
                        else:
                            sum_total_like[base] += size_val
                pending_subsection = None

    # 选择最终结果
    result: Dict[str, int] = {}
    for base in SECTION_BASES:
        leaf_sum = sum_leaf.get(base, 0)
        total_sum = sum_total_like.get(base, 0)
        if mode == "leaf":
            result[base] = leaf_sum
        elif mode == "total":
            result[base] = total_sum
        else:  # auto
            result[base] = leaf_sum if leaf_sum > 0 else total_sum
    return result


def format_size(nbytes: int) -> str:
    kb = nbytes / 1024.0
    return f"{nbytes} B\t({kb:.2f} KB)"


def main() -> None:
    parser = argparse.ArgumentParser(description="Summarize section sizes from a GNU ld .map file")
    parser.add_argument("map_file", help="Path to .map file")
    parser.add_argument("--mode", choices=["auto", "leaf", "total"], default="auto", help="统计模式：auto/leaf/total，默认 auto")
    args = parser.parse_args()

    map_path = os.path.abspath(args.map_file)
    if not os.path.isfile(map_path):
        raise SystemExit(f"Map 文件不存在: {map_path}")

    sizes = parse_map_file(map_path, mode=args.mode)

    # 排序（从大到小）
    sorted_items = sorted(sizes.items(), key=lambda kv: kv[1], reverse=True)

    total = sum(sizes.values())
    print(f"文件: {map_path}")
    print("节名\t大小\t\t占比")
    for name, sz in sorted_items:
        percent = (sz / total * 100.0) if total > 0 else 0.0
        print(f"{name}\t{format_size(sz)}\t{percent:5.2f}%")


if __name__ == "__main__":
    main()



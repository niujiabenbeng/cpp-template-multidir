#! /usr/bin/env python
# coding: utf-8
#
# pylint: disable=all
#
"""clang-tidy加强版.

clang-tidy不支持文件范围的设置, 并且只能设置单行和下一行, 这给使用过程带来
了不便. 这里, 我们扩展了原生clang-tidy工具, 扩展后的工具支持如下设置:

1. NOLINT(*):         原生设置, 忽略当前行, 括号内为指定检查项
2. NOLINTNEXTLINE(*): 原生设置, 忽略下一行, 括号内为指定检查项
3. NOLINTFIELD(*):    扩展设置, 忽略检查项, 括号内为指定检查项
4. NOLINTLINE(*):     扩展设置, 忽略指定行, 括号内为指定行范围(相对于当前行)

下面是几个使用扩展设置的例子:

NOLINTFIELD(misc-no-recursion): 忽略检查项: misc-no-recursion.
NOLINTFIELD(all): 忽略所有检查项.
NOLINTLINE(0):    忽略当前行.
NOLINTLINE(2):    忽略行号为2的行. 当前行行号为0, 负数往前数, 正数往后数.
NOLINTLINE(4:6):  忽略行号为4~6的行.
NOLINTLINE(-2:):  相当于: NOLINTLINE(-2:0).
NOLINTLINE(:2):   相当于: NOLINTLINE(0:2).
"""

import os
import re
import json
import argparse


def _locate_dominate_file(root, name):
    """从root开始根据目录结构逐级寻找名字为name的文件."""

    if os.path.samefile(root, "/"): return None
    if os.path.isdir(root):
        path = os.path.join(root, name)
        if os.path.exists(path): return path
    return _locate_dominate_file(os.path.dirname(root), name)


def _parse_range_string(line_id, content):
    """解析范围字符串."""

    def _parse_string(string):
        if ":" not in string:
            return int(string), int(string)
        if string.startswith(":"):
            return 0, int(string[1:])
        if string.endswith(":"):
            return int(string[:-1]), 0
        return [int(v) for v in string.split(":")]

    start, end = _parse_string(content)
    assert start <= end, f"Wrong format: {content}"
    return line_id + start, line_id + end


def _flip_ranges(ranges, length):
    """返回ranges的补集, 即所有需要检查的行."""

    curr_loc = 0
    flip_ranges = []
    # 注意1: ranges中从0开始, flip_ranges中从1开始
    # 注意2: ranges中包括两端, flip_ranges为前开后闭区间
    for start, end in ranges:
        if curr_loc < start:
            flip_ranges.append((curr_loc + 1, start))
        curr_loc = end + 1
    if curr_loc < length - 1:
        flip_ranges.append((curr_loc + 1, length))
    return flip_ranges


############################### local functions ################################


def _collect_nolint_fields(lines):
    """收集需要忽略的检查项."""

    nolint_fields = set()
    pattern = re.compile("NOLINTFIELD\((.+)\)")
    for line in lines:
        match = pattern.search(line)
        if not match: continue
        nolint_fields.update(match.group(1).split(","))
    return nolint_fields


def _collect_nolint_ranges(lines):
    """收集需要忽略的行."""

    nolint_ranges = []
    pattern = re.compile(r"NOLINTLINE\(([-0-9:]+)\)")
    for i, line in enumerate(lines):
        match = pattern.search(line)
        if not match: continue
        content = match.group(1)
        ranges = _parse_range_string(i, content)
        nolint_ranges.append(ranges)

    # 不允许每一个range之间有overlap
    nolint_ranges.sort(key=lambda x: x[0])
    for prev, curr in zip(nolint_ranges[:-1], nolint_ranges[1:]):
        assert prev[1] + 1 < curr[0], "no overlap ranges allowed."
    return nolint_ranges


def _generate_modified_code(lines, nolint_ranges, save_file):
    """将修改过的代码保存到文件. 这里只修改需要忽略的行."""

    nolint_indices = set()
    for start, end in nolint_ranges:
        nolint_indices.update(range(start, end + 1))
    with open(save_file, "w") as dstfile:
        for i, line in enumerate(lines):
            if i in nolint_indices:
                line = f"{line[:-1]}  // NOLINT\n"
            dstfile.write(line)


def _lint_cpp_file(file_path, file_length, global_fields, nolint_ranges):
    """对单个cpp源码文件执行lint操作."""

    checks = ",".join(f"-{v}" for v in global_fields)
    lines = _flip_ranges(nolint_ranges, file_length)
    line_filter = {"name": os.path.basename(file_path), "lines": lines}
    line_filter = json.dumps([line_filter]).replace(" ", "")
    assert _locate_dominate_file(file_path, "compile_commands.json")
    assert _locate_dominate_file(file_path, ".clang-tidy")

    command = ["clang-tidy --quiet"]
    command.append(f"--checks='{checks}'")
    command.append(f"--line-filter='{line_filter}'")
    command.append(file_path)
    command = " ".join(command)
    print(command)
    os.system(command)


def run_clang_tidy(args):
    for file in args.file:
        assert os.path.exists(file), f"File not found: {file}"
        # 注意: 这里的lines带了前后的空格字符, 结尾带了换行符
        with open(file, "r") as srcfile:
            lines = [l for l in srcfile]
        file = os.path.abspath(file)
        nolint_fields = _collect_nolint_fields(lines)
        nolint_ranges = _collect_nolint_ranges(lines)
        if "all" in nolint_fields: continue
        if args.save_modified:
            dirname, filename = os.path.split(file)
            path = os.path.join(dirname, f".{filename}.4ct")
            _generate_modified_code(lines, nolint_ranges, path)
        _lint_cpp_file(file, len(lines), nolint_fields, nolint_ranges)


def main():
    parser = argparse.ArgumentParser(
        description="Customized clang-tidy to lint c++ files.")

    parser.add_argument(
        "file",
        type=str,
        nargs="+",
        help="Path of c++ file to be linted.",
    )
    parser.add_argument(
        "--save_modified",
        action="store_true",
        help="Flag to save modified file (.4ct format).",
    )
    args = parser.parse_args()
    run_clang_tidy(args)
    print("Done!")


if __name__ == "__main__":
    main()

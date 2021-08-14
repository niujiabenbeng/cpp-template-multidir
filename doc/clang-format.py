#! /usr/bin/env python
# coding: utf-8
#
# pylint: disable=all
#
"""clang-format加强版.

clang-format本身不支持单行(单语句)设置, 这给实际使用带来了不便. 这里我们
扩展了原生的clang-format工具, 引入一个新的设置标记: NOFORMAT, 新的标记在
设置单行(单语句)的情况下非常方便, 下面是几个使用NOFORMAT的示例:

NOFORMAT:       忽略当前行.
NOFORMAT(2):    忽略行号为2的行. 当前行行号为0, 负数往前数, 正数往后数.
NOFORMAT(4:6):  忽略行号为4~6的行.
NOFORMAT(-2:):  相当于: NOFORMAT(-2:0).
NOFORMAT(:2):   相当于: NOFORMAT(0:2).
NOFORMAT(:2,4): 相当于: NOFORMAT(0:2)和NOFORMAT(4).
"""

import os
import re
import argparse


def _locate_dominate_file(root, name):
    """从root开始根据目录结构逐级寻找名字为name的文件."""

    if os.path.samefile(root, "/"): return None
    if os.path.isdir(root):
        path = os.path.join(root, name)
        if os.path.exists(path): return path
    return _locate_dominate_file(os.path.dirname(root), name)


def _parse_range_string(line_id, content):
    """解析如下格式的字符串: `2,:3,4:,5:6`."""
    def _parse_item(item):
        if ":" not in item:
            return int(item), int(item)
        if item.startswith(":"):
            return 0, int(item[1:])
        if item.endswith(":"):
            return int(item[:-1]), 0
        return tuple([int(v) for v in item.split(":")])

    line_ranges = []
    for item in content.split(","):
        start, end = _parse_item(item)
        assert start <= end, f"Wrong format: {content}"
        line_ranges.append((start + line_id, end + line_id))
    return line_ranges


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


def _collect_noformat_ranges(lines):
    """收集需要忽略的行."""

    noformat_ranges = []
    pattern = re.compile(r"//NOFORMAT\((.+)\)")
    for i, line in enumerate(lines):
        line = line.strip().replace(" ", "")
        match = pattern.search(line)
        if match:
            content = match.group(1)
            ranges = _parse_range_string(i, content)
            noformat_ranges.extend(ranges)
        elif f"//NOFORMAT" in line:
            noformat_ranges.append((i, i))

    # 不允许每一个range之间有overlap
    noformat_ranges.sort(key=lambda x: x[0])
    for prev, curr in zip(noformat_ranges[:-1], noformat_ranges[1:]):
        assert prev[1] + 1 < curr[0], "no overlap ranges allowed."
    return noformat_ranges


def _generate_modified_code(lines, noformat_ranges, save_file):
    """将修改过的代码保存到文件."""

    curr_loc = 0
    new_lines = []
    for start, end in noformat_ranges:
        new_lines.extend(lines[curr_loc:start])
        head = " " * (len(lines[start]) - len(lines[start].lstrip()))
        new_lines.append(f"{head}// clang-format off\n")
        new_lines.extend(lines[start:end + 1])
        head = " " * (len(lines[end]) - len(lines[end].lstrip()))
        new_lines.append(f"{head}// clang-format on\n")
        curr_loc = end + 1
    new_lines.extend(lines[curr_loc:])
    with open(save_file, "w") as dstfile:
        dstfile.writelines(new_lines)


def _format_cpp_file(file_path, file_length, noformat_ranges):
    """对单个cpp源码文件执行format操作."""

    assert _locate_dominate_file(file_path, ".clang-format")
    lines = _flip_ranges(noformat_ranges, file_length)
    command = ["clang-format -i --sort-includes --style=file"]
    for start, end in lines:
        command.append(f"--lines {start}:{end}")
    command.append(file_path)
    command = " ".join(command)
    print(command)
    os.system(command)


def run_clang_format(args):
    for file in args.file:
        assert os.path.exists(file), f"File not found: {file}"
        # 注意: 这里的lines带了前后的空格字符, 结尾带了换行符
        with open(file, "r") as srcfile:
            lines = [l for l in srcfile]
        file = os.path.abspath(file)
        noformat_ranges = _collect_noformat_ranges(lines)
        _generate_modified_code(lines, noformat_ranges, f"{file}.4cf")
        _format_cpp_file(file, len(lines), noformat_ranges)


def main():
    parser = argparse.ArgumentParser(
        description="Customized clang-format to auto-format c++ files.")

    parser.add_argument(
        "file",
        type=str,
        nargs="+",
        help="Path of c++ file to be auto-formatted.",
    )
    args = parser.parse_args()
    run_clang_format(args)
    print("Done!")


if __name__ == "__main__":
    main()

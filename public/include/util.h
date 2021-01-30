#ifndef PUBLIC_UTIL_H_
#define PUBLIC_UTIL_H_

#include "common.h"

// 如果需要的话, 生成文件所在的目录
void MakeDirsForFile(const std::string& path);

// 一次性读取文件所有的内容，如打开失败返回空字符串
std::string ReadFile(const std::string& file, bool is_binary = false);

// 一次性写入文件的所有内容
bool WriteFile(const std::string& file, const char* data, const int len);
bool WriteFile(const std::string& file, const std::string& content);
bool WriteFile(const std::string& file, const std::vector<std::string>& lines);

// 解析json字符串，如失败则返回空的Json::Value
Json::Value ParseJsonString(const std::string& content);

// 将json数据转化成字符串
std::string DumpJsonValue(const Json::Value& content);

// 读取json文件并解析，如失败则返回空的Json::Value
Json::Value ReadJsonFile(const std::string& json_file);

// 写json结构到文件，必要的时候生成必须的目录
void WriteJsonFile(const Json::Value& root, const std::string& json_file);

// 运行shell命令, 出错返回空字符串
std::string ExecShell(const std::string& cmd);

// vector to string, format控制单个value的格式
template <class Dtype>
std::string ToString(const std::vector<Dtype>& values,
                     const std::string& format) {
  std::vector<std::string> string_values;
  for (size_t i = 0; i < values.size(); ++i) {
    string_values.push_back((boost::format(format) % values[i]).str());
  }
  std::string result = boost::algorithm::join(string_values, ", ");
  return std::string("[") + result + std::string("]");
}

// 返回目录中所有的文件和子目录, 结果做升序排列
std::vector<std::string> ListDirectory(
    const std::string& dirname, const std::regex& pattern = std::regex(".*"));

// 计算字符串的md5值
std::string CalcMD5(const std::string& content);

// 返回path所在的磁盘的可用空间的大小, 无效路径返回-1.
int64_t GetAvailableSpace(const std::string& path);

// 返回path指定的文件或者目录的大小, 无效路径返回-1.
// 只考虑普通文件和目录, 不包括链接等其他形式的文件
int64_t GetFileSize(const std::string& path);

// 通过字符串计算字节数, 支持的单位包括: b, k, m, g
// 不区分大小写, 支持小数, 比如: "1.5k", "0.3G"
int64_t GetBytesFromString(const std::string& content);

// 将bytes转换成利于人读的字符串
std::string GetBytesString(int64_t bytes);

#endif  // PUBLIC_UTIL_H_

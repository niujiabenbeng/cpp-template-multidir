#include "util.h"

#include <openssl/md5.h>

#include "common.h"

using UnitValuePair = std::pair<std::string, int64_t>;
using UnitValueVec = std::vector<UnitValuePair>;

static int64_t ParseUnitString(std::string content, const UnitValueVec& map) {
  boost::algorithm::to_lower(content);
  for (const auto& pair : map) {
    if (boost::algorithm::ends_with(content, pair.first)) {
      auto length = content.length() - pair.first.length();
      auto head = content.substr(0, length);
      auto amount = std::atof(head.c_str()) * pair.second;
      return int64_t(std::round(amount));
    }
  }
  LOG(ERROR) << "Unknown format: " << content;
  return -1;
}

static std::string DumpUnitString(int64_t value, const UnitValueVec& map) {
  for (size_t i = 0; i < map.size() - 1; ++i) {
    if (value >= map[i].second) {
      auto amount = float(value) / float(map[i].second);
      return (boost::format("%.2f %s") % amount % map[i].first).str();
    }
  }
  auto amount = float(value) / float(map.back().second);
  return (boost::format("%.2f %s") % amount % map.back().first).str();
}

//////////////////////////////// implementation ////////////////////////////////

void MakeDirsForFile(const std::string& path) {
  auto dirname = boost::filesystem::absolute(path).parent_path();
  if (!dirname.empty() && !boost::filesystem::exists(dirname)) {
    boost::filesystem::create_directories(dirname);
  }
}

std::string ReadFile(const std::string& file, bool is_binary) {
  std::ios_base::openmode mode = std::ios_base::in;
  if (is_binary) { mode |= std::ios_base::binary; }
  std::ifstream infile(file.c_str(), mode);
  if (!infile.is_open()) { return std::string(); }
  return std::string((std::istreambuf_iterator<char>(infile)),
                     std::istreambuf_iterator<char>());
}

bool WriteFile(const std::string& file, const char* data, int length) {
  MakeDirsForFile(file);
  std::ofstream outfile(file, std::ios_base::binary);
  if (!outfile.is_open()) { return false; }
  outfile.write(data, length);
  return true;
}

bool WriteFile(const std::string& file, const std::string& content) {
  return WriteFile(file, content.data(), content.length());
}

bool WriteFile(const std::string& file, const std::vector<std::string>& lines) {
  return WriteFile(file, boost::algorithm::join(lines, "\n") + "\n");
}

Json::Value ParseJsonString(const std::string& content) {
  Json::Value root;
  if (content.empty()) { return root; }

  std::string error;
  Json::CharReaderBuilder builder;
  Json::CharReader* reader = builder.newCharReader();
  const char* begin = content.data();
  const char* end = begin + content.length();
  if (!reader->parse(begin, end, &root, &error)) {
    LOG(ERROR) << "failed to parse json string, error: " << error;
    root = Json::Value();
  }
  delete reader;
  return root;
}

std::string DumpJsonValue(const Json::Value& content) {
  Json::StreamWriterBuilder builder;
  builder["indentation"] = "";
  return Json::writeString(builder, content);
}

Json::Value ReadJsonFile(const std::string& json_file) {
  std::string content = ReadFile(json_file);
  if (content.empty()) {
    LOG(INFO) << "failed to read file: " << json_file;
    return Json::Value();
  }
  return ParseJsonString(content);
}

void WriteJsonFile(const Json::Value& root, const std::string& json_file) {
  MakeDirsForFile(json_file);
  std::ofstream outfile(json_file.c_str());
  CHECK(outfile.is_open()) << "failed to write to file: " << json_file;
  Json::StreamWriterBuilder builder;
  Json::StreamWriter* writer = builder.newStreamWriter();
  writer->write(root, &outfile);
  delete writer;
}

std::string ExecShell(const std::string& cmd) {
  std::string result;
  std::array<char, 128> buffer = {};
  FILE* pipe = popen(cmd.c_str(), "r");
  if (pipe == nullptr) {
    LOG(ERROR) << "popen() failed! " << strerror(errno);
    return std::string();
  }
  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    result += buffer.data();
  }
  pclose(pipe);
  return result;
}

std::vector<std::string> ListDirectory(const std::string& dirname,
                                       const std::regex& pattern) {
  namespace bf = boost::filesystem;
  std::vector<std::string> names;
  for (const auto& entry : bf::directory_iterator(dirname)) {
    auto path = entry.path().filename().string();
    if (std::regex_match(path, pattern)) { names.push_back(path); }
  }
  std::sort(names.begin(), names.end());
  return names;
}

std::string CalcMD5(const std::string& content) {
  std::array<unsigned char, MD5_DIGEST_LENGTH> md5 = {};
  MD5((unsigned char*) content.data(), content.size(), md5.data());
  std::string result;
  for (const auto& c : md5) {
    result += (boost::format("%02x") % int(c)).str();
  }
  return result;
}

int64_t GetAvailableSpace(const std::string& path) {
  bf::path check_path(path);
  if (bf::exists(check_path)) {
    return bf::space(check_path).available;
  }  // NOFORMAT(-2:)
  if (check_path.has_parent_path()) {
    return GetAvailableSpace(check_path.parent_path().string());
  }
  return -1;
}

int64_t GetFileSize(const std::string& path) {
  // linux系统中, 一个文件夹占4096个字节
  const int64_t empty_dir_size = 4096;
  if (!bf::exists(path)) { return -1; }
  if (bf::is_regular_file(path)) { return bf::file_size(path); }
  if (bf::is_directory(path)) {
    int64_t total = empty_dir_size;
    using Iterator = bf::recursive_directory_iterator;
    for (Iterator iter(path); iter != Iterator{}; ++iter) {
      if (bf::is_regular_file(iter->path())) {
        total += bf::file_size(iter->path());
      } else if (bf::is_directory(iter->path())) {
        total += empty_dir_size;
      }
    }
    return total;
  }
  return -1;
}

int64_t GetBytesByString(std::string content) {
  const UnitValueVec map = {
    // 算法匹配最前面的pair, 所以这里顺序很重要
    {"kb", 1024},
    {"mb", 1024*1024},
    {"gb", 1024*1024*1024},
    {"b", 1},
    {"k", 1024},
    {"m", 1024*1024},
    {"g", 1024*1024*1024},
  };  // NOFORMAT(-9:)
  return ParseUnitString(std::move(content), map);
}

std::string GetBytesString(int64_t bytes) {
  const UnitValueVec map = {
    // 算法匹配最前面的pair, 所以这里顺序很重要
    {"GB", 1024*1024*1024},
    {"MB", 1024*1024},
    {"KB", 1024},
    {"B", 1},
  };  // NOFORMAT(-6:)
  return DumpUnitString(bytes, map);
}

int64_t GetSecondsByString(std::string content) {
  const UnitValueVec map =  {
    {"s", 1},         {"sec",  1},
    {"m", 60},        {"min",  60},
    {"h", 60*60},     {"hour", 60*60},
    {"d", 24*60*60},  {"day",  24*60*60}
  };  // NOFORMAT(-5:)
  return ParseUnitString(std::move(content), map);
}

std::string GetSecondsString(int64_t seconds) {
  const UnitValueVec map =  {
    // 算法匹配最前面的pair, 所以这里顺序很重要
    {"day",  24*60*60},
    {"hour", 60*60},
    {"min",  60},
    {"sec",  1}
  };  // NOFORMAT(-6:)
  return DumpUnitString(seconds, map);
}

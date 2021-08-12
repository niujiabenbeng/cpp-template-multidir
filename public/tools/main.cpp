#include <gflags/gflags.h>
#include <glog/logging.h>

#include "util.h"

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  google::LogToStderr();
  google::ParseCommandLineFlags(&argc, &argv, true);

  std::vector<unsigned char> uchar_vec = {'1', '2', '3'};
  LOG(INFO) << "uchar vector: " << ToString(uchar_vec);

  std::vector<char> char_vec = {1, 2, 3};
  LOG(INFO) << "char vector: " << ToString(char_vec);

  std::vector<int> int_vec = {1, 2, 3};
  LOG(INFO) << "int vector: " << ToString(int_vec);

  std::vector<float> float_vec = {0.1, 1.0, 1.112};
  LOG(INFO) << "float vector: " << ToString(float_vec);

  std::vector<double> double_vec = {0.1, 1.0, 1.112};
  LOG(INFO) << "double vector: " << ToString(double_vec);

  std::vector<std::string> string_vec = {"0.1", "1.0", "1.112"};
  LOG(INFO) << "string vector: " << ToString(string_vec);

  using StringIntPair = std::pair<std::string, int>;
  std::vector<StringIntPair> pair_vec = {{"one", 1}, {"two", 2}};
  auto converter = [](const StringIntPair& pair) {
    return (boost::format("%s: %d") % pair.first % pair.second).str();
  };
  LOG(INFO) << "pair vector: " << ToString(pair_vec, converter);
  return 0;
}

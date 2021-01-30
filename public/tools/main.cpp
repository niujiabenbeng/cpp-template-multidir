#include <glog/logging.h>
#include <gtest/gtest.h>

#include "common.h"
#include "thread_pool.h"
#include "timer.h"
#include "util.h"

TEST(FileIOTest, fileio) {
  auto tempfile = boost::filesystem::unique_path().string();
  std::vector<std::string> lines = {"hello", "world"};
  EXPECT_TRUE(WriteFile(tempfile, lines));
  EXPECT_EQ(ReadFile(tempfile), "hello\nworld\n");
  if (boost::filesystem::exists(tempfile)) {
    boost::filesystem::remove(tempfile);
  }
}

TEST(ThreadPoolTest, pool) {
  ThreadPool pool(4);
  auto result = pool.enqueue([](int answer) { return answer; }, 42);
  EXPECT_EQ(result.get(), 42);
}

TEST(JsonTest, json) {
  Json::Value root;
  root["one"] = 1;
  root["hello"] = "world";
  auto tempfile = boost::filesystem::unique_path().string();
  WriteJsonFile(root, tempfile);
  auto content = ReadJsonFile(tempfile);
  EXPECT_EQ(content.get("one", 0).asInt(), 1);
  EXPECT_EQ(content.get("hello", "").asString(), "world");
  if (boost::filesystem::exists(tempfile)) {
    boost::filesystem::remove(tempfile);
  }
}

TEST(TimeUtilTest, time) {
  auto timestamp = TimeUtil::now();
  auto timestamp_str = TimeUtil::ToString(timestamp);
  EXPECT_EQ(TimeUtil::FromString(timestamp_str), timestamp);
  int64_t interval = static_cast<int64_t>(1.5 * 3600 * 1000);
  auto interval_str = TimeUtil::ToHumanReadableString(interval);
  EXPECT_EQ(TimeUtil::FromHumanReadableString(interval_str), interval);
  int64_t day_start = TimeUtil::GetStartOfDay();
  std::string format = "%Y-%m-%d %H:%M:%S";
  auto day_start_str = TimeUtil::ToDatetimeString(day_start, format);
  EXPECT_EQ(TimeUtil::FromDateTimeString(day_start_str, format), day_start);
}

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  google::LogToStderr();

  // 这两个函数不适合用gtest, 所以直接输出结果
  auto now = TimeUtil::now();
  auto start = TimeUtil::GetStartOfDay();
  auto format = std::string("%Y-%m-%d %H:%M:%S");
  LOG(INFO) << "Current Time: " << TimeUtil::ToDatetimeString(now, format);
  LOG(INFO) << "today start: " << TimeUtil::ToDatetimeString(start, format);
  LOG(INFO) << "available space: " << GetBytesString(GetAvailableSpace("./"));
  LOG(INFO) << "total size: " << GetBytesString(GetFileSize("./"));
  LOG(INFO) << "which gcc: " << ExecShell("which gcc");
  std::vector<float> float_vec{0.01, 0.1, 1.0, 10.0, 100.0};
  LOG(INFO) << "some numbers: " << ToString(float_vec, "%.4f");

  Timer timer;
  timer.Start();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  LOG(INFO) << "Sleeping 1 sec uses: " << timer.Seconds() << "s";

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

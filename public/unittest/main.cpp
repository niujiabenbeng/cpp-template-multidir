#include <glog/logging.h>
#include <gtest/gtest.h>

#include "common.h"
#include "thread_pool.h"
#include "timer.h"
#include "util.h"

TEST(FileIOTest, fileio) {  // NOLINT
  auto tempfile = boost::filesystem::unique_path().string();
  std::vector<std::string> lines = {"hello", "world"};
  EXPECT_TRUE(WriteFile(tempfile, lines));
  EXPECT_EQ(ReadFile(tempfile), "hello\nworld\n");
  if (boost::filesystem::exists(tempfile)) {
    boost::filesystem::remove(tempfile);
  }
}

TEST(ThreadPoolTest, pool) {  // NOLINT
  ThreadPool pool(4);
  auto result = pool.enqueue([](int answer) { return answer; }, 42);
  EXPECT_EQ(result.get(), 42);
}

TEST(JsonTest, json) {  // NOLINT
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

TEST(UnitDurationTest, unitduration) {  // NOLINT
  auto duration = UnitDuration(std::chrono::seconds(16));
  auto duration2 = UnitDuration(duration.string(false));
  auto duration3 = UnitDuration(duration.string(true));
  EXPECT_TRUE(duration.value == duration2.value);
  EXPECT_TRUE(duration.value == duration3.value);
}

TEST(DateTimeTest, datetime) {  // NOLINT
  auto dt = DateTime().seconds();
  auto dt2 = DateTime(dt.string());
  EXPECT_TRUE(dt.value == dt2.value);
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

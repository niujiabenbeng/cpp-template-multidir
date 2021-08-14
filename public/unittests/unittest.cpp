#include <glog/logging.h>
#include <gtest/gtest.h>

#include "common.h"
#include "thread_pool.h"
#include "timer.h"
#include "util.h"

// NOLINTGLOBAL(cppcoreguidelines-avoid-non-const-global-variables)
// NOLINTGLOBAL(cppcoreguidelines-special-member-functions)
// NOLINTGLOBAL(cppcoreguidelines-pro-type-vararg)

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

TEST(DateTimeTest, datetime) {
  auto dt = DateTime().seconds();
  auto dt2 = DateTime(dt.string());
  EXPECT_TRUE(dt.value == dt2.value);
}

TEST(BytesTest, bytes) {
  EXPECT_EQ(GetBytesByString("512K"), GetBytesByString("0.5MB"));
  EXPECT_EQ(GetBytesByString("1024K"), GetBytesByString("1MB"));
  EXPECT_TRUE(GetBytesString(GetBytesByString("1M")) == "1.00 MB");
  EXPECT_TRUE(GetBytesString(GetBytesByString("0.5M")) == "512.00 KB");
}

TEST(SecondsTest, seconds) {
  EXPECT_EQ(GetSecondsByString("24h"), GetSecondsByString("1day"));
  EXPECT_EQ(GetSecondsByString("12h"), GetSecondsByString("12 hour"));
  EXPECT_TRUE(GetSecondsString(GetSecondsByString("0.5D")) == "12.00 hour");
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

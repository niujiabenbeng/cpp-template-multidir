#ifndef PUBLIC_TIMER_H_
#define PUBLIC_TIMER_H_

#include "common.h"

class Timer {
 public:
  using system_clock = std::chrono::system_clock;
  using second_type = std::chrono::duration<float>;
  using millisecond_type = std::chrono::duration<float, std::milli>;
  using microsecond_type = std::chrono::duration<float, std::micro>;

  Timer() {
    start_ = stop_ = system_clock::now();
    total_ = system_clock::duration::zero();
  }
  DEFAULT_COPY_ASIGN(Timer);
  DEFAULT_MOVE_ASIGN(Timer);
  ~Timer() {}

  void Start() {
    CHECK(!is_running_) << "Timer is already started.";
    start_ = system_clock::now();
    is_running_ = true;
  }
  void Stop() {
    CHECK(is_running_) << "Timer is not started yet.";
    stop_ = system_clock::now();
    is_running_ = false;
    has_run_once_ = true;
    has_accumulated_ = false;
  }
  float MilliSeconds() {
    if (is_running_) { this->Stop(); }
    if (!has_run_once_) { return -1.0f; }
    return std::chrono::duration_cast<millisecond_type>(stop_ - start_).count();
  }
  float MicroSeconds() {
    if (is_running_) { this->Stop(); }
    if (!has_run_once_) { return -1.0f; }
    return std::chrono::duration_cast<microsecond_type>(stop_ - start_).count();
  }
  float Seconds() {
    if (is_running_) { this->Stop(); }
    if (!has_run_once_) { return -1.0f; }
    return std::chrono::duration_cast<second_type>(stop_ - start_).count();
  }

  void Accumulate() {
    if (is_running_) { this->Stop(); }
    CHECK(has_run_once_) << "Timer has never been run at all.";
    CHECK(!has_accumulated_) << "This interval has already been accumulated.";
    total_ += stop_ - start_;
    count_ += 1;
    has_accumulated_ = true;
  }
  float TotalMilliSeconds() {
    if (count_ <= 0) { return -1.0f; }
    return std::chrono::duration_cast<millisecond_type>(total_).count();
  }
  float TotalMicroSeconds() {
    if (count_ <= 0) { return -1.0f; }
    return std::chrono::duration_cast<microsecond_type>(total_).count();
  }
  float TotalSeconds() {
    if (count_ <= 0) { return -1.0f; }
    return std::chrono::duration_cast<second_type>(total_).count();
  }
  float AverageMilliSeconds() {
    if (count_ <= 0) { return -1.0f; }
    return this->TotalMilliSeconds() / count_;
  }
  float AverageMicroSeconds() {
    if (count_ <= 0) { return -1.0f; }
    return this->TotalMicroSeconds() / count_;
  }
  float AverageSeconds() {
    if (count_ <= 0) { return -1.0f; }
    return this->TotalSeconds() / count_;
  }
  void ResetAccumulator() {
    total_ = system_clock::duration::zero();
    count_ = 0;
    has_accumulated_ = false;
  }
  int count() const { return count_; }
  bool has_run_once() const { return has_run_once_; }

 private:
  system_clock::time_point start_;
  system_clock::time_point stop_;
  system_clock::duration total_;
  bool is_running_ = false;
  bool has_run_once_ = false;
  bool has_accumulated_ = false;
  int count_ = 0;
};

class FrequencyCounter {
 public:
  using system_clock = std::chrono::system_clock;
  using second_type = std::chrono::duration<float>;

  // 这里interval以秒为单位, interval=0.001为毫秒
  explicit FrequencyCounter(float interval = 1.0f) : interval_(interval) {}
  DEFAULT_COPY_ASIGN(FrequencyCounter);
  DEFAULT_MOVE_ASIGN(FrequencyCounter);
  ~FrequencyCounter() {}

  float accumulate(int times = 1, float default_value = -1.0f) {
    if (!is_started_) {
      stamp_ = system_clock::now();
      is_started_ = true;
    }
    count_ += times;
    auto current = system_clock::now();
    auto interval = std::chrono::duration_cast<second_type>(current - stamp_);
    float result = default_value;
    if (interval >= interval_) {
      result = float(count_) * interval_.count() / interval.count();
      stamp_ = current;
      count_ = 0;
    }
    return result;
  }
  void reset() {
    count_ = 0;
    is_started_ = false;
  }

 private:
  second_type interval_;
  system_clock::time_point stamp_;
  int count_ = 0;
  bool is_started_ = false;
};

// 本类统一时间点和时间段, 时间点取unix时间. 两者都以毫秒为单位.
class TimeUtil {
 public:
  // 返回当前时间
  static int64_t now();

  // 返回给定时刻所在天的起始时间, 若输入为负值, 则返回今天的起始时间
  static int64_t GetStartOfDay(int64_t timestamp = -1);

  // 从字符串中读取时间, 实际上是: std::string -> int64_t
  static int64_t FromString(const std::string& content);

  // 从human readable字符串中读取时间, 格式为: 数字+单位. 单位支持如下的简写:
  // s, sec, m, min, h, hour, d, day. 示例: "16s", "5.4 day".
  static int64_t FromHumanReadableString(const std::string& content);

  // 从日期字符串中读取时间, 字符串格式由format指定.
  static int64_t FromDateTimeString(const std::string& content,
                                    const std::string& format);

  // 将timestamp转化为字符串, 实际上是: int64_t -> std::string
  static std::string ToString(const int64_t timestamp);

  // 将timestamp转化为human readable类型的字符串
  static std::string ToHumanReadableString(const int64_t timestamp);

  // 将timestamp转化为日期字符串, 格式由format指定
  static std::string ToDatetimeString(const int64_t timestamp,
                                      const std::string& format);
};

#endif  // PUBLIC_TIMER_H_

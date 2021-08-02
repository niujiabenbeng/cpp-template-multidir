#ifndef CPP_TEMPLATE_TIMER_H_
#define CPP_TEMPLATE_TIMER_H_

#include <date/date.h>
#include <date/tz.h>

#include "common.h"

class Timer {
 public:
  using SystemClock = std::chrono::system_clock;
  using SecondType = std::chrono::duration<float>;
  using MilliSecondType = std::chrono::duration<float, std::milli>;
  using MicroSecondType = std::chrono::duration<float, std::micro>;
  PLAIN_OLD_DATA_CLASS(Timer);

  void Start(bool check_status = true) {
    CHECK(!check_status || !is_running_) << "Timer is already started.";
    start_ = SystemClock::now();
    is_running_ = true;
  }
  void Stop(bool check_status = true) {
    CHECK(!check_status || is_running_) << "Timer is not started yet.";
    stop_ = SystemClock::now();
    is_running_ = false;
    has_run_once_ = true;
    has_accumulated_ = false;
  }

  float Seconds() {
    using std::chrono::duration_cast;
    if (is_running_) { this->Stop(); }
    if (!has_run_once_) { return -1.0F; }
    return duration_cast<SecondType>(stop_ - start_).count();
  }
  float MilliSeconds() {
    using std::chrono::duration_cast;
    if (is_running_) { this->Stop(); }
    if (!has_run_once_) { return -1.0F; }
    return duration_cast<MilliSecondType>(stop_ - start_).count();
  }
  float MicroSeconds() {
    using std::chrono::duration_cast;
    if (is_running_) { this->Stop(); }
    if (!has_run_once_) { return -1.0F; }
    return duration_cast<MicroSecondType>(stop_ - start_).count();
  }

  float TotalSeconds() {
    using std::chrono::duration_cast;
    if (count_ <= 0) { return -1.0F; }
    return duration_cast<SecondType>(total_).count();
  }
  float TotalMilliSeconds() {
    using std::chrono::duration_cast;
    if (count_ <= 0) { return -1.0F; }
    return duration_cast<MilliSecondType>(total_).count();
  }
  float TotalMicroSeconds() {
    using std::chrono::duration_cast;
    if (count_ <= 0) { return -1.0F; }
    return duration_cast<MicroSecondType>(total_).count();
  }

  float AverageSeconds() {
    if (count_ <= 0) { return -1.0F; }
    return this->TotalSeconds() / count_;
  }
  float AverageMilliSeconds() {
    if (count_ <= 0) { return -1.0F; }
    return this->TotalMilliSeconds() / count_;
  }
  float AverageMicroSeconds() {
    if (count_ <= 0) { return -1.0F; }
    return this->TotalMicroSeconds() / count_;
  }

  void Accumulate() {
    if (is_running_) { this->Stop(); }
    CHECK(has_run_once_);
    CHECK(!has_accumulated_);
    total_ += stop_ - start_;
    count_ += 1;
    has_accumulated_ = true;
  }
  void ResetAccumulator() {
    total_ = SystemClock::duration::zero();
    count_ = 0;
    has_accumulated_ = false;
  }

  int count() const { return count_; }
  bool has_run_once() const { return has_run_once_; }

 private:
  SystemClock::time_point start_{SystemClock::now()};
  SystemClock::time_point stop_{SystemClock::now()};
  SystemClock::duration total_{SystemClock::duration::zero()};
  bool is_running_ = false;
  bool has_run_once_ = false;
  bool has_accumulated_ = false;
  int count_ = 0;
};

class FrequencyCounter {
 public:
  using SystemClock = std::chrono::system_clock;
  using Duration = SystemClock::duration;
  using TimePoint = SystemClock::time_point;
  PLAIN_OLD_DATA_CLASS(FrequencyCounter);
  explicit FrequencyCounter(Duration interval) : interval_(interval) {}

  void Reset() {
    count_ = 0;
    stamp_ = SystemClock::now();
  }
  float Accumulate(int times = 1, float default_value = -1.0F) {
    count_ += times;
    auto elapsed = SystemClock::now() - stamp_;
    if (elapsed < interval_) { return default_value; }
    auto result = (interval_ / elapsed) * count_;
    this->Reset();
    return result;
  }

 private:
  Duration interval_{std::chrono::seconds(1)};
  TimePoint stamp_{std::chrono::system_clock::now()};
  int count_{0};
};

class UnitDuration {
 public:
  using Duration = std::chrono::system_clock::duration;
  using SecondType = std::chrono::duration<float>;

  PLAIN_OLD_DATA_CLASS(UnitDuration);
  explicit UnitDuration(Duration d) : value(d) {}
  explicit UnitDuration(const std::string& content);
  std::string string(bool short_unit = false) const;

  Duration value{Duration::zero()};  // NOLINT
};

class DateTime {
 public:
  using SystemClock = std::chrono::system_clock;
  using TimePoint = SystemClock::time_point;
  using Duration = SystemClock::duration;

  PLAIN_OLD_DATA_CLASS(DateTime);
  explicit DateTime(TimePoint t) : value(t) {}
  explicit DateTime(const std::string& content) {
    date::local_time<Duration> local_time;
    std::stringstream ss(content);
    date::from_stream(ss, "%Y-%m-%d %H:%M:%S", local_time);
    // 时区的剥离与添加实际上是time_zone做的
    value = date::current_zone()->to_sys(local_time);
  }

  std::string string() const {
    std::stringstream ss;
    auto zoned = date::make_zoned(date::current_zone(), value);
    date::to_stream(ss, "%Y-%m-%d %H:%M:%S", zoned);
    return ss.str().substr(0, 19);
  }
  DateTime seconds() const {
    using std::chrono::floor;
    using std::chrono::seconds;
    auto stamp = value.time_since_epoch();
    auto sec = floor<seconds>(stamp);
    return DateTime(TimePoint{sec});
  }

  TimePoint value{SystemClock::now()};  // NOLINT
};

#endif  // CPP_TEMPLATE_TIMER_H_

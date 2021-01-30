#include "timer.h"

#include <string>

int64_t TimeUtil::now() {
  using milliseconds = std::chrono::milliseconds;
  auto now = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<milliseconds>(now).count();
}

int64_t TimeUtil::GetStartOfDay(int64_t timestamp) {
  if (timestamp < 0) { timestamp = TimeUtil::now(); }
  std::time_t seconds = timestamp / 1000;
  std::tm* datetime = std::gmtime(&seconds);
  datetime->tm_sec = 0;
  datetime->tm_min = 0;
  datetime->tm_hour = 0;
  return std::mktime(datetime) * 1000;
}

int64_t TimeUtil::FromString(const std::string& content) {
  return static_cast<int64_t>(std::atoll(content.c_str()));
}

int64_t TimeUtil::FromHumanReadableString(const std::string& content) {
  auto lower = boost::algorithm::to_lower_copy(content);
  // clang-format off
  std::vector<std::pair<std::string, int>> unit_value_vec = {
      {"s",    1},
      {"sec",  1},
      {"m",    60},
      {"min",  60},
      {"h",    60 * 60},
      {"hour", 60 * 60},
      {"d",    24 * 60 * 60},
      {"day",  24 * 60 * 60}
  };
  // clang-format on
  for (const auto& pair : unit_value_vec) {
    if (boost::algorithm::ends_with(lower, pair.first)) {
      auto head = lower.substr(0, lower.length() - pair.first.length());
      auto amount = std::atof(head.c_str()) * pair.second;
      return static_cast<int64_t>(amount * 1000);
    }
  }
  LOG(ERROR) << "Unknown format: " << content;
  return -1;
}

int64_t TimeUtil::FromDateTimeString(const std::string& content,
                                     const std::string& format) {
  struct tm tm;
  strptime(content.c_str(), format.c_str(), &tm);
  return std::mktime(&tm) * 1000;
}

std::string TimeUtil::ToString(const int64_t timestamp) {
  return std::to_string(timestamp);
}

std::string TimeUtil::ToHumanReadableString(const int64_t timestamp) {
  float amount = std::abs(timestamp / 1000);
  std::string unit = "sec";
  // clang-format off
  if (unit == "sec"  && amount > 60) { amount /= 60; unit = "min";  }
  if (unit == "min"  && amount > 60) { amount /= 60; unit = "hour"; }
  if (unit == "hour" && amount > 24) { amount /= 24; unit = "day";  }
  if (timestamp < 0) { amount *= -1.0; }
  // clang-format on
  return (boost::format("%.2f %s") % amount % unit).str();
}

std::string TimeUtil::ToDatetimeString(const int64_t timestamp,
                                       const std::string& format) {
  char buffer[64] = {0};
  time_t seconds = timestamp / 1000;
  strftime(buffer, 64, format.c_str(), localtime(&seconds));
  return std::string(buffer);
}

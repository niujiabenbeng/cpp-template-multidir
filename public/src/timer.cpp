#include "timer.h"

UnitDuration::UnitDuration(const std::string& content) {
  // clang-format off
  std::map<std::string, int> unit_value_map =  {
    {"s", 1},         {"sec",  1},
    {"m", 60},        {"min",  60},
    {"h", 60*60},     {"hour", 60*60},
    {"d", 24*60*60},  {"day",  24*60*60}
  };
  // clang-format on
  using std::chrono::duration_cast;
  auto lower = boost::algorithm::to_lower_copy(content);
  for (const auto& pair : unit_value_map) {
    if (boost::algorithm::ends_with(lower, pair.first)) {
      auto head = lower.substr(0, lower.length() - pair.first.length());
      auto amount = std::atof(head.c_str()) * pair.second;
      value = duration_cast<Duration>(SecondType(amount));
      return;
    }
  }
  LOG(ERROR) << "Unknown format: " << content;
}

std::string UnitDuration::string(bool short_unit) const {
  constexpr auto second = SecondType(1);
  constexpr auto minute = second * 60;
  constexpr auto hour = minute * 60;
  constexpr auto day = hour * 64;
  auto abs_value = std::chrono::abs(value);
  if (abs_value >= day) {
    auto amount = value / day;
    auto unit = std::string(short_unit ? "d" : "day");
    return (boost::format("%.2f %s") % amount % unit).str();
  }
  if (abs_value >= hour) {
    auto amount = value / hour;
    auto unit = std::string(short_unit ? "h" : "hour");
    return (boost::format("%.2f %s") % amount % unit).str();
  }
  if (abs_value >= minute) {
    auto amount = value / minute;
    auto unit = std::string(short_unit ? "m" : "min");
    return (boost::format("%.2f %s") % amount % unit).str();
  }
  auto amount = value / second;
  auto unit = std::string(short_unit ? "s" : "sec");
  return (boost::format("%.2f %s") % amount % unit).str();
}

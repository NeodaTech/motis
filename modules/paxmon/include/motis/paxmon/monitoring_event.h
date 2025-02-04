#pragma once

#include <cstdint>
#include <optional>

#include "motis/core/schedule/time.h"

#include "motis/paxmon/broken_transfer_info.h"
#include "motis/paxmon/index_types.h"
#include "motis/paxmon/localization.h"
#include "motis/paxmon/reachability.h"

namespace motis::paxmon {

enum class monitoring_event_type : std::uint8_t {
  NO_PROBLEM,
  BROKEN_TRANSFER,
  MAJOR_DELAY_EXPECTED
};

struct monitoring_event {
  monitoring_event_type type_{monitoring_event_type::NO_PROBLEM};
  passenger_group_with_route pgwr_;
  passenger_localization localization_;
  reachability_status reachability_status_{reachability_status::OK};
  time expected_arrival_time_{INVALID_TIME};
  std::optional<broken_transfer_info> broken_transfer_;
};

}  // namespace motis::paxmon

#pragma once

#include <string>
#include <vector>

#include "motis/module/module.h"
#include "motis/protocol/Message_generated.h"
#include "geo/latlng.h"

namespace motis::ondemand {

struct ondemand : public motis::module::module {
public:
  ondemand();
  void init(motis::module::registry&) override;

private:
  motis::module::msg_ptr calculate(motis::module::msg_ptr const&);
  motis::module::msg_ptr add_taxi(motis::module::msg_ptr const&);

  motis::module::msg_ptr make_osrm_request(
          Position const* pos, std::vector<Position> const* destinations,
          std::string const& profile, SearchDir direction);
  //motis::module::msg_ptr make_geo_request(geo::latlng const& pos, double radius);
};

}  // namespace motis::intermodal

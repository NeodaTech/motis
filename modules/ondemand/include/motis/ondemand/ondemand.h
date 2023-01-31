#pragma once

#include <string>

#include "motis/module/module.h"

namespace motis::ondemand {

struct ondemand : public motis::module::module {
public:
  ondemand();
  void init(motis::module::registry&) override;

private:
  motis::module::msg_ptr calculate(motis::module::msg_ptr const&);
  motis::module::msg_ptr add_taxi(motis::module::msg_ptr const&);
};

}  // namespace motis::intermodal

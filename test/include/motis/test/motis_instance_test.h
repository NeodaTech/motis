#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "motis/core/access/time_access.h"
#include "motis/bootstrap/motis_instance.h"
#include "motis/loader/loader_options.h"

namespace motis::test {

struct motis_instance_test : public ::testing::Test {
  explicit motis_instance_test(
      loader::loader_options const&,
      std::vector<std::string> const& modules = {},
      std::vector<std::string> const& modules_cmdline_opt = {});

  template <typename F>
  void subscribe(std::string const& topic, F&& func) {
    instance_->subscribe(topic, std::forward<F>(func), {});
  }

  template <typename Fn>
  auto run(Fn&& fn) {
    return instance_->run(fn);
  }

  module::msg_ptr call(std::string const& target) const;
  module::msg_ptr call(module::msg_ptr const&) const;
  void publish(std::string const& target) const;
  void publish(module::msg_ptr const&) const;

  static std::function<module::msg_ptr(module::msg_ptr const&)> msg_sink(
      std::vector<module::msg_ptr>*);

  ::motis::schedule const& sched() const;

  std::time_t unix_time(int hhmm, int day_idx = 0,
                        int timezone_offset = DEFAULT_TIMEZONE_OFFSET) const;

  template <typename Module>
  Module& get_module(std::string const& module_name) {
    auto it = std::find_if(
        instance_->modules_.begin(), instance_->modules_.end(),
        [&module_name](auto const& m) { return m->name() == module_name; });
    if (it == instance_->modules_.end()) {
      throw std::runtime_error("module not found");
    }
    return *reinterpret_cast<Module*>(it->get());
  }

  std::unique_ptr<bootstrap::motis_instance> instance_;
};

}  // namespace motis::test

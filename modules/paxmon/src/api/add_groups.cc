#include "motis/paxmon/api/add_groups.h"

#include "utl/to_vec.h"
#include "utl/verify.h"

#include "motis/core/common/logging.h"

#include "motis/paxmon/build_graph.h"
#include "motis/paxmon/get_universe.h"
#include "motis/paxmon/messages.h"
#include "motis/paxmon/print_stats.h"

using namespace motis::module;
using namespace motis::paxmon;
using namespace motis::logging;

namespace motis::paxmon::api {

msg_ptr add_groups(schedule const& sched, paxmon_data& data,
                   rt_update_context& rt_update_ctx, bool const allow_reuse,
                   msg_ptr const& msg) {
  auto const req = motis_content(PaxMonAddGroupsRequest, msg);
  auto& uv = get_universe(data, req->universe());

  auto reused_groups = 0ULL;
  auto const added_groups =
      utl::to_vec(*req->groups(), [&](PaxMonGroup const* pg_fbs) {
        utl::verify(pg_fbs->planned_journey()->legs()->size() != 0,
                    "trying to add empty passenger group");
        auto input_pg = from_fbs(sched, pg_fbs);
        if (allow_reuse) {
          if (auto it =
                  uv.passenger_groups_.groups_by_source_.find(input_pg.source_);
              it != end(uv.passenger_groups_.groups_by_source_)) {
            for (auto const id : it->second) {
              auto existing_pg = uv.passenger_groups_.at(id);
              if (existing_pg != nullptr && existing_pg->valid() &&
                  existing_pg->compact_planned_journey_ ==
                      input_pg.compact_planned_journey_) {
                existing_pg->probability_ += input_pg.probability_;
                ++reused_groups;
                return existing_pg;
              }
            }
          }
        }
        auto pg = uv.passenger_groups_.add(std::move(input_pg));
        add_passenger_group_to_graph(sched, data.capacity_maps_, uv, *pg);
        for (auto const& leg : pg->compact_planned_journey_.legs_) {
          rt_update_ctx.trips_affected_by_last_update_.insert(leg.trip_);
        }
        return pg;
      });

  print_allocator_stats(uv);
  LOG(info) << "add_groups: " << added_groups.size() << " total, "
            << reused_groups << " reused (universe " << uv.id_ << ")";

  message_creator mc;
  mc.create_and_finish(
      MsgContent_PaxMonAddGroupsResponse,
      CreatePaxMonAddGroupsResponse(
          mc, mc.CreateVector(utl::to_vec(
                  added_groups, [](auto const pg) { return pg->id_; })))
          .Union());
  return make_msg(mc);
}

}  // namespace motis::paxmon::api
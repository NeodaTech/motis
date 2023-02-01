#include "motis/ondemand/ondemand.h"

#include "motis/core/common/logging.h"
#include "motis/protocol/Message_generated.h"
#include "motis/core/conv/position_conv.h"
#include "motis/module/context/motis_call.h"
#include "motis/module/message.h"

using namespace flatbuffers;
using namespace motis::module;
using namespace motis::osrm;
using motis::logging::info;

namespace motis::ondemand {
    ondemand::ondemand() : module("ondemand Options", "ondemand") {}

    void ondemand::init(motis::module::registry& reg) {
        reg.register_op(
            "/ondemand/ondemand", [this](msg_ptr const& m) {
                LOG(info) << "ondemand - register_op";
                return calculate(m);
            }, {});
        reg.register_op(
            "/ondemand/taxi", [this](msg_ptr const& m) {
                LOG(info) << "ondemand - register_op";
                return add_taxi(m);
            }, {});
        LOG(info) << "ondemand - Init";
    }

    msg_ptr ondemand::add_taxi(msg_ptr const& msg) {
        LOG(info) << "add_taxi";
        auto req = motis_content(OndemandTaxiRequest, msg);
        LOG(info) << "ondemand - add_taxi - AddTaxiRequest - Input - " << req->id()->str();
        message_creator mc;
        mc.create_and_finish(
                MsgContent_OndemandResponse,
                CreateOndemandResponse(mc, mc.CreateString("Ondemand Test response")).Union());
        return make_msg(mc);
    }

    msg_ptr ondemand::calculate(msg_ptr const& msg) {
        LOG(info) << "calculate";
        auto req = motis_content(OndemandRequest, msg);
        LOG(info) << "ondemand - calculate - OndemandRequest - Input - " << req->RequestTime();

        //make_geo_request(); // Request to get Geo Locations ?
        Position start_pos{req->InputStartPosition()->lat(), req->InputStartPosition()->lng()};
        std::vector<Position> destinations;
        destinations.emplace_back(req->InputDestinationPosition()->lat(), req->InputDestinationPosition()->lng());

        //make_osrm_request(start_pos, parkings, "car", SearchDir_Backward));
        //make_osrm_request(start_pos, parkings, "car", SearchDir_Forward));
        auto const osrm_msg = motis_call(make_osrm_request(&start_pos, &destinations, "car", SearchDir_Backward))->val();
        auto const osrm_resp = motis_content(OSRMOneToManyResponse, osrm_msg);

        auto const dur = osrm_resp->costs()->Get(0)->duration();
        auto const dist = osrm_resp->costs()->Get(0)->distance();

        LOG(info) << "duration: " << dur;
        LOG(info) << "distance: " << dist;

        message_creator mc;
        mc.create_and_finish(
            MsgContent_OndemandResponse,
            CreateOndemandResponse(mc, mc.CreateString("Ondemand Test response")).Union());
        return make_msg(mc);
    }

    motis::module::msg_ptr ondemand::make_osrm_request(
            Position const* pos, std::vector<Position> const* destinations,
            std::string const& profile, SearchDir direction) {
        message_creator mc;
        mc.create_and_finish(
                MsgContent_OSRMOneToManyRequest,
                CreateOSRMOneToManyRequest(mc, mc.CreateString(profile), direction,
                                           pos, mc.CreateVectorOfStructs(*destinations))
                        .Union(),
                "/osrm/one_to_many");
        return make_msg(mc);
    }

}  // namespace motis::ondemand

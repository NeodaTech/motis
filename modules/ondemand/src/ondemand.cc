#include "motis/ondemand/ondemand.h"

#include "motis/core/common/logging.h"
#include "motis/protocol/Message_generated.h"

using namespace flatbuffers;
using namespace motis::module;
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
        message_creator mc;
        mc.create_and_finish(
            MsgContent_OndemandResponse,
            CreateOndemandResponse(mc, mc.CreateString("Ondemand Test response")).Union());
        return make_msg(mc);
    }
}  // namespace motis::ondemand

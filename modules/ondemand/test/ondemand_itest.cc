#include "gtest/gtest.h"

#include <string>

#include "motis/module/message.h"
#include "motis/test/motis_instance_test.h"
#include "motis/test/schedule/simple_realtime.h"

using namespace motis::osrm;
using namespace motis::test;
using namespace motis::module;
using namespace motis::ondemand;
using motis::test::schedule::simple_realtime::dataset_opt;

namespace motis::ondemand {

struct ondemand_itest : public motis_instance_test {
  ondemand_itest(): motis::test::motis_instance_test(dataset_opt,
                                                     {"osrm", "ondemand"},
                                                     {
                                                      //"--import.paths=osm:data/norcal-latest.osm.pbf",
                                                      //"--import.paths=schedule:data/hrd",
                                                      //"--import.data_dir=data",
                                                      //"--import.require_successful=1",
                                                      "--osrm.profiles=deps/osrm-backend/profiles/car.lua"
                                                      }) {}
};


TEST_F(ondemand_itest, first) {
  auto json_taxi = R"(
    {
      "destination": {
        "type": "Module",
        "target": "/ondemand/taxi"
      },
      "content_type": "OndemandTaxiRequest",
      "content": {
        "id" : "enyenewl",
        "Shifts" : [
            { "start": 1212616413, "stop": 1212650139 },
            { "start": 1212677290, "stop": 1212742039 },
            { "start": 1212779168, "stop": 1212821517 },
            { "start": 1212865812, "stop": 1212914790 },
            { "start": 1212944838, "stop": 1212991838 }
        ],
        "SupportedAreas" : [
            {"Position" : { "lat" : 37.75892, "lng" : -122.44372 }, "Radius" : 50 }
        ]
      }
    }
  )";
  auto json_ondemand = R"(
    {
      "destination": {
        "type": "Module",
        "target": "/ondemand/ondemand"
      },
      "content_type": "OndemandRequest",
      "content": {
        "PickupTime": 1211022191,
        "ArrivalTime": 1211022520,
        "RequestTime": 1209819395,
        "InputStartPosition": {
            "lat": 37.76211,
            "lng": -122.41934
        },
        "InputDestinationPosition": {
            "lat": 37.76224,
            "lng": -122.41937
        }
      }
    }
  )";

  auto res_taxt = call(make_msg(json_taxi));
  auto content_taxt = motis_content(OndemandResponse, res_taxt);
  LOG(motis::logging::info) << content_taxt->output()->c_str();
  //LOG(motis::logging::info) << res_taxt->to_json();
  EXPECT_STREQ("Ondemand Test response", content_taxt->output()->c_str());

  auto res_ondemand = call(make_msg(json_ondemand));
  auto content_OndemandResponse = motis_content(OndemandResponse, res_ondemand);
  LOG(motis::logging::info) << content_OndemandResponse->output()->c_str();
  //LOG(motis::logging::info) << res_ondemand->to_json();
  EXPECT_STREQ("Ondemand Test response", content_OndemandResponse->output()->c_str());
}

}  // namespace motis::ondemand

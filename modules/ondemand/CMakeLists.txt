cmake_minimum_required(VERSION 3.10)
project(motis)

include_directories(include)

file(GLOB_RECURSE motis-ondemand-files src/*.cc)
add_library(motis-ondemand STATIC ${motis-ondemand-files})
target_include_directories(motis-ondemand PUBLIC include)
target_compile_features(motis-ondemand PUBLIC cxx_std_17)
target_link_libraries(motis-ondemand
  motis-osrm
)
target_compile_options(motis-ondemand PRIVATE ${MOTIS_CXX_FLAGS})
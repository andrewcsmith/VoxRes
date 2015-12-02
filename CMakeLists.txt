cmake_minimum_required (VERSION 2.8)
project (VoxRes)
set(SC_PATH /Users/acsmith/workspaces/supercollider)

include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)
include_directories(${SC_PATH}/external_libraries/libsndfile/)

set(CMAKE_SHARED_MODULE_PREFIX "")
if(APPLE OR WIN32)
	set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

# include_directories(/Users/acsmith/workspaces/music/rust/vox_box/target/release)
add_library(VoxRes MODULE VoxRes.cpp)
find_library(VOX_BOX_LIBRARIES NAMES vox_box libvox_box PATHS /Users/acsmith/workspaces/music/rust/vox_box/target/release)
target_link_libraries(VoxRes PRIVATE ${VOX_BOX_LIBRARIES})
install(TARGETS VoxRes DESTINATION .)

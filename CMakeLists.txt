cmake_minimum_required(VERSION 2.6)

project(LPCGame)

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)

add_subdirectory(tinyxml)
add_subdirectory(tmxparser)
include_directories(${SDL_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} tmxparser)
add_executable(load_map load_map.cpp)
target_link_libraries(load_map tmxparser tinyxml ${SDLIMAGE_LIBRARY} ${SDL_LIBRARY}  z)

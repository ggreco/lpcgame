cmake_minimum_required(VERSION 2.6)

project(LPCGame)
set(CMAKE_BUILD_TYPE Debug)

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)

include_directories(${SDL_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} tmxparser tinyxml lua)
add_definitions("-DTIXML_USE_STL")
add_subdirectory(lua)
add_subdirectory(tinyxml)
add_subdirectory(tmxparser)
add_executable(testgame game_map.cpp test.cpp char.cpp obj.cpp anim.cpp findpath.cpp
                        engine.cpp scripting.cpp)
target_link_libraries(testgame tmxparser tinyxml lua ${SDLIMAGE_LIBRARY} ${SDL_LIBRARY}  z)

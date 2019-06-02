project(search-engine CXX)

set(CMAKE_CXX_FLAGS "-Wall -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC "*.cc" "**/*.cc")
add_executable(search-engine ${SRC})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED serialization)

target_link_libraries(search-engine ${Boost_LIBRARIES} stdc++fs readline argument-parser htmlparser)

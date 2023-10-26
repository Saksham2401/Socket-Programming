cmake_minimum_required(VERSION 3.16)

project(boss-deliver CXX)

add_definitions(-DASIO_STANDALONE)

add_executable(server
	src/server.cc
)

add_executable(client
	src/client.cc
)

target_include_directories(server SYSTEM PRIVATE
	"asio/asio/include"
	"asio/include"
)

target_include_directories(client SYSTEM PRIVATE
	"asio/asio/include"
	"asio/include"
)

target_include_directories(server PRIVATE
	"include"
)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

target_link_libraries(server
	Threads::Threads
	Boost::filesystem
)

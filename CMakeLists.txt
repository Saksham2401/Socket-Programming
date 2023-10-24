cmake_minimum_required(VERSION 3.16)

project(boss-deliver CXX)

add_definitions(-DASIO_STANDALONE)

add_executable(${PROJECT_NAME}
	src/server.cc
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
	"asio/asio/include"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"include"
)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME}
	Threads::Threads
)

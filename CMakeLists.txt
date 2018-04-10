cmake_minimum_required (VERSION 3.0)
project (cfg)

add_library(cfg SHARED cfg.cpp cfg_impl.cpp app.cpp)
find_package(DynamoRIO REQUIRED)
configure_DynamoRIO_client(cfg)
use_DynamoRIO_extension(cfg "drmgr")
use_DynamoRIO_extension(cfg "droption")
set(CMAKE_CXX_FLAGS "-std=c++14 -flto")

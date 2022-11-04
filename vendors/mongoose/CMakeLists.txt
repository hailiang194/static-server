cmake_minimum_required(VERSION 3.1)
project(mongoose)

add_library(mongoose ./mongoose.c ./mongoose.h)

set_property(TARGET mongoose PROPERTY C_STANDARD 99)

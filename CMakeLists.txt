cmake_minimum_required(VERSION 3.13.1)

list(INSERT OVERLAY_CONFIG 0 $ENV{ZEPHYR_BASE}/../nrf/samples/openthread/common/overlay-ot-defaults.conf)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(openthread_coap_server)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})

target_include_directories(app PRIVATE interface)

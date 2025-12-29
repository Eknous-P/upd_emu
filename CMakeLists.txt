cmake_minimum_required(VERSION 3.20)
project(upd_emu)

add_executable(upd_emu test.c)
target_sources(upd_emu PRIVATE upd.c)
target_include_directories(upd_emu PRIVATE .)

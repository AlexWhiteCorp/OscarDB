cmake_minimum_required(VERSION 3.13)
project(OscarDB C)

set(CMAKE_C_STANDARD 99)

add_executable(OscarDB main.c src/oscardb.c src/gc.c)
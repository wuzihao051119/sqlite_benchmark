cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(/MT)
add_link_options(/LTCG)
link_libraries(../sqlite/sqlite3 ../uuid4/uuid4)

project(sqlite_use_in LANGUAGES C)
add_executable(sqlite_use_in main.c)
target_compile_definitions(sqlite_use_in PRIVATE DELETE_WITH_IN=1)

project(sqlite_not_use_in LANGUAGES C)
add_executable(sqlite_not_use_in main.c)
target_compile_definitions(sqlite_not_use_in PRIVATE DELETE_WITH_IN=0)

install(TARGETS sqlite_use_in sqlite_not_use_in
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}
)

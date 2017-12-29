find_package(benchmark REQUIRED)

add_executable(utilsbench
    stringbench.cpp
)

target_link_libraries(utilsbench PRIVATE utils benchmark::benchmark)

find_package(benchmark REQUIRED)

add_executable(utilsbench
    trimstringbench.cpp
    splitstringbench.cpp
    joinstringbench.cpp
)

target_link_libraries(utilsbench PRIVATE utils benchmark::benchmark)

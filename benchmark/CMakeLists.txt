# Download and unpack benchmark at configure time
configure_file(CMakeLists.txt.in benchmark-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/benchmark-download )
if(result)
	message(FATAL_ERROR "CMake step for benchmark failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/benchmark-download )
if(result)
	message(FATAL_ERROR "Build step for benchmark failed: ${result}")
endif()

# configure Google Benchmarks
set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/benchmark-src
                 ${CMAKE_CURRENT_BINARY_DIR}/benchmark-build
                 EXCLUDE_FROM_ALL)

add_executable(run-benchmark
    main.cpp
    ${SERVER_SOURCES}
    ${SERVER_HEADERS}
)

set_target_properties(run-benchmark PROPERTIES
	XCODE_GENERATE_SCHEME TRUE
	XCODE_SCHEME_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

target_include_directories(run-benchmark
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/benchmark-src"
    PUBLIC "${PROJECT_SOURCE_DIR}/src"
    PUBLIC "${PROJECT_SOURCE_DIR}/cpp-httplib"
    PUBLIC "${PROJECT_SOURCE_DIR}/json/include"
    PUBLIC "${PROJECT_SOURCE_DIR}/uriparser/include"
)

target_link_libraries(run-benchmark uriparser)
target_link_libraries(run-benchmark httplib::httplib)
target_link_libraries(run-benchmark cxxopts::cxxopts)
target_link_libraries(run-benchmark nlohmann_json::nlohmann_json)
target_link_libraries(run-benchmark benchmark)

install(TARGETS run-benchmark
    RUNTIME DESTINATION bin
    RESOURCE DESTINATION bin
)
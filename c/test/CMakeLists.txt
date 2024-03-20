find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE TEST_HEADERS "${CMAKE_SOURCE_DIR}/test/*.h")
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")

add_executable(iml_test ${HEADERS} ${SOURCES} ${TEST_SOURCES})
target_include_directories(iml_test PRIVATE
        "${CMAKE_SOURCE_DIR}/test/common"
        "${CMAKE_SOURCE_DIR}/include"
)
target_link_libraries(iml_test GTest::gtest GTest::gtest_main)

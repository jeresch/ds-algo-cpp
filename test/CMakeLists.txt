include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG "release-1.10.0"
)

FetchContent_MakeAvailable(googletest)

add_executable(ds-algo-test
    test-main.cpp
)
target_link_libraries(ds-algo-test
    gtest_main
)

add_test(test COMMAND ds-algo-test)

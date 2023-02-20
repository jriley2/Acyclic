set (Scheduler_INCLUDE include)

file(GLOB Scheduler_SRC "src/*.cpp" "src/*.H")
file(GLOB Scheduler_H "${Scheduler_INCLUDE}/*.H")

add_library(Scheduler "${Scheduler_SRC}")

target_include_directories(Scheduler PUBLIC "${PROJECT_BINARY_DIR}/lib/Scheduler/")
target_include_directories(Scheduler PUBLIC "${Scheduler_INCLUDE}")

if (UNIX)
    target_link_libraries(Scheduler PRIVATE pthread)
endif (UNIX)

install(TARGETS Scheduler 
    ARCHIVE DESTINATION lib/Scheduler
)

install(FILES ${Scheduler_H} DESTINATION include/Scheduler)

find_package(GTest QUIET)
if (GTEST_FOUND)
    set(testTarget testAcyclicLib)
    file(GLOB SRC "test/*.cpp")
    include(GoogleTest)
    add_executable(${testTarget} ${SRC})
    target_link_libraries(${testTarget} PRIVATE Scheduler gtest_main gtest)
    gtest_discover_tests(${testTarget})
endif (GTEST_FOUND)

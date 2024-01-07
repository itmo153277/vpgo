
file(GLOB SRC src/*.cc)
add_library(vpgo-core ${SRC})
target_compile_features(vpgo-core PUBLIC cxx_std_23)
target_include_directories(vpgo-core
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/public
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(BUILD_TESTING)
  file(GLOB TEST_SRC test/*.cc)
  add_executable(vpgo-core-test ${TEST_SRC})
  target_compile_features(vpgo-core-test PUBLIC cxx_std_23)
  target_include_directories(vpgo-core-test
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
  )
  target_link_libraries(vpgo-core-test PRIVATE
    vpgo-core
    GTest::gtest
    GTest::gtest_main
  )
  gtest_discover_tests(vpgo-core-test)
  set_target_properties(vpgo-core-test PROPERTIES CXX_CPPCHECK "")
endif()

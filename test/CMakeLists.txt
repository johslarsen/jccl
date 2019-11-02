file(GLOB cutests "${CMAKE_CURRENT_SOURCE_DIR}/../*.c")
add_custom_command(
  OUTPUT AllTests.c
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../CuTest/make-tests.sh ${cutests} > AllTests.c
  DEPENDS ${cutests}
)
add_executable(AllTests
  AllTests.c
  ${cutests}
  ../CuTest/CuTest.c
)
target_link_libraries(AllTests -lm -lpthread)
add_test(all_tests AllTests)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

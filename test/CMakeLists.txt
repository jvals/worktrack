cmake_minimum_required (VERSION 3.14)

project (worktrack-test C)

# Find cmocka
FIND_LIBRARY(CMOCKA_LIBRARY cmocka)

# List of tests and their flags
LIST(APPEND tests_names "test_time_service")
LIST(APPEND tests_flags "-Wl,--wrap,safe_new_entry")

# Declare all tests targets
LIST(LENGTH tests_names count)
MATH(EXPR count "${count} - 1")
FOREACH(i RANGE ${count})
    LIST(GET tests_names ${i} test_name)
    LIST(GET tests_flags ${i} test_flags)
    ADD_EXECUTABLE(${test_name} ${test_name}.c)
    TARGET_LINK_LIBRARIES(
        ${test_name}
        ${CMOCKA_LIBRARY}
    )
    IF(test_flags STREQUAL " ")
    ELSE()
        TARGET_LINK_LIBRARIES(
            ${test_name}
            ${test_flags}
        )
    ENDIF()
    ADD_TEST(${test_name} ${test_name})
ENDFOREACH()


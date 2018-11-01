if (UNIX AND NOT APPLE)
    SET (ADDITIONAL_LIBS "rt")
else()
    SET (ADDITIONAL_LIBS "")
endif (UNIX AND NOT APPLE)


add_executable(c-sample sample.c)
target_link_libraries(c-sample sheap ${ADDITIONAL_LIBS})

add_executable(cpp-sample sample.cpp)
target_link_libraries(cpp-sample sheap ${ADDITIONAL_LIBS})


add_executable(c-measure measure.c)
target_link_libraries(c-measure sheap ${ADDITIONAL_LIBS})


add_executable(c-test test.c)
target_link_libraries(c-test sheap ${ADDITIONAL_LIBS})

add_executable(cpp-test test.cpp)
target_link_libraries(cpp-test sheap ${ADDITIONAL_LIBS})


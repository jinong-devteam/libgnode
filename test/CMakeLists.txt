
ENABLE_TESTING ()

INCLUDE_DIRECTORIES (../include ${CMAKE_CURRENT_BINARY_DIR}/../include)

ADD_EXECUTABLE (test_base test_base.cpp)
TARGET_LINK_LIBRARIES (test_base ${LIBS})

ADD_TEST (unittest1 test)


ENABLE_TESTING ()

INCLUDE_DIRECTORIES (../include ${CMAKE_CURRENT_BINARY_DIR}/../include)

ADD_EXECUTABLE (test_base test_base.cpp)
TARGET_LINK_LIBRARIES (test_base ${LIBS})

ADD_EXECUTABLE (test_node test_node.cpp)
TARGET_LINK_LIBRARIES (test_node ${LIBS})

ADD_TEST (unittest1 test_base)
ADD_TEST (unittest2 test_node)

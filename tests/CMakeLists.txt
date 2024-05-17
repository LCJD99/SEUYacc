set(HEADER_LIST "${PROJECT_SOURCE_DIR}/include/yacc.hpp")

add_executable(testlib testTable.cpp)

target_compile_features(testlib PRIVATE cxx_std_17)

add_test(NAME testlibtest COMMAND testlib) # Command can be a target

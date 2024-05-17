set(HEADER_LIST "${PROJECT_SOURCE_DIR}/include/yacc.hpp")


# We need this directory, and users of our library will need it too
add_executable(seuyacc yacc.cpp)

target_compile_features(seuyacc PUBLIC cxx_std_17)

target_link_libraries(seuyacc PUBLIC fmt::fmt)

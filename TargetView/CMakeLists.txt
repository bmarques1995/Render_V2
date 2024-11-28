add_executable(TargetView "src/entry.cpp")
target_link_libraries(TargetView PUBLIC Render)

set_property(TARGET TargetView PROPERTY CXX_STANDARD 20)

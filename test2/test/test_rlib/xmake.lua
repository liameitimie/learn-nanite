target("test_result")
    add_files("test_result.cpp")
    add_deps("rlib")
target_end()

target("output_format")
    add_files("test.cpp")
    set_rundir(".")
target_end()
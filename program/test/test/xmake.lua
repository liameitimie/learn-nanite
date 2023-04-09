target("test")
    add_files("test.cpp")
    add_deps("rlib","util","mesh")
    set_rundir(".")
target_end()

add_requires("metis")

target("test1")
    add_files("test1.cpp")
    add_deps("mesh")
    add_packages("metis")
target_end()

target("test2")
    add_files("test2.cpp")
    add_deps("mesh")
    set_rundir(".")
target_end()
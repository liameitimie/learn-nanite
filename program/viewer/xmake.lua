add_requires("fmt")

target("viewer")
    add_files("*.cpp")
    add_deps("vk","vk_win","mesh")
    add_packages("fmt")
    set_rundir(".")
target_end()
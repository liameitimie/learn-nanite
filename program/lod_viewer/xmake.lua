add_requires("fmt","coost")

target("lod_viewer")
    add_files("*.cpp")
    add_deps("vk","vk_win","virtual_mesh")
    add_packages("fmt","coost")
    set_rundir(".")
target_end()
add_requires("fmt")

target("lod_viewer")
    add_files("*.cpp")
    add_deps("vk","vk_win","virtual_mesh")
    add_packages("fmt")
    set_rundir(".")
target_end()
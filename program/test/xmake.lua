add_requires("vulkansdk","glfw","glm")

target("engine")
    set_kind("static")
    add_files("engine.cpp")
    add_deps("vk","vk_win","mesh")
    add_packages("vulkansdk","glfw","glm")
target_end()

target("main")
    add_files("main.cpp")
    add_deps("engine","mesh")
    set_rundir(".")
target_end()
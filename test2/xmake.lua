set_languages("c++20")
add_rules("mode.debug", "mode.release")

add_requires("vulkansdk","glfw","glm")

includes("thirdparty")
includes("rlib")
-- includes("test")
includes("vk")

target("engine")
    set_kind("static")
    add_files("engine.cpp")
    add_deps("vk")
    add_packages("vulkansdk","glfw")
target_end()

target("main")
    add_files("main.cpp")
    add_deps("engine")
    set_rundir(".")
target_end()
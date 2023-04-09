add_requires("vulkansdk","glfw")

target("vk_win")
    set_kind("static")
    add_files("*.cpp")
    add_includedirs(".",{public=true})
    add_deps("util","vk")
    add_packages("vulkansdk","glfw")
target_end()
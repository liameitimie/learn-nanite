add_requires("tinyobjloader","metis","assimp")

target("mesh")
    set_kind("static")
    add_files("*.cpp")
    add_deps("util","vk")
    add_packages("tinyobjloader","metis","assimp")
    add_includedirs(".",{public=true})
target_end()
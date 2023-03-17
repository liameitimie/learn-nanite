add_requires("tinyobjloader")

target("mesh")
    set_kind("static")
    add_files("*.cpp")
    add_deps("vk","util")
    add_packages("tinyobjloader")
    add_includedirs(".",{public=true})
target_end()
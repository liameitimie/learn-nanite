add_requires("metis","fmt")

target("virtual_mesh")
    set_kind("static")
    add_files("*.cpp")
    add_deps("mesh_simplify")
    add_packages("metis","fmt")
    add_includedirs(".",{public=true})
target_end()
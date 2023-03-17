add_requires("vulkansdk")

target("vk-bootstrap")
    set_kind("static")
    add_files("src/VkBootstrap.cpp")
    add_includedirs("src",{public=true})
    add_packages("vulkansdk")
target_end()

set_languages("c++20")
add_rules("mode.debug", "mode.release")
add_requires("vulkansdk","glfw","glm")

includes("thirdparty")

target("learn_nanite")
    add_files("*.cpp")
    add_packages("vulkansdk","glfw")
    add_deps("nlohmann_json")
    set_rundir(".")
target_end()
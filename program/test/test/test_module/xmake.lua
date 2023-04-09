add_rules("mode.release", "mode.debug")
set_languages("c++20")

target("mod")
    set_kind("static")
    add_files("src/hello.mpp","src/hello.cpp")

target("hello")
    set_kind("binary")
    add_files("src/main.cpp")
    add_deps("mod")
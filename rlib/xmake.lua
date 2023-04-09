set_languages("c++20")

target("rlib")
    set_kind("headeronly")
    add_headerfiles("*.h")
    add_includedirs(".",{public=true})
target_end()
#pragma once

#include <iostream>
#include <type_traits>

namespace option_types{
    template<typename T>
    struct Some{
        Some(){}
        Some(const T& val): val(val){}

        T val;
    };

    struct None{};
}

template<typename T, typename CleanT=typename std::decay<T>::type>
option_types::Some<CleanT> Some(T&& val){
    return option_types::Some<CleanT>(std::forward<T>(val));
}

inline option_types::None None(){
    return option_types::None{};
}

template<typename T>
struct Option{

    Option(){
        this->some_=false;
    }

    Option(option_types::Some<T> some){
        this->some_=true;
        this->value.val=some.val;
    }

    Option(option_types::None none){
        this->some_=false;
    }

    ~Option(){}

    bool is_some(){
        return some_;
    }

    bool is_none(){
        return !some_;
    }

    T expect(const char* msg){
        if (is_some()){
            return value.val;
        }
        std::fprintf(stderr,"%s\n",msg);
        std::terminate();
    }

    T unwrap(){
        if (is_some()){
            return value.val;
        }
        std::fprintf(stderr,"called `Option::unwrap()` on a `None` value\n");
        std::terminate();
    }

    T unwrap_or(const T& default_value){
        if (is_some()) {
            return value.val;
        }
        return default_value;
    }

private:
    option_types::Some<T> value;
    bool some_;
};
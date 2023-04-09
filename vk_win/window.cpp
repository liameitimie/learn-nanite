#include "window.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <vk_check.h>

namespace vk_win{

struct WindowContext{
    u32 key_state[26]={0};
    bool is_key_trans[26]={0};
};

Window Window::create(u32 width,u32 height,const char* name){
    Window window{};
    window.width=width;
    window.height=height;
    window.context=new WindowContext;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

    window.handle=(u64)glfwCreateWindow(width,height,name,nullptr,nullptr);
    return window;
}

void Window::build_vk_surface(u64 instance){
    VkSurfaceKHR surface;
    VK_CHECK(glfwCreateWindowSurface((VkInstance)instance,(GLFWwindow*)handle,nullptr,&surface));
    this->surface=(u64)surface;
}

bool Window::should_close(){
    return glfwWindowShouldClose((GLFWwindow*)handle);
}
void Window::poll_events(){
    glfwPollEvents();
    for(u32 i=0;i<26;i++){
        int state=glfwGetKey((GLFWwindow*)handle,'A'+i);
        context->is_key_trans[i]=context->key_state[i]!=state;
        context->key_state[i]=state;
    }
}

bool Window::is_key_down(char c){
    return context->key_state[c-'A']==GLFW_PRESS;
}

bool Window::is_key_begin_press(char c){
    return context->is_key_trans[c-'A']&&context->key_state[c-'A']==GLFW_PRESS;
}

bool Window::is_key_begin_release(char c){
    return context->is_key_trans[c-'A']&&context->key_state[c-'A']==GLFW_RELEASE;
}

dvec2 Window::get_cursor_pos(){
    dvec2 p;
    glfwGetCursorPos((GLFWwindow*)handle,&p.x,&p.y);
    return p;
}

void Window::set_cursor_disabled(){
    glfwSetInputMode((GLFWwindow*)handle,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
}

void Window::set_cursor_normal(){
    glfwSetInputMode((GLFWwindow*)handle,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
}

// bool window_should_close(){return glfwWindowShouldClose(window());}
// void poll_events(){glfwPollEvents();}
}
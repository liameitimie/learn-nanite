#include "pass.h"
#include "render_graph.h"

int main(){
    RenderGraph render_graph;
    render_graph.init();
    render_graph.from_json_file("test.json");

    while(!glfwWindowShouldClose(render_graph.window())){
        glfwPollEvents();
        render_graph.render();
    }
    render_graph.clear();
    return 0;
}
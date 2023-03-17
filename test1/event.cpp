#include "event.h"
#include "pass.h"
#include "render_graph.h"
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>

using namespace std;

Event* Event::create(RenderGraph& rdg,const json& event_desc){
    Event* event=new Event;

    string event_type=event_desc["type"].get<string>();
    if(event_type=="pass"){
        event->type=Type::pass;
    }
    else{
        cout<<"error event type "<<event_type<<endl;
        exit(0);
    }

    if(event->type==Type::pass){
        string pass_name=event_desc["pass"];
        event->pass=rdg.get_pass(pass_name);
    }

    return event;
}

void Event::proc_event(RenderGraph& rdg,VkCommandBuffer cmd_buffer){
    if(type==Type::pass){
        pass->proc_pass(rdg,cmd_buffer);
    }
}
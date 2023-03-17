#include "engine.h"

int main(){
    Engine engine;
    engine.init();
    engine.run();
    engine.cleanup();
    
    return 0;
}
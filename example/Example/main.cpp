#include <iostream>
#include "App.h"

int main(int argc, char* argv[]) {
    App* app = new App();

    app->Init();
    while (!app->quit) {
        app->Update();
    }
    app->CleanUp();
    delete app;    

    return 0;
}
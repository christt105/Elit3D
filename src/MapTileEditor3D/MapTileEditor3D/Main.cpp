#include <iostream>

#include <stdio.h>
#include <string>
#include <list>

#include "ExternalTools/mmgr/mmgr.h"

#include "ExternalTools/SDL2/include/SDL.h"

#include "Logger.h"
#include "Application.h"
#include "Module.h"

#pragma comment(lib, "ExternalTools/SDL2/libx86/SDL2.lib")
#pragma comment(lib, "ExternalTools/SDL2/libx86/SDL2main.lib")

enum class MainState {
    EXIT_ERROR = -1,
    EXIT = 0,

    CREATION,
    INIT,
    START,
    UPDATE,
    FINISH
};

Application* App = nullptr;

int main(int argc, char* argv[]) {

    LOG("Starting Program...");

    MainState mainState = MainState::CREATION;
    bool running = true;
    while (running) {
        switch (mainState)
        {
        case MainState::CREATION:
            LOG("Application Creation");
            App = new Application();
            mainState = MainState::INIT;
            break;
        case MainState::INIT:
            LOG("Application Init");
            if (App->Init()) {
                mainState = MainState::START;
            }
            else {
                LOGE("Application Init() failed, exiting with error");
                mainState = MainState::EXIT_ERROR;
            }
            break;
        case MainState::START:
            LOG("Application Start");
            if (App->Start()) {
                mainState = MainState::UPDATE;
            }
            else {
                LOGE("Application Start() failed, exiting with error");
                mainState = MainState::EXIT_ERROR;
            }
            break;
        case MainState::UPDATE:
            switch (App->Update())
            {
            case UpdateStatus::UPDATE_STOP:
                mainState = MainState::FINISH;
                break;
            case UpdateStatus::UPDATE_ERROR:
                mainState = MainState::EXIT_ERROR;
                break;
            default:
                break;
            }
            break;
        case MainState::FINISH:
            LOG("Application CleanUp");
            if (App->CleanUp()) {
                mainState = MainState::EXIT;
            }
            else {
                LOGE("Application CleanUp() failed, exiting with error");
                mainState = MainState::EXIT_ERROR;
            }

            delete App;
            App = nullptr;

            break;
        case MainState::EXIT:
            //LOG file;
            LOG("Closing program...\nBye :)");
            running = false;
            break;
        case MainState::EXIT_ERROR:
            LOG("Exiting with errors :(");
            running = false;
            break;
        default:
            break;
        }
    }
    
    return (int)mainState;
}

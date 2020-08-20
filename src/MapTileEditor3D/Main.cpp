#include <iostream>

#include <stdio.h>
#include <string>
#include <list>
#include <SDL.h>

#include "Logger.h"
#include "Application.h"
#include "Profiler.h"
#include "Module.h"

//MathGeoLib--------------------------------------------------------
#include "ExternalTools/MathGeoLib/include/MathGeoLib.h"

#ifdef _DEBUG
#pragma comment (lib, "ExternalTools/MathGeoLib/lib_x86/lDebug/MathGeoLib.lib")
#else
#pragma comment (lib, "ExternalTools/MathGeoLib/lib_x86/lRelease/MathGeoLib.lib")
#endif
//--------------------------------------------------------------------

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

    int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(flag);

    MainState mainState = MainState::CREATION;
    bool running = true;
    while (running) {
        //PROFILE_SECTION("MAIN THREAD");
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

            break;
        case MainState::EXIT:
            Logger::ExportLog();
            LOG("Closing program... Bye :)");
            running = false;
            break;
        case MainState::EXIT_ERROR:
            Logger::ExportLog();
            LOG("Exiting with errors :(");
            running = false;
            break;
        default:
            break;
        }
    }

    delete App;
    App = nullptr;

    return (int)mainState;
}

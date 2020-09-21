#include <SDL.h>

#include "Logger.h"
#include "Application.h"
#include "Profiler.h"
#include "Module.h"

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

    LOGN("Starting Program...");

    int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(flag);

    MainState mainState = MainState::CREATION;
    bool running = true;
    while (running) {
        switch (mainState)
        {
        case MainState::CREATION:
            LOGN("Application Creation");
            App = new Application();
            mainState = MainState::INIT;
            break;
        case MainState::INIT:
            LOGN("Application Init");
            if (App->Init()) {
                mainState = MainState::START;
            }
            else {
                LOGNE("Application Init() failed, exiting with error");
                mainState = MainState::EXIT_ERROR;
            }
            break;
        case MainState::START:
            LOGN("Application Start");
            if (App->Start()) {
                mainState = MainState::UPDATE;
            }
            else {
                LOGNE("Application Start() failed, exiting with error");
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
            LOGN("Application CleanUp");
            if (App->CleanUp()) {
                mainState = MainState::EXIT;
            }
            else {
                LOGNE("Application CleanUp() failed, exiting with error");
                mainState = MainState::EXIT_ERROR;
            }

            break;
        case MainState::EXIT:
            Logger::ExportLog();
            LOGN("Closing program... Bye :)");
            running = false;
            break;
        case MainState::EXIT_ERROR:
            Logger::ExportLog();
            LOGN("Exiting with errors :(");
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

#pragma once
#include "Modules/Base/Module.h"

class Command;

class m1UndoRedo :
    public Module
{
public:
    m1UndoRedo();

    void AddCommand(Command* command);

    UpdateStatus Update() override;

    bool CleanUp() override;

private:
    std::list<Command*> commands;
};


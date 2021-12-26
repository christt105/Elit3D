#include "m1UndoRedo.h"
#include "Core/Application.h"
#include "Modules/m1Input.h"

#include "Tools/Command.h"

#include "Tools/System/Logger.h"

m1UndoRedo::m1UndoRedo() : Module("UndoRedo", true)
{
}

void m1UndoRedo::AddCommand(Command* command)
{
	if (iterator != commands.size() - 1 && iterator != -1)
	{
		for (int i = iterator + 1; i < commands.size(); ++i)
		{
			delete commands[i];
		}
		commands.erase(commands.begin() + iterator + 1, commands.end());
	}

	commands.push_back(command);

	iterator = commands.size() - 1;
}

UpdateStatus m1UndoRedo::Update()
{
	if (App->input->IsKeyPressed(SDL_SCANCODE_LCTRL) && App->input->IsKeyDown(SDL_SCANCODE_Z) && iterator >= 0 &&
		iterator < commands.size())
	{
		commands[iterator]->Undo();
		iterator += iterator > 0 ? -1 : 1;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_LCTRL) && App->input->IsKeyDown(SDL_SCANCODE_Y) && iterator >= 0 && iterator < commands.size())
	{
		commands[iterator]->Redo();
		iterator += iterator < commands.size() - 1 ? 1 : -1;
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1UndoRedo::CleanUp()
{
	for (auto& command : commands)
	{
		delete command;
	}

	return true;
}

#include <stdlib.h>

#include "Application.h"
#include "Globals.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/lib/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/lib/libx86/SDL2main.lib" )

#ifdef QUE_PASOO
//Brofiler implementation
#include "Brofiler/Brofiler.h"
#pragma comment(lib, "Brofiler/ProfilerCore32.lib")
#endif // QUE_PASOO

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = NULL;

int main(int argc, char ** argv)
{
	CONSOLE_DEBUG("Starting Engine");

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	while (state != MAIN_EXIT)
	{
		BROFILER_FRAME("TheCreator3D");

		switch (state)
		{
		case MAIN_CREATION:

			CONSOLE_DEBUG("-------------- Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			CONSOLE_DEBUG("-------------- Application Init --------------");
			if (App->Init() == false)
			{
				CONSOLE_DEBUG("Application Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				CONSOLE_DEBUG("-------------- Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				CONSOLE_DEBUG("Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			CONSOLE_DEBUG("-------------- Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				CONSOLE_DEBUG("Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	return main_return;
}
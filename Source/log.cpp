#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ConsoleWindow.h"
#include <stdio.h>

void log(const char file[], const char function[], int line, bool is_warning, bool is_error, bool is_debug, const char* format, ...)
{
	if (App != nullptr && !App->IsGame())
	{
		static char tmp_string[4096];
		static char tmp_string2[4096];
		static va_list  ap;

		// Construct the string from variable arguments
		va_start(ap, format);
		vsprintf_s(tmp_string, 4096, format, ap);
		va_end(ap);
		sprintf_s(tmp_string2, 4096, "At: %s => %s() => Line: %d. ""%s""\n", file, function, line, tmp_string);
		OutputDebugString(tmp_string2);

		sprintf_s(tmp_string2, 4096, "%s", tmp_string);
		if (App != nullptr && App->editor != nullptr && App->editor->console_window != nullptr)
		{
			if (is_error)
			{
				App->editor->console_window->AddLog(tmp_string2, true);
			}
			else if (is_warning)
			{
				App->editor->console_window->AddLog(tmp_string2, false, true);
			}
			else if (is_debug)
			{
				App->editor->console_window->AddLog(tmp_string2, false, false, true);
			}
			else
			{
				App->editor->console_window->AddLog(tmp_string2);
			}
		}
	}
}
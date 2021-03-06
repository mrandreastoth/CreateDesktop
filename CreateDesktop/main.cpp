//////////////////////////////////////////////////////////////////////
//
// main.cpp
//
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////

HDESK CreateHiddenDesktopW(const wchar_t *desktop_name)
{
	wchar_t explorer_path[MAX_PATH];
	HDESK original_desktop;
	HDESK hidden_desktop = NULL;
	STARTUPINFOW startup_info = { 0 };
	PROCESS_INFORMATION process_info = { 0 };

	ExpandEnvironmentStringsW(L"%windir%\\explorer.exe", explorer_path, MAX_PATH - 1);

	hidden_desktop = OpenDesktopW(desktop_name, NULL, FALSE, GENERIC_ALL);

	if (!hidden_desktop)
	{
		hidden_desktop = CreateDesktopW(desktop_name, NULL, NULL, 0, GENERIC_ALL, NULL);

		if (hidden_desktop)
		{
			original_desktop = GetThreadDesktop(GetCurrentThreadId());

			if (SetThreadDesktop(hidden_desktop))
			{
				startup_info.cb = sizeof(startup_info);
				startup_info.lpDesktop = LPWSTR(desktop_name);

				// Create explorer.exe in the context of the new desktop for start menu, etc.
				CreateProcessW(explorer_path, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startup_info, &process_info);

				SetThreadDesktop(original_desktop);
			}
		}
	}

	return hidden_desktop;
}

///////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	HDESK original_desktop;
	HDESK hidden_desktop = NULL;

	hidden_desktop = CreateHiddenDesktopW(L"HiddenDesktop");
	original_desktop = GetThreadDesktop(GetCurrentThreadId());

	//wprintf(L"Entering hidden desktop\n");

	// Switch thread into context of new desktop to register hotkey
	SetThreadDesktop(hidden_desktop);
	SwitchDesktop(hidden_desktop);

	if (RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, int('E')))
	{
		MSG msg = { 0 };

		while (GetMessageW(&msg, NULL, 0, 0) != 0)
		{
			if (msg.message == WM_HOTKEY)
			{
				//wprintf(L"Exiting hidden desktop\n");
				SwitchDesktop(original_desktop);
				break; // ==>
			}
		}
	}

	system("pause");
}
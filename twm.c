#include <windows.h>
#include <winuser.h>
#include <stdio.h>

#include <psapi.h>

#include "tools.h"
#include "windows_functions.h"
#include "monitor_functions.h"
#include "hotkey_functions.h"
#include "windows_manager.h"
#include "layout_manager.h"
#include "regexp.h"

void hotkey_quit(void)
{
	exit(1);
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	// Reading in all windows
	EnumWindows(enumWindowsProc, 0);

	// Get all the monitors
	if (Monitor_Setup())
		return 1;

	// get the taskbar
	if (Window_Get_Taskbar())
		return 1;

	// Initialize Layouts
	if (Layout_Init())
		return 1;

	// Initialize Window Manager
	if (Windows_Manager_Init())
		return 1;

	HotKey_Register("win Q", &hotkey_quit);
	HotKey_Register("win T", &hotkey_taskbar);
	HotKey_Register("win C", &hotkey_code_mode);
	HotKey_Register("win W", &hotkey_list_windows);

	fflush(stdout);

	MSG msg = {0};

	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (msg.message == WM_HOTKEY)
		{
			HotKey_Call(LOWORD(msg.lParam), HIWORD(msg.lParam));
		}
		
		TranslateMessage(&msg); 
		DispatchMessage(&msg);
	}

	return 1;
}

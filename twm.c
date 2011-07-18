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

void hotkey_previous_window(void)
{
	extern struct layout *current_layout;
	Layout_Section_Previous_Window(current_layout);
}

void hotkey_next_window(void)
{
	extern struct layout *current_layout;
	Layout_Section_Next_Window(current_layout);
}

void hotkey_next_section(void)
{
	extern struct layout *current_layout;
	Layout_Section_Next_Section(current_layout);
}

void hotkey_previous_section(void)
{
	extern struct layout *current_layout;
	Layout_Section_Previous_Section(current_layout);
}

void hotkey_section_setup(void)
{
	extern struct layout *current_layout;
	Layout_Section_Next_Setup(current_layout);
}

void hotkey_next_layout(void)
{
	extern struct layout *current_layout;
	if (current_layout->name)
		printf("layout: %s\n", current_layout->name);
	Layout_Next_Layout();
	fflush(stdout);
}

void hotkey_prev_layout(void)
{
	extern struct layout *current_layout;
	if (current_layout->name)
		printf("layout: %s\n", current_layout->name);
	Layout_Previous_Layout();
	fflush(stdout);
}


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{

	// Get all the monitors
	if (Monitor_Setup())
		return 1;

	// Initialize Layouts
	if (Layout_Init())
		return 1;

	// Initialize Window Manager
	if (Windows_Manager_Init())
		return 1;

	// get the taskbar
	if (Window_Get_Taskbar())
		return 1;



	HotKey_Register("win Q", &hotkey_quit, "quit");
	HotKey_Register("win T", &hotkey_taskbar, "hide taskbar");
	HotKey_Register("ctrl win X", &Window_Add_Active, "add active window");
	HotKey_Register("ctrl win M", &hotkey_windows_manage, "manage windows");
	HotKey_Register("ctrl win H", &hotkey_previous_window, "previus window");
	HotKey_Register("ctrl win J", &hotkey_previous_section, "previous section");
	HotKey_Register("ctrl win U", &hotkey_section_setup, "section setup");
	HotKey_Register("ctrl win K", &hotkey_next_section, "next section");
	HotKey_Register("ctrl win L", &hotkey_next_window, "next window");
	HotKey_Register("ctrl win I", &hotkey_next_layout, "next layout");
	HotKey_Register("ctrl win O", &hotkey_prev_layout, "previous layout");

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

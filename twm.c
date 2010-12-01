#include <windows.h>
#include <winuser.h>
#include <stdio.h>

#include <psapi.h>

#include "tools.h"

struct window
{
	HWND handle;
	DWORD process_id;
	DWORD original_dwStyle;
	DWORD original_dwExStyle;
	char *class_name;
	char *window_name;
	
	struct window *next, *prev;
};

struct window *windows_list, *windows_list_last;

struct window *taskbar;

struct window *add_window(void)
{
	if (windows_list == NULL)
	{
		windows_list = calloc(1, sizeof(struct window));
		windows_list_last = windows_list;
		return windows_list;
	}

	windows_list_last->next = calloc(1, sizeof(struct window));

	if (windows_list_last->next == NULL)
		return NULL;

	windows_list_last->next->prev = windows_list_last;

	windows_list_last = windows_list_last->next;

	return windows_list_last;
}

struct window *find_window_by_class(char *class)
{
	struct window *w;

	w = windows_list;

	while (w)
	{
		if (strstr(class, w->class_name))
			return w;
		w = w->next;
	}

	return NULL;
}

void setup_window(struct window *w, HWND hwnd, WINDOWINFO *winfo)
{
	char buf[1024];
	
	if (w == NULL)
		return;

	w->handle = hwnd;
	GetWindowThreadProcessId(hwnd, &w->process_id);
	w->original_dwExStyle = winfo->dwExStyle;
	w->original_dwStyle = winfo->dwStyle;

	if (RealGetWindowClass(hwnd, buf, 1024))
		w->class_name = strdup(buf);
	else
		w->class_name = NULL;

	if (GetWindowText(hwnd, buf, 1024))
		w->window_name = strdup(buf);
	else
		w->window_name = NULL;
}

BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lparam)
{
	struct window *w;

	WINDOWINFO info;

	GetWindowInfo(hwnd, &info);

	if (!(info.dwStyle & WS_VISIBLE))
		return 1;

	w = add_window();

	if (w == NULL)
	{
		printf("error allocating window\n");
		return 0;
	}

	setup_window(w, hwnd, &info);
	return 1;
/*
	if (process_id)
	{
		if ((process_handle = OpenProcess(PROCESS_ALL_ACCESS ,FALSE , process_id)))
		{
			if (GetModuleFileNameEx(process_handle, NULL, buf, 512))
			{
				printf("process file name: %s\n", buf);
			}
		}
	}

	CloseHandle(process_handle);

	if (IsWindow(hwnd))
	{
		if (GetWindowText(hwnd, buf, 512))
			printf("name: \"%s\"\n", buf);
		if (RealGetWindowClass(hwnd, buf1, 512))
			printf("class: \"%s\"\n", buf1);
		printf("position: %i %i %i %i\n", info.rcWindow.top,  info.rcWindow.bottom, info.rcWindow.left, info.rcWindow.right);
	}

	printf("\n\n");
*/
	return 1;
}

void hotkey_quit(void)
{
	exit(1);
}

void hotkey_taskbar(void)
{
	static int taskbar_hidden = 0;

	if (taskbar_hidden)
		ShowWindow(taskbar->handle, SW_SHOW);
	else
		ShowWindow(taskbar->handle, SW_HIDE);

	taskbar_hidden = !taskbar_hidden;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	// Reading in all windows
	EnumWindows(enumWindowsProc, 0);

	// get the taskbar
	taskbar = find_window_by_class("Shell_TrayWnd");

	HotKey_Register("alt B", &hotkey_quit);
	HotKey_Register("alt T", &hotkey_taskbar);

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

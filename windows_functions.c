#include <windows.h>
#include <winuser.h>
#include <stdio.h>

#include <psapi.h>

#include "tools.h"
#include "windows_functions.h"

static struct window *taskbar;

static struct window *windows_list, *windows_list_last;

static struct window *window_add(void)
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

static struct window *window_find_by_class(char *class)
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

static void window_resetup(struct window *w)
{
	char buf[1024];
	WINDOWINFO info, *winfo;
	
	if (w == NULL)
		return;

	GetWindowInfo(w->handle, &info);
	winfo = &info;

	GetWindowThreadProcessId(w->handle, &w->process_id);
	w->original_dwExStyle = winfo->dwExStyle;
	w->original_dwStyle = winfo->dwStyle;

	if (RealGetWindowClass(w->handle, buf, 1024))
		w->class_name = strdup(buf);
	else
		w->class_name = NULL;

	if (GetWindowText(w->handle, buf, 1024))
		w->window_name = strdup(buf);
	else
		w->window_name = NULL;

	memcpy(&w->original_rcWindow, &winfo->rcWindow, sizeof(RECT));
	memcpy(&w->original_rcClient, &winfo->rcClient, sizeof(RECT));
}


void window_setup(struct window *w, HWND hwnd, WINDOWINFO *winfo)
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

	memcpy(&w->original_rcWindow, &winfo->rcWindow, sizeof(RECT));
	memcpy(&w->original_rcClient, &winfo->rcClient, sizeof(RECT));
}

static void window_restore(struct window *w)
{
	SetWindowLong(w->handle, GWL_STYLE, w->original_dwStyle);
	SetWindowLong(w->handle, GWL_EXSTYLE, w->original_dwExStyle);
}

BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lparam)
{
	struct window *w;

	WINDOWINFO info;

	GetWindowInfo(hwnd, &info);

	w = window_add();

	if (w == NULL)
	{
		printf("error allocating window\n");
		return 0;
	}

	window_setup(w, hwnd, &info);
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
	return 1;
*/
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

void hotkey_code_mode(void)
{
	static int code_toggle;
	static int code_toggle_initialized;
	static int count = 0;
	static struct window **code_windows;
	struct window *w;
	int i, j, x, y, width, height;

	if (code_toggle_initialized == 2)
		return;

	if (code_toggle_initialized == 0)
	{
		code_toggle_initialized = 1;
		
		w = windows_list;
		while(w)
		{
			if (strcmp("Xming X", w->class_name) == 0)
			{
				count++;
			}
			w = w->next;
		}

		code_windows = calloc(count + 1, sizeof(struct window *));

		if (code_windows == NULL)
		{
			code_toggle_initialized = 2;
			printf("code toggle failed\n");
			return;
		}
	
		w = windows_list;
		i = 0;
		while(w)
		{
			if (strcmp("Xming X", w->class_name) == 0)
				code_windows[i++] = w;
			w = w->next;
		}
	}

	if (code_windows[0] == NULL)
	{
		code_toggle_initialized = 2;
		printf("setting it up failed\n");
		return;
	}

	/*

	printf("------------ %i ------------\n", code_toggle);
	for (i=0;i<count;i++)
	{
		w = code_windows[i];
		printf("%s\n", w->window_name);
		get_size_from_rect(&w->original_rcWindow, &x, &y, &width, &height);
		printf("w: %i %i %i %i\n", x, y, width, height);
		get_size_from_rect(&w->original_rcClient, &x, &y, &width, &height);
		printf("c: %i %i %i %i\n", x, y, width, height);
	}
	printf("-----------------------------\n");
	fflush(stdout);
	*/

	if (code_toggle)
	{
		for (i=0;i<count;i++)
		{
			window_restore(code_windows[i]);
			get_size_from_rect(&code_windows[i]->original_rcWindow, &x, &y, &width, &height);
			MoveWindow(code_windows[i]->handle, x, y, width ,height, 1);
			ShowWindow(code_windows[i]->handle, SW_SHOW);
			UpdateWindow(code_windows[i]->handle);
		}
	}
	else
	{
			j = 0;
		for (i=0;i<count;i++)
		{
			SetWindowLong(code_windows[i]->handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			SetWindowLong(code_windows[i]->handle, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
		//	get_size_from_rect(&code_windows[i]->original_rcClient, &x, &y, &width, &height);
		//	MoveWindow(code_windows[i]->handle, x, y, width ,height, 1);
			if (strstr(code_windows[i]->window_name, "GVIM"))
			{
				MoveWindow(code_windows[i]->handle, 640, 0, 1280, 1200, 1);
			}
			else
			{
				MoveWindow(code_windows[i]->handle, 0, j++*600, 640, 600, 1);
			}
			ShowWindow(code_windows[i]->handle, SW_SHOW);
			UpdateWindow(code_windows[i]->handle);
		}
		fflush(stdout);
	}

	code_toggle = !code_toggle;
}

void hotkey_list_windows(void)
{
	struct window *w;

	w = windows_list;

	while (w)
	{
		printf("name : %s\n", w->window_name);
		printf("class: %s\n", w->class_name);
		printf("\n\n");
		w = w->next;
	}
}

int Window_Get_Taskbar(void)
{
	taskbar = window_find_by_class("Shell_TrayWnd");
	
	if (taskbar == NULL)
	{
		printf("no taskbar...\n");
		return 1;
	}

	return 0;
}

struct window *Window_Get_First(void)
{
	return windows_list;
}



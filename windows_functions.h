struct window
{
	HWND handle;
	DWORD process_id;
	DWORD original_dwStyle;
	DWORD original_dwExStyle;
	RECT original_rcWindow;
	RECT original_rcClient;
	char *class_name;
	char *window_name;
	
	struct window *next, *prev;
};

BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lparam);
void hotkey_taskbar(void);
void hotkey_code_mode(void);
void hotkey_list_windows(void);
int Window_Get_Taskbar(void);
struct window *Window_Get_First(void);
int Window_Readin_Windows(void);
int Window_Restore_Original(struct window *w);
int Window_Set_Style(struct window *w, DWORD style);
int Window_Set_ExStyle(struct window *w, DWORD style);
int Window_Set_Dimensions(struct window *w, int x, int y, int width, int height, int redraw);

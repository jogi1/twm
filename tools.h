int get_time (char *time_string, int time_string_size);
int HotKey_Register(char *hotkey_combination, void (*function)(void), char *function_name);
void HotKey_Call( WORD modifiers, WORD key);
void get_size_from_rect(RECT* rect, int *x, int *y, int *width, int *height);

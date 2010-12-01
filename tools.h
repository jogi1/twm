int get_time (char *time_string, int time_string_size);
int HotKey_Register(char *hotkey_combination, void (*function)(void));
void HotKey_Call( WORD modifiers, WORD key);

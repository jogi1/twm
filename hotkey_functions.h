int HotKey_Register(char *hotkey_combination, void (*function)(void), char *function_name);
void HotKey_Call(WORD modifiers, WORD key);

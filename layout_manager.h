struct section
{
	float fx, fy, fw, fh;
	int ix, iy, iw, ih;
	char *name;
	int windows_attached_count;
	struct window **windows_attached;
	struct window *active_window;
};

struct layout
{
	char *name;
	int monitor;
	int section_count;
	struct section **section;
	
	struct layout *next, *prev;
};

int Layout_Section_Set_Dimension_Float(struct section *s, float x, float y, float w, float h);
int Layout_Section_Calculate_Dimensions(struct section *s, struct layout *l);
struct section *Layout_Add_Section(struct layout *l, char *name);
int Layout_Set_Monitor(struct layout *l, int monitor);
struct layout *Layout_Create(char *name);
int Layout_Init(void);
int Layout_Section_Add_Window(struct section *s, struct window *w);

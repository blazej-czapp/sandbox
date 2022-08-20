#ifndef CONSOLE
#define CONSOLE

static char WHITE_ON_BLACK = 0x0f;
static char EDISON = 0x03;

void print_at(const char *text, int row, int col, char color, int update_cursor);
void clear_screen();
void move_cursor(int row, int col);

#endif
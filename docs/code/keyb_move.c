static unsigned char keycode_map[] = {
    KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8,
        KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_DELETE,
    KEY_GRAVE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8,
        KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE,
    KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O,
        KEY_P, KEY_LEFTBRACE, KEY_RIGHTBRACE, KEY_BACKSLASH,
    KEY_CAPSLOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K,
        KEY_L, KEY_SEMICOLON, KEY_APOSTROPHE, KEY_ENTER,
    KEY_LEFTSHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M,
        KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_RIGHTSHIFT,
    KEY_LEFTCTRL, 0, KEY_META, KEY_LEFTALT, KEY_SPACE, KEY_RIGHTALT,
        KEY_RIGHTCTRL, KEY_LEFT, KEY_UP, KEY_DOWN, KEY_RIGHT,
};

static int keycode_row_len[] = { 14, 14, 14, 13, 12, 11 };
static int keycode_row_len_acc[] = { 14, 28, 42, 55, 67, 78 };

#define KEYCODE_ROWS (sizeof(keycode_row_len) / sizeof(keycode_row_len[0]))
#define KEYCODE_COLS(row) (keycode_row_len[row])

unsigned char move_keyboard_cursor(unsigned int *row, unsigned int *col,
    int d_row, int d_col)
{
    unsigned int new_row;
    unsigned int new_col;
    unsigned int index;

    if (d_row == 0 && d_col == 0)
    {
        index = *row == 0 ? *col : keycode_row_len_acc[*row - 1] + *col;
        return keycode_map[index];
    }

    new_row = ((int)*row + d_row + KEYCODE_ROWS) % KEYCODE_ROWS;
    *row = new_row;
    
    new_col = ((int)*col + d_col + KEYCODE_COLS(*row)) % KEYCODE_COLS(*row);
    *col = new_col;

    index = *row == 0 ? *col : keycode_row_len_acc[*row - 1] + *col;
    if (index > ARRAY_SIZE(keycode_map))
    {
        printk(MOD_PREFIX "invalid keycode index: %u\n", index);
        printk(MOD_PREFIX "given row=%u col=%u\n", *row, *col);
        return keycode_map[0];
    }

    return keycode_map[index];
}
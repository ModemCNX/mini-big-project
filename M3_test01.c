#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
// ----------------------------
// Globals (game/system state)
// ----------------------------
static volatile int key_w = 0, key_a = 0, key_s = 0, key_d = 0, mouse_l = 0;

static long long tick_count = 0;      // frames since start
static time_t     start_time = 0;     // wall-clock at start
static int        game_time_s = 0;    // seconds since start (integer)
static int        fps = 0;            // frames per second (last second)
static int        fps_time_s = 0;     // last second we computed fps
static long long  last_sec_tick = 0;  // tick at last fps sample

static int term_cols = 0, term_rows = 0;  // console visible window size (columns, rows)

static int mouse_x = 0;  // screen coords (1-based for display only)
static int mouse_y = 0;
static int screen_w = 0; // physical screen size (pixels)
static int screen_h = 0;

static int game_state = 0; // unused for now
static int pos[2] = {3, 6};            // x,y (1-based console coordinates)
static int pos_last[2] = {3, 6};

// ----------------------------
// Helpers
// ----------------------------
static void enable_vt_sequences(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode)) return;
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; // enable ANSI/VT sequences
    SetConsoleMode(hOut, mode);
}

static void get_console_size(void) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hOut, &info)) {
        // visible window size
        term_cols = info.srWindow.Right - info.srWindow.Left + 1;
        term_rows = info.srWindow.Bottom - info.srWindow.Top + 1;
        if (term_cols < 1) term_cols = 1;
        if (term_rows < 1) term_rows = 1;
    }
}

static void clear_screen(void) {
    // Clear screen and move cursor to 1,1 (home)
    printf("\x1b[2J\x1b[H");
}

static void move_cursor(int y, int x) {
    if (y < 1) y = 1; if (x < 1) x = 1;
    printf("\x1b[%d;%dH", y, x);
}

static void draw_char(int y, int x, char ch) {
    move_cursor(y, x);
    putchar(ch);
}

// ----------------------------
// Init/setup
// ----------------------------
static void start_setup(void) {
    enable_vt_sequences();
    start_time = time(NULL);

    screen_w = GetSystemMetrics(SM_CXSCREEN);
    screen_h = GetSystemMetrics(SM_CYSCREEN);

    get_console_size();

    // Initialise last position for diff-drawing
    pos_last[0] = pos[0];
    pos_last[1] = pos[1];
}

// ----------------------------
// Updates
// ----------------------------
static void update_input(void) {
    // High-order bit indicates key is down
    key_w = (GetAsyncKeyState('W') & 0x8000) ? 1 : 0;
    key_a = (GetAsyncKeyState('A') & 0x8000) ? 1 : 0;
    key_s = (GetAsyncKeyState('S') & 0x8000) ? 1 : 0;
    key_d = (GetAsyncKeyState('D') & 0x8000) ? 1 : 0;
    mouse_l = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
}

static void update_mouse_position_in_screen(void) {
    POINT p;
    if (GetCursorPos(&p)) {
        // +1 only for cosmetic parity with original code's display
        mouse_x = p.x + 1;
        mouse_y = p.y + 1;
    }
}

static void update_tick(void) {
    tick_count++;
}

static void update_time_and_fps(void) {
    time_t now = time(NULL);
    game_time_s = (int)(now - start_time);

    if (game_time_s > fps_time_s) {
        int elapsed_s = game_time_s - fps_time_s;
        if (elapsed_s <= 0) elapsed_s = 1;
        long long delta_ticks = tick_count - last_sec_tick;
        fps = (int)(delta_ticks / elapsed_s);
        fps_time_s = game_time_s;
        last_sec_tick = tick_count;
    }
}

static void update_console_metrics(void) {
    get_console_size();
}

static void move_player(void) {
    // HUD takes first 5 rows, clamp playfield below
    const int min_x = 1;
    const int min_y = 6; // start drawing play area under HUD
    int max_x = term_cols; if (max_x < 1) max_x = 1;
    int max_y = term_rows; if (max_y < min_y) max_y = min_y;

    if (key_w) pos[1] -= 1;
    if (key_s) pos[1] += 1;
    if (key_a) pos[0] -= 1;
    if (key_d) pos[0] += 1;

    if (pos[0] < min_x) pos[0] = min_x;
    if (pos[1] < min_y) pos[1] = min_y;
    if (pos[0] > max_x) pos[0] = max_x;
    if (pos[1] > max_y) pos[1] = max_y;
}

// ----------------------------
// Enemy (Z)
// ----------------------------
#define MAX_Z 50   // maximum enemies on screen

typedef struct {
    int x, y;
    int active;
    long long last_move_tick;
} Enemy;

static Enemy enemies[MAX_Z];
static long long last_spawn_tick = 0;
static int game_over = 0;

// Spawn a new Z at the far right, random Y
static void spawn_z(void) {
	int i;
    int min_y = 6;
    int max_y = term_rows;
    if (max_y < min_y) max_y = min_y;

    // Find an inactive slot
    for (i = 0; i < MAX_Z; i++) {
        if (!enemies[i].active) {
            enemies[i].x = term_cols;
            enemies[i].y = (rand() % (max_y - min_y + 1)) + min_y;
            enemies[i].active = 1;
            enemies[i].last_move_tick = tick_count;
            break;
        }
    }
}

// Update all Z enemies
static void update_z(void) {
	int i;
    // Spawn new Z every 1 second (?60 ticks)
    if (tick_count - last_spawn_tick >= 30) {
        spawn_z();
        last_spawn_tick = tick_count;
    }

    // Move active enemies
    for (i = 0; i < MAX_Z; i++) {
        if (enemies[i].active) {
            if (tick_count - enemies[i].last_move_tick >= 1) { // move every 0.5s
                enemies[i].x--;
                enemies[i].last_move_tick = tick_count;
            }

            // If off screen  deactivate
            if (enemies[i].x <= 0) {
                enemies[i].active = 0;
            }
        }
    }
}

// ----------------------------
// Enemy (V)
// ----------------------------
#define MAX_V 50   // maximum V enemies on screen

typedef struct {
    int x, y;
    int active;
    long long last_move_tick;
} EnemyV;

static EnemyV venemies[MAX_V];
static long long last_v_spawn_tick = 0;

// Spawn a new V at the top, random X
static void spawn_v(void) {
    int min_x = 1;
    int max_x = term_cols;
    if (max_x < min_x) max_x = min_x;
	int i;
    for (i = 0; i < MAX_V; i++) {
        if (!venemies[i].active) {
            venemies[i].x = (rand() % (max_x - min_x + 1)) + min_x;
            venemies[i].y = 6; // spawn just below HUD
            venemies[i].active = 1;
            venemies[i].last_move_tick = tick_count;
            break;
        }
    }
}

// Update all V enemies
static void update_v(void) {
    // Spawn new V every 1 second (?60 ticks)
    if (tick_count - last_v_spawn_tick >= 60) {
        spawn_v();
        last_v_spawn_tick = tick_count;
    }
	int i;
    for (i = 0; i < MAX_V; i++) {
        if (venemies[i].active) {
            if (tick_count - venemies[i].last_move_tick >= 5) { // move every 0.5s
                venemies[i].y++;
                venemies[i].last_move_tick = tick_count;
            }

            // If off screen bottom  deactivate
            if (venemies[i].y > term_rows) {
                venemies[i].active = 0;
            }
        }
    }
}
static void check_collision(void) {
	int i;
    for (i = 0; i < MAX_Z; i++) {
        if (enemies[i].active && pos[0] == enemies[i].x && pos[1] == enemies[i].y) {
            game_over = 1;
        }
    }
    int c;
    for (c = 0; c < MAX_V; c++) {
        if (venemies[c].active && pos[0] == venemies[c].x && pos[1] == venemies[c].y) {
            game_over = 1;
        }
    }
}
// ----------------------------
// Render
// ----------------------------
static void render(void) {
    clear_screen();

    move_cursor(1, 1);
    printf("[ terminal size : %d cols  %d rows ]", term_cols, term_rows);

    move_cursor(2, 1);
    printf("[ game_time: %d s  tick: %lld  fps: %d ]", game_time_s, tick_count, fps);

    move_cursor(3, 1);
    printf("[ screen size : %d x %d  mouse: %d %d ]", screen_w, screen_h, mouse_x, mouse_y);

    move_cursor(4, 1);
    printf("[ input W %d  A %d  S %d  D %d  M1 %d ]", key_w, key_a, key_s, key_d, mouse_l);

    // Draw player '@' (erase last, draw new)
    if (pos_last[0] != pos[0] || pos_last[1] != pos[1]) {
        draw_char(pos_last[1], pos_last[0], ' ');
    }
    draw_char(pos[1], pos[0], '@');

    // Update last position after drawing
    pos_last[0] = pos[0];
    pos_last[1] = pos[1];
    int i;
    int siuu;
    // Draw all active Z enemies
    for (i = 0; i < MAX_Z; i++) {
        if (enemies[i].active) {
            draw_char(enemies[i].y, enemies[i].x, 'Z');
        }
    }

    // Draw all active V enemies
    for (siuu = 0; siuu < MAX_V; siuu++) {
        if (venemies[siuu].active) {
            draw_char(venemies[siuu].y, venemies[siuu].x, 'V');
        }
    }

    fflush(stdout);

    // Game Over message
    if (game_over) {
        move_cursor(term_rows / 2, (term_cols / 2) - 5);
        printf("GAME OVER");
        fflush(stdout);
        Sleep(3000);
        exit(0);
    }
}


// ----------------------------
// Main loop
// ----------------------------
int main(void) {
    start_setup();
    srand((unsigned)time(NULL)); // seed random

    printf("Press any key to start...\n");
    _getch();

   
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    if (GetConsoleCursorInfo(hOut, &ci)) { ci.bVisible = FALSE; SetConsoleCursorInfo(hOut, &ci); }

    spawn_z(); 
    spawn_v(); 

    for (;;) {
        update_console_metrics();
        update_tick();
        update_time_and_fps();
        update_mouse_position_in_screen();
        update_input();
        move_player();
        update_z();  
		update_v();         
        check_collision();    
        render();

        Sleep(16); // ~60 FPS
    }

    return 0;
}


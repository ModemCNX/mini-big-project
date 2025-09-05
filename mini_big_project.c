#include <stdio.h>
#include <windows.h>
#include <string.h>

//variable that NOT gonna change by player ( system variable )
char version[] = "1.0.1";

int w=0,a=0,s=0,d=0,m1=0; // user input flag

long int tick = 0;
LARGE_INTEGER frequency; // for game_time calculation
LARGE_INTEGER t1, t2; // for game_time calculation
float game_time = 0;
float delta_time = 0;
float fps = 0;
float fps_time = 0; // for fps calculation
int last_sec_tick = 0; // for fps calculation

float mode1_move_speed = 30;

int screen_change = 0;
char screen_instruction_1[100000] = "";
char screen_instruction_2[100000] = "";

//variable that gonna change ( game variable )
int game_state = 1;
char screen[120][30];
float position_x = 0;
float position_y = 0;

//sprite
char map[30][1200];
char player[10][10];

// Message storage variables
char stored_message[500] = "";

// Type 2: Dialog messages array
char dialog_messages[10][200];  // Store up to 10 messages
int message_count = 0;

// Type 3: Categorized messages
char game_messages[50][100];    // Game messages
char system_messages[20][150];  // System messages  
char interaction_messages[30][200]; // Interaction messages

// Type 4: Struct for messages with metadata
typedef struct {
    char content[300];
    char category[50];
    int is_used;
    float timestamp;
} GameMessage;

GameMessage stored_game_messages[100];
int total_messages = 0;

// Functions for message management
void store_simple_message(char* message) {
    strcpy(stored_message, message);
}

void add_dialog_message(char* message) {
    if (message_count < 10) {
        strcpy(dialog_messages[message_count], message);
        message_count++;
    }
}

void store_categorized_message(char* message, char* category) {
    if (total_messages < 100) {
        strcpy(stored_game_messages[total_messages].content, message);
        strcpy(stored_game_messages[total_messages].category, category);
        stored_game_messages[total_messages].is_used = 0;
        stored_game_messages[total_messages].timestamp = game_time;
        total_messages++;
    }
}

// Get stored messages
char* get_stored_message() {
    return stored_message;
}

char* get_dialog_message(int index) {
    if (index < message_count) {
        return dialog_messages[index];
    }
    return "";
}

void initialize_messages() {
    // Store initial messages
    store_simple_message("SYBAU");
    
    add_dialog_message("Hello there!");
    add_dialog_message("How are you today?");
    add_dialog_message("Nice weather, isn't it?");
    
    store_categorized_message("Player moved to new area", "movement");
    store_categorized_message("System initialized", "system");
    store_categorized_message("Ready for interaction", "interaction");
}

void make_sprite(){
	strcpy(map[0]  , "[......................................................................................................................]");
	strcpy(map[1]  , "[......................................................................................................................]");
	strcpy(map[2]  , "[......................................................................................................................]");
	strcpy(map[3]  , "[......................................................................................................................]");
	strcpy(map[4]  , "[......................................................................................................................]");
	strcpy(map[5]  , "[......................................................................................................................]");
	strcpy(map[6]  , "[......................................................................................................................]");
	strcpy(map[7]  , "[......................................................................................................................]");
	strcpy(map[8]  , "[......................................................................................................................]");
	strcpy(map[9]  , "[......................................................................................................................]");
	strcpy(map[10] , "[......................................................................................................................]");
	strcpy(map[11] , "[......................................................................................................................]");
	strcpy(map[12] , "[......................................................................................................................]");
	strcpy(map[13] , "[......................................................................................................................]");
	strcpy(map[14] , "[......................................................................................................................]");
	strcpy(map[15] , "[......................................................................................................................]");
	strcpy(map[16] , "[......................................................................................................................]");
	strcpy(map[17] , "[......................................................................................................................]");
	strcpy(map[18] , "[......................................................................................................................]");
	strcpy(map[19] , "[......................................................................................................................]");
	strcpy(map[20] , "[......................................................................................................................]");
	strcpy(map[21] , "[......................................................................................................................]");
	strcpy(map[22] , "[......................................................................................................................]");
	strcpy(map[23] , "[......................................................................................................................]");
	strcpy(map[24] , "[......................................................................................................................]");
	strcpy(map[25] , "[......................................................................................................................]");
	strcpy(map[26] , "[......................................................................................................................]");
	strcpy(map[27] , "[......................................................................................................................]");
	strcpy(map[28] , "[......................................................................................................................]");
	strcpy(map[29] , "[......................................................................................................................]");
}

// display function
void screen_draw(){
	int i;
	for(i=0;i<30;i++){
		char str[100];
		sprintf(str,"\e[%d;1H",i+1);
		strcat(screen_instruction_2,str);// set cursor position (y,x)
		strcat(screen_instruction_2,map[i]);
	}
	screen_change = 1;
}

void add_screen_instruction_1(){
	// Example: use stored messages here
	// strcat(screen_instruction_1, get_stored_message());
}

void add_screen_instruction_2(){
	// Example: use dialog messages here
	// strcat(screen_instruction_2, get_dialog_message(0));
}

// game function
void mode1_move(){
	if (w) position_y-=mode1_move_speed*delta_time;
	if (s) position_y+=mode1_move_speed*delta_time;
	if (a) position_x-=mode1_move_speed*delta_time;
	if (d) position_x+=mode1_move_speed*delta_time;
	
	if (position_x < 1) position_x = 1; // prevent negative position
	if (position_y < 1) position_y = 1;
	if (position_x > 120) position_x = 120; // prevent more position than screen
	if (position_y > 30) position_y = 30;
}

void physics_process(){
	mode1_move();
}

// start function
void start_setup(){
	printf("\e[?25l"); // hide cursor
	make_sprite();
	initialize_messages(); // FIXED: Add this line!
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
}

// update function
void update_input(){
	w = GetAsyncKeyState('W')!=0?1:0;
	a = GetAsyncKeyState('A')!=0?1:0;
	s = GetAsyncKeyState('S')!=0?1:0;
	d = GetAsyncKeyState('D')!=0?1:0;
	m1 = GetAsyncKeyState(0x01)!=0?1:0;
}

void update_game_tick(){
	tick ++;
}

void update_game_time(){
	QueryPerformanceCounter(&t2);
	delta_time = ((t2.QuadPart - t1.QuadPart) / (float)frequency.QuadPart)  - game_time;
	game_time = (t2.QuadPart - t1.QuadPart) / (float)frequency.QuadPart;
}

void update_fps(){  // fps of last frame
	if (game_time > fps_time){
		fps = ((float)tick - (float)last_sec_tick) / (game_time - fps_time);
		fps_time = game_time;
		last_sec_tick = tick;
	}
}

void update_screen(){
	if(screen_change == 0)return;
	printf("%s",screen_instruction_1);
	printf("%s",screen_instruction_2);
	strcpy(screen_instruction_1 , "");
	strcpy(screen_instruction_2 , "");
	screen_change = 0;
}

void update_game(){
	update_game_tick();
	update_game_time();
	update_fps();
	update_input();
	update_screen();
}

void display_var(){
	printf("\e[%d;%dH",1,1); // set cursor position (y,x)
	printf("[ version %s]",version);
	printf("\e[%d;%dH",2,1); // set cursor position (y,x)
	printf("[ game_time: %f delta_time: %f  tick: %d  fps: %f ]",game_time,delta_time,tick,fps);
	printf("\e[%d;%dH",3,1); // set cursor position (y,x)
	printf("[ position_x : %f position_y : %f ]",position_x,position_y);
	
	// Example: Display stored message when needed
	// printf("\e[%d;%dH",5,1);
	// printf("[ Stored: %s ]", get_stored_message());
}

//main function
int main(){ // FIXED: Change void main() to int main()
	start_setup();
	screen_draw();
	while (1){ // game loop
		update_game();
		display_var();
		physics_process();
		//usleep(16666); // 60 fps but in reality lower fps
	}
	return 0; // FIXED: Add return statement
}

// note
/*

      from modem's brance
printf("\e[1;1H\e[2J"); // clear screen like damn
printf("\e[1;1H\e[2J\e[1;1H\e[3J"); // clear screen & clear scroll up
printf("\e[%d;%dH",1,1); // set cursor position (y,x)

*/

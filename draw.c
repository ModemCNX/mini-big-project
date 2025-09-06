// DRAWING PROGRAM
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>

//variable that NOT gonna change by player ( system variable )
char version[] = "1.0.1";

int w=0,a=0,s=0,d=0,v=0,p=0,i=0,m1=0,shift=0,space=0,r=0,g=0,b=0,n0=0,n1=0,n2=0,n3=0,n4=0,n5=0,n6=0,n7=0,n8=0,n9=0; // user input flag

long int tick = 0;
LARGE_INTEGER frequency; // for game_time calculation
LARGE_INTEGER t1, t2; // for game_time calculation
float game_time = 0;
float delta_time = 0;
float fps = 0;
float fps_time = 0; // for fps calculation
int last_sec_tick = 0; // for fps calculation
float current_num_time = 0; // for input delay

float mode1_move_speed = 40;

CONSOLE_SCREEN_BUFFER_INFO csbi; // for get terminal size 
int terminal_x = 0; // store terminal size
int terminal_y = 0;
int mouse_x = 0;
int mouse_y = 0;
int screen_size_x = 0;
int screen_size_y = 0;

int screen_change = 0;
char screen_instruction_1[100000] = "";
char screen_instruction_2[100000] = "";

//variable that gonna change ( game variable )
int game_state = 1;
float position_x = 0;
float position_y = 0;

int canvas_x = 120;
int canvas_y = 30;

int current_r = 0;
int current_g = 0;
int current_b = 0;

int pause = 0;
//sprite
int screen[120][30];
char screen_data[360000] = "";
int player[10][10];

void update_screen(int x , int y){
	int pixel_r,pixel_g,pixel_b,pixel_rgb;
			pixel_rgb = screen[x-1][y-1];
			pixel_b = pixel_rgb % 1000;
			pixel_rgb /= 1000;
			pixel_g = pixel_rgb % 1000;
			pixel_rgb /= 1000;
			pixel_r = pixel_rgb % 1000;
			pixel_rgb /= 1000;
	printf("\e[%d;%dH\e[48;2;%d;%d;%dm ",y,x,pixel_r,pixel_g,pixel_b); // set cursor position (y,x)  // change color of ' ' to r:g:b value
}

void mode1_move(){
	float new_x = position_x;
	float new_y = position_y;
	if (shift) {
		mode1_move_speed = 10;
	}else{
		mode1_move_speed = 40;
	}
	if (w) new_y -= mode1_move_speed*delta_time;
	if (s) new_y += mode1_move_speed*delta_time;
	if (a) new_x -= mode1_move_speed*delta_time;
	if (d) new_x += mode1_move_speed*delta_time;
	
	if (new_x < 1) new_x = 1; // prevent negative position
	if (new_y < 1) new_y = 1;
	if (new_x > canvas_x) new_x = canvas_x; // prevent more position than screen
	if (new_y > canvas_y) new_y = canvas_y;
	
	if ((int)new_x != (int)position_x || (int)new_y != (int)position_y){
		int pixel_r,pixel_g,pixel_b,pixel_rgb;
			pixel_rgb = screen[((int)position_x)-1][((int)position_y)-1];
			pixel_b = pixel_rgb % 1000;
			pixel_rgb /= 1000;
			pixel_g = pixel_rgb % 1000;
			pixel_rgb /= 1000;
			pixel_r = pixel_rgb % 1000;
			pixel_rgb /= 1000;
		printf("\e[%d;%dHX",(int)new_y,(int)new_x); // set cursor position (y,x)
		update_screen((int)position_x,(int)position_y);
	}
	
	position_x = new_x;
	position_y = new_y;
}

void move_mouse(){
	if (screen_size_x == 0 || screen_size_y == 0) {
		return;
	}
	
	float new_x = position_x;
	float new_y = position_y;
	
	//printf("\e[1;1H  [ %d ]       ",(int)(terminal_x*((float)mouse_x / (float)screen_size_x)));
	new_x = (int)(terminal_x*((float)mouse_x / (float)screen_size_x));
	new_y = (int)(terminal_y*((float)mouse_y / (float)screen_size_y));
	
	if (new_x < 1) new_x = 1; // prevent negative position
	if (new_y < 1) new_y = 1;
	if (new_x > canvas_x) new_x = canvas_x; // prevent more position than screen
	if (new_y > canvas_y) new_y = canvas_y;
	
	if ((int)new_x != (int)position_x || (int)new_y != (int)position_y){
		int pixel_r,pixel_g,pixel_b,pixel_rgb;
			pixel_rgb = screen[((int)position_x)-1][((int)position_y)-1];
			pixel_b = pixel_rgb % 1000;
			pixel_rgb /= 1000;
			pixel_g = pixel_rgb % 1000;
			pixel_rgb /= 1000;
			pixel_r = pixel_rgb % 1000;
			pixel_rgb /= 1000;
		update_screen((int)new_x,(int)new_y);
		printf("\e[%d;%dHX",(int)new_y,(int)new_x); // set cursor position (y,x)
		update_screen((int)position_x,(int)position_y);
	}
	
	position_x = new_x;
	position_y = new_y;
	
}

void start_setup(){
    printf("\e[?25l"); // hide cursor
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);
    screen_size_x = GetSystemMetrics(SM_CXSCREEN);
    screen_size_y = GetSystemMetrics(SM_CYSCREEN);
    position_x = canvas_x / 2.0f;
    position_y = canvas_y / 2.0f;
}

// update function
void update_terminal_size(){ // bruh
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	terminal_x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	terminal_y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
void update_input(){
	w = GetAsyncKeyState('W')!=0||GetAsyncKeyState(0x26)!=0?1:0;
	a = GetAsyncKeyState('A')!=0||GetAsyncKeyState(0x25)!=0?1:0;
	s = GetAsyncKeyState('S')!=0||GetAsyncKeyState(0x28)!=0?1:0;
	d = GetAsyncKeyState('D')!=0||GetAsyncKeyState(0x27)!=0?1:0;
	v =  GetAsyncKeyState('V')!=0?1:0;
	p =  GetAsyncKeyState('P')!=0?1:0;
	i =  GetAsyncKeyState('I')!=0?1:0;
	
	m1 = GetAsyncKeyState(0x01)!=0?1:0;
	shift = GetAsyncKeyState(0x10)!=0?1:0;
	space = GetAsyncKeyState(0x20)!=0?1:0;
	
	r = GetAsyncKeyState('R')||GetAsyncKeyState('Z')!=0?1:0;
	g = GetAsyncKeyState('G')||GetAsyncKeyState('X')!=0?1:0;
	b = GetAsyncKeyState('B')||GetAsyncKeyState('C')!=0?1:0;
	
	n0 = GetAsyncKeyState('0')!=0||GetAsyncKeyState(0x60)!=0?1:0;
	n1 = GetAsyncKeyState('1')!=0||GetAsyncKeyState(0x61)!=0?1:0;
	n2 = GetAsyncKeyState('2')!=0||GetAsyncKeyState(0x62)!=0?1:0;
	n3 = GetAsyncKeyState('3')!=0||GetAsyncKeyState(0x63)!=0?1:0;
	n4 = GetAsyncKeyState('4')!=0||GetAsyncKeyState(0x64)!=0?1:0;
	n5 = GetAsyncKeyState('5')!=0||GetAsyncKeyState(0x65)!=0?1:0;
	n6 = GetAsyncKeyState('6')!=0||GetAsyncKeyState(0x66)!=0?1:0;
	n7 = GetAsyncKeyState('7')!=0||GetAsyncKeyState(0x67)!=0?1:0;
	n8 = GetAsyncKeyState('8')!=0||GetAsyncKeyState(0x68)!=0?1:0;
	n9 = GetAsyncKeyState('9')!=0||GetAsyncKeyState(0x69)!=0?1:0;
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

void update_mouse_position_in_screen(){ // can be outside terminal tab
	POINT mouse ;
	GetCursorPos(&mouse);
	mouse_y = mouse.y +1; // mouse on top-left of screen is (0,0) so we need to offset by (1,1)
	mouse_x = mouse.x +1; // mouse on top-left of screen is (0,0) so we need to offset by (1,1)
}

void update_game(){
	update_terminal_size();
	update_game_tick();
	update_game_time();
	update_fps();
	update_mouse_position_in_screen();
	update_input();
}

void draw_screen(){
	int i,j;
	for(i=0;i<canvas_y;i++){
		printf("\e[%d;1H",i+1); // set cursor position (y,x)
		for(j=0;j<canvas_x;j++){
			int pixel_r,pixel_g,pixel_b,pixel_rgb;
			pixel_rgb = screen[j][i];
			pixel_b = pixel_rgb % 1000;
			pixel_rgb /= 1000;
			pixel_g = pixel_rgb % 1000;
			pixel_rgb /= 1000;
			pixel_r = pixel_rgb % 1000;
			pixel_rgb /= 1000;
			printf("\e[48;2;%d;%d;%dm ",pixel_r,pixel_g,pixel_b);// change color of ' ' to r:g:b value	
		}
	}
}

void show_current_rgb(){
	printf("\e[%d;%dH",1,1); // set cursor position (y,x)  printf("\e[48;2;%d;%d;%dm ",0,0,0);// change color of ' ' to r:g:b value
	printf("[\e[48;2;%d;%d;%dm \e[48;2;0;0;0m][ R:%3d G:%3d B:%3d ]",current_r,current_g,current_b,current_r,current_g,current_b);
}

void set_screen_data(){
	int index = 0;
	
	int i,j;
	for(i=0;i<canvas_y;i++){
		for(j=0;j<canvas_x;j++){
			char pixel_data[10] = "";
			strncpy(pixel_data , screen_data + index , 9);
			index += 9;
			screen[j][i] = 1000000000 + atoi(pixel_data);
		}
	}
	
}

void update_screen_data(){
	strcpy(screen_data,"");
	int i,j;
	for(i=0;i<canvas_y;i++){
		for(j=0;j<canvas_x;j++){
			char pixel_data[10] = "";
			char save_data[10] = "";
			if(screen[j][i]== 0)screen[j][i] = 1000000000;
			itoa(screen[j][i],pixel_data,10);
			strncpy(save_data, pixel_data + 1, 9);
			//if(!(i+j))printf("[%d]",strlen(save_data));
			strcat(screen_data,save_data);
		}
	}
	
}

void check_action(){
	if(pause){
		if(v){
			pause = 0;
			printf("\e[1;1H\e[2J\e[1;1H\e[3J"); // clear screen & clear scroll up
			draw_screen();
		}
		return;
	}
	if(space){
		screen[((int)position_x)-1][((int)position_y)-1] = 1000000000 + current_b + (1000*current_g) + (1000000*current_r);
		update_screen((int)position_x,(int)position_y);
		printf("\e[%d;%dHX",(int)position_y,(int)position_x); // set cursor position (y,x)
	}
	if(r+g+b){
		show_current_rgb();
		int var = -1;
		if(n0){
			var = 0;
		}else if(n1){
			var = 1;
		}else if(n2){
			var = 2;
		}else if(n3){
			var = 3;
		}else if(n4){
			var = 4;
		}else if(n5){
			var = 5;
		}else if(n6){
			var = 6;
		}else if(n7){
			var = 7;
		}else if(n8){
			var = 8;
		}else if(n9){
			var = 9;
		}
		if (var!= -1 && game_time > current_num_time + 0.2){
			if(r)current_r = ((current_r * 10) + var)%1000;
			if(g)current_g = ((current_g * 10) + var)%1000;
			if(b)current_b = ((current_b * 10) + var)%1000;
			current_num_time = game_time;
		}
	}
	if(v){
		printf("\e[48;2;%d;%d;%dm ",0,0,0);// change color of ' ' to r:g:b value
		printf("\e[1;1H\e[2J\e[1;1H\e[3J"); // clear screen & clear scroll up
		draw_screen();
	}
	if(p){
		printf("\e[48;2;%d;%d;%dm ",0,0,0);// change color of ' ' to r:g:b value
		printf("\e[1;1H\e[2J\e[1;1H\e[3J"); // clear screen & clear scroll up
		printf("\e[%d;%dH",1,1); // set cursor position (y,x)
		pause = 1;
		update_screen_data();
		printf("%s",screen_data);
	}
	if(i){
		int pixel_r,pixel_g,pixel_b,pixel_rgb;
			pixel_rgb = screen[((int)position_x)-1][((int)position_y)-1];
			pixel_b = pixel_rgb % 1000;
			pixel_rgb /= 1000;
			pixel_g = pixel_rgb % 1000;
			pixel_rgb /= 1000;
			pixel_r = pixel_rgb % 1000;
			pixel_rgb /= 1000;
		current_r = pixel_r;
		current_g = pixel_g;
		current_b = pixel_b;
		show_current_rgb();
	}
}


menu(){
	printf("version : %s\n",version);
	printf("\n<key>\n-hold R/Z press number to set Red\n-hold G/X press number to set Green\n-hold B/C press number to set Blue\n-hold Space Draw\n- V reset screen\n- I select color\n- P print data\n");
	printf("\nIf you want to load immage data paste number now \nIf you want to draw type anything that NOT A NUMBER!!!\n");
	scanf("%s",&screen_data);
	if(screen_data[0] >= '0' && screen_data[0] <= '9'){
		printf("\e[1;1H\e[2J\e[1;1H\e[3J"); // clear screen & clear scroll up
		set_screen_data();
		draw_screen();
	}
}

//main bruh
void main(){
	start_setup();
	menu();
	draw_screen();
	while (1){ // game loop
		update_game();
		if(!pause){
			move_mouse();
		}
		check_action();
	}
	// end game
}


// note
/*
printf("\e[1;1H\e[2J"); // clear screen
printf("\e[1;1H\e[2J\e[1;1H\e[3J"); // clear screen & clear scroll up
printf("\e[%d;%dH",1,1); // set cursor position (y,x)
printf("\e[48;2;%d;%d;%dm ",0,0,0);// change color of ' ' to r:g:b value
*/


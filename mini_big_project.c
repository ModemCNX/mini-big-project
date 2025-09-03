#include <stdio.h>

//variable that NOT gonna change by player ( system variable )
int w=0,a=0,s=0,d=0,m1=0; // user input flag

long int tick = 0;
int start_time = 0; // for game_time calculation
int game_time = 0;
int fps = 0;
int fps_time = 0; // for fps calculation
int last_sec_tick = 0; // for fps calculation

int terminal_x = 0; // store terminal size
int terminal_y = 0;

int mouse_x = 0;
int mouse_y = 0;
int screen_size_x = 0;
int screen_size_y = 0;


//variable that gonna change ( game variable )
int game_state = 0;
int position[2] = {3,5};
int position_last_frame[2];


//function 

void move(){
	if (w) position[1]-=1;
	if (s) position[1]+=1;
	if (a) position[0]-=1;
	if (d) position[0]+=1;
	
	if (position[0] < 1) position[0] = 1; // prevent negative position
	if (position[1] < 1) position[1] = 1;
	
	if (position[0] != position_last_frame[0] || position[1] != position_last_frame[1]){ // draw @ at position
		printf("\e[%d;%dH ",position_last_frame[1],position_last_frame[0]);
		printf("\e[%d;%dH@",position[1],position[0]);
	}
	
	position_last_frame[0] = position[0]; // update position last frame 
	position_last_frame[1] = position[1];
}

void move_mouse(){
	if (screen_size_x == 0 || screen_size_y == 0) {
		return;
	}
	position[0] = (int)(terminal_x*((float)mouse_x / (float)screen_size_x));
	position[1] = (int)(terminal_y*((float)mouse_y / (float)screen_size_y));
	
	if (position[0] < 1) position[0] = 1; // prevent negative position
	if (position[1] < 1) position[1] = 1;
	
	if (position[0] != position_last_frame[0] || position[1] != position_last_frame[1]){ // draw @ at position
		printf("\e[%d;%dH ",position_last_frame[1],position_last_frame[0]);
		printf("\e[%d;%dH@",position[1],position[0]);
	}
	
	position_last_frame[0] = position[0]; // update position last frame 
	position_last_frame[1] = position[1];
}

void print_full_screen(){
	int i,l;
	for(i=0;i<40;i++){
		
		for(l=0;l<160;l++){
			printf("\e[%d;%dH*",position_last_frame[l],position_last_frame[i]);
		}
	}
}

void update_terminal_size(){ // bruh
    int data; // contain letter from printf("\e[6n"); (and user input)
    int count=0,x=0,y=0;
    int reach_x =0; // flag when data give ';' 
    printf("\e[%d;%dH",10000,10000); // set cursor position (this willcap at terminal size)
    printf("\e[6n"); // print cursor position need to getch() it (below)
	while(1)
	{
		//printf("----------c = %d------------\n",count);      //for debug 
		
		if(count == -1){
			//printf("[break %d] ",count);
			break;
		}
		else{
		
			data = getch(); //get character 1 by 1
			//printf("\n  [ %c  ][ascii %d]\n",data,data);         //for debug 
			
			if (data == ';'){
				reach_x =1;
			}else if (count > 1 && data != 'R'){
				if (reach_x){
					x = x*10 + (data-'0');
					//printf(" [%d] ",x);    //for debug 
				}else {
					y = y*10 + (data-'0');
					//printf(" [%d] ",y);        //for debug 
				}
			}
			if (count > 0)count ++ ;// next character
			if (data == 27){ // data befor ascii 27(cant use '?') is user input need to be ignore
				count = 1;
			}else if (data == 0){
				count = -1;
				printf("\e[%d;%dH",0,0); // set cursor position (y,x)
				printf("[ERROR] did not use getch() befor use update_terminal_size()");  // error massage      pls use getch() at lease once befor use this function
				
			}
			else if (data == 'R'){ // end
				count = -1;
				terminal_x = x;
				terminal_y = y;
			}
		}
	}
}
// start funtion

void start_setup(){
	start_time = time(NULL);
	int screen_size[2];
	screen_size_x = GetSystemMetrics(screen_size[3]);
	screen_size_y = GetSystemMetrics(screen_size[2]);
}

// update function

void update_input(){
	w = GetAsyncKeyState('W')!=0?1:0;
	a = GetAsyncKeyState('A')!=0?1:0;
	s = GetAsyncKeyState('S')!=0?1:0;
	d = GetAsyncKeyState('D')!=0?1:0;
	m1 = GetAsyncKeyState(0x01)!=0?1:0;
}
void update_mouse_position_in_screen(){ // can be outside terminal tab
	int mouse[2] ;
	GetCursorPos(mouse);
	mouse_x = mouse[0] +1; // mouse on top-left of screen is (0,0) so we need to offset by (1,1)
	mouse_y = mouse[1] +1; // mouse on top-left of screen is (0,0) so we need to offset by (1,1)
}

void update_game_tick(){
	tick ++;
}

void update_game_time(){
	game_time = time(NULL) - start_time;
}

void update_fps(){  // fps of last frame
	if (game_time > fps_time){
		fps = (tick - last_sec_tick) / (game_time - fps_time);
		fps_time = game_time;
		last_sec_tick = tick;
	}
}

void update_game(){
	update_terminal_size();
	update_game_tick();
	update_game_time();
	update_fps();
	update_mouse_position_in_screen();
	update_input();
}

void display_var(){
	printf("\e[1;1H\e[2J\e[1;1H\e[3J"); // clear screen & clear scroll up
	printf("\e[%d;%dH",1,1); // set cursor position (y,x)
	printf("[ terminal size : %d  %d ]",terminal_x,terminal_y);
	printf("\e[%d;%dH",2,1); // set cursor position (y,x)
	printf("[ game_time: %d  tick: %d  fps: %d ]",game_time,tick,fps);
	printf("\e[%d;%dH",3,1); // set cursor position (y,x)
	printf("[ screen size : %d %d  mouse position: %d %d ]",screen_size_x,screen_size_y,mouse_x,mouse_y);
	printf("\e[%d;%dH",4,1); // set cursor position (y,x)
	printf("[ input w %d  a %d  s %d  d %d  m1 %d ]",w,a,s,d,m1);
}
//main bruh
void main(){
	start_setup();
	ShowCursor(0);
	printf("press any key for [update_terminal_size()] to work");
	getch(); // need to getch at start (or befor use update_terminal_size())  for check_terminal_size() to work without have to getch() every time you want to know size
	printf("\e[1;1H\e[2J\e[1;1H\e[3J"); // clear screen & clear scroll up
	while (1){ // game loop
		update_game();
		display_var();
		move_mouse();
		usleep(16666); // 60 fps but in reality lower fps
	}
	// end game
}

// note
/*
hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhg
printf("\e[1;1H\e[2J"); // clear screen
printf("\e[1;1H\e[2J\e[1;1H\e[3J"); // clear screen & clear scroll up
printf("\e[%d;%dH",1,1); // set cursor position (y,x)

*/


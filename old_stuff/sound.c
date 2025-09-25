#include <stdio.h>
#include <pthread.h>  // Required for multithreading
#include <windows.h>

char version[] = "1.0.0";

int w=0,a=0,s=0,d=0,space=0; // user input flag

LARGE_INTEGER frequency; // for game_time calculation
LARGE_INTEGER t1, t2; // for game_time calculation
float game_time = 0;
float delta_time = 0;
int current_sound = -1; // -1 = no sound currently playeing
int sound_index = 0; // 0 = first note
float sound_time = -200;
pthread_t beep_thread; // for multithread sound system

float eee =0;  ///// all this for debug only!!
int aaa = 0;
int bbb = 0;
int ccc = 0;

#define b3 247  ///// for sound note XD
#define c4 262
#define d4 294
#define d4sharp 311
#define e4 330
#define f4 349
#define f4sharp 370
#define g4 392
#define a4 440
#define b4 494
#define c5 523
#define d5 587
#define e5 659
#define f5 698
#define g5 784
#define a5 880

#define whole 1600  ///// for sound time XD
#define haft 800
#define quarter 400
#define eighth 200 // if use 200 or shorter time sound may not work properly

int sound_data[2][100][2]=   //[sound count]  [max sound index(note)]  [note data(0 time to play note(ms),1 note frequency)]
{
	{  // sound 0 test sound
		{500,200},
		{500,300},
		{500,400},
	},{ // sound 1 best indie game of all time.
		{quarter,b3},
		{quarter,f4},
		{quarter,e4},
		{quarter,b3},
		{haft,d4sharp},
		{haft,d4sharp},
		{quarter,e4},
		{quarter,0},
		{quarter,b3},
		{quarter,e4},
		{quarter,b3},
		{haft,d4sharp},
		{haft,d4sharp},
		{quarter,e4},            ////helf way
		{quarter,b3},
		{quarter,f4},
		{quarter,e4},
		{quarter,b3},
		{haft,d4sharp},
		{haft,d4sharp},
		{quarter,e4},
		{quarter,0},
		{quarter,b3},
		{quarter,e4},
		{quarter,g4},
		{haft,f4sharp},
		{haft,e4},
		{quarter,f4},
	}		
};

void* beep_thread_function(void* e) {
    aaa = sound_index;////////////////////////////////////////////////////////////////  for debug
    bbb = sound_data[current_sound][sound_index][0];
    ccc = sound_data[current_sound][sound_index][1];
    
    
    Beep(sound_data[current_sound][sound_index][1],sound_data[current_sound][sound_index][0]);
    
}

void set_sound(int set_sound_index){
	current_sound = set_sound_index;
	sound_index = 0;
	sound_time = sound_data[current_sound][sound_index][0];
	pthread_create(&beep_thread,NULL,beep_thread_function,NULL);  // make sound thread
}

void sound_process(){
	if(current_sound <= -1) return;
	sound_time -= delta_time*1000;
	if(sound_time > 0) return;
	sound_time += sound_data[current_sound][sound_index][0];
	sound_index += 1;
	if(sound_data[current_sound][sound_index][0] <= 0 && sound_data[current_sound][sound_index][1] <= 0){  // if note is empty stop sound
		set_sound(current_sound);
	}else if(sound_data[current_sound][sound_index][1]  != 0){ // if not 0 frequency (wait)
		pthread_create(&beep_thread,NULL,beep_thread_function,NULL);  // make sound thread
	}else{  // 0 frequency
		//eee ++;
	}
}


void start_setup(){
	printf("\e[?25l"); // hide cursor
	SetConsoleOutputCP(CP_UTF8);      // for print emoji(player character in combat mode)
	QueryPerformanceFrequency(&frequency); // for update_game_time(){
	QueryPerformanceCounter(&t1);
}


void display_var(){              // for debug only
	printf("\e[48;2;0;0;0m");// change color of '' to r:g:b value	
	printf("\e[%d;%dH",1,1); // set cursor position (y,x)
	printf("[ version %s]",version);
	printf("[ game_time: %f delta_time: %f fps: %d ]              ",game_time,delta_time,(int)(1/delta_time));
	printf("\e[%d;%dH",2,1); // set cursor position (y,x)
	printf("[current_sound:%d sound_id(note number):%d ]",current_sound,sound_index);
	printf("\e[%d;%dH",3,1); // set cursor position (y,x)
	printf("[sound_time(ms)):%f ]",sound_time);
	printf("\e[%d;%dH",4,1); // set cursor position (y,x)
	printf("[(last played note)(note long(ms))(frequency) [%d][%d][%d]]           ",aaa,bbb,ccc);
	printf("\e[%d;%dH",6,1); // set cursor position (y,x)
	printf("[eee:%f]",eee);
	
	//printf("[ input w %d  a %d  s %d  d %d  m1 %d ]",w,a,s,d,m1);
}


float input_time = 0;   // for input only once (so sound get set once in 1 click)
void update_input(){
	w = 0;
	a = 0;
	s = 0;
	d = 0;
	
	if(input_time >= game_time)return;
	if(!(GetAsyncKeyState('W')+GetAsyncKeyState('A')+GetAsyncKeyState('S')+GetAsyncKeyState('D')))return;
	input_time = game_time + 0.3;
	
	
	w = GetAsyncKeyState('W')!=0||GetAsyncKeyState(0x26)!=0?1:0;
	a = GetAsyncKeyState('A')!=0||GetAsyncKeyState(0x25)!=0?1:0;
	s = GetAsyncKeyState('S')!=0||GetAsyncKeyState(0x28)!=0?1:0;
	d = GetAsyncKeyState('D')!=0||GetAsyncKeyState(0x27)!=0?1:0;
	space = GetAsyncKeyState(0x20)!=0?1:0;
}


void update_game_time(float *game_time){
	QueryPerformanceCounter(&t2);
	delta_time = ((t2.QuadPart - t1.QuadPart) / (float)frequency.QuadPart)  - *game_time;
	*game_time = (t2.QuadPart - t1.QuadPart) / (float)frequency.QuadPart;
}


void update_game(){
	update_game_time(&game_time);
	update_input();
	sound_process();   //sound system
}



void main() {
	start_setup();
    while(1){
    	update_game();
    	display_var();  // for debug only
    	if (w){
    		set_sound(-1);
		}else if (a){
    		set_sound(0);
		}else if (s){
    		set_sound(1);
		}
	}
}

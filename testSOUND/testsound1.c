#include <windows.h>
#include <stdio.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	PlaySound("name.wav", NULL, SND_FILENAME | SND_ASYNC);
	printf("play sound");
	
	sleep(5);
	
	PlaySound(NULL, 0, 0);
	printf("black silence!?!");
	
	sleep(5);
	
}

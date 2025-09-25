#include <windows.h>

#include <mmsystem.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	PlaySound(TEXT("Name.wav"), NULL, SND_SYNC);
}

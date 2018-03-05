#ifndef UINPUT_MANAGER_H
#define UINPUT_MANAGER_H

#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <time.h>
#include <sys/time.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <utils/RefBase.h>
#include <cutils/log.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <malloc.h>
#include <stdint.h>

#define KEYDOWN 1
#define KEYUP 0
#define TOUCHDOWN 1
#define TOUCHUP 0
#define ROTATING_90 1
#define ROTATING_180 2
#define ROTATING_270 3
class UinputManager{
private:
int fd_keyboard;
int fd_mouse;
int fd_touch;
int width;
int height;
void adapterxy(int &x,int &y,int ori);
public:
	UinputManager();
	virtual ~UinputManager();
	int createKeyboardDevice();
	int handleKey(int keycode,int status);

	int createMouseDevice();
	int handleMouseMove(int RelX,int RelY);
	int handleMouseKey(int status,int keycode);

	int createTouchDevice();
	int handleTouch(int type,int x,int y,int ori,int touchcode);
};

#endif
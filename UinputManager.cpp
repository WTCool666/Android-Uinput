#include "UinputManager.h"
UinputManager::UinputManager():fd_mouse(-1),fd_keyboard(-1),fd_touch(-1),width(-1),height(-1){
}
virtual UinputManager::~UinputManager(){
}
int UinputManager::createKeyboardDevice(){
	struct uinput_user_dev uinp; 
	fd_keyboard=open("/dev/uinput", O_WRONLY | O_NDELAY);
	if(fd_keyboard<0){
		return -1;
	}
	memset(&uinp, 0x00, sizeof(uinp));
       strncpy(uinp.name,"myKeyboard", strlen("myKeyboard"));
       uinp.id.version = 1;
       uinp.id.bustype = BUS_USB;
	uinp.id.vendor = ***;
	uinp.id.product = ***;
	
	uinp.absmin[ABS_HAT0X] = -1;
	uinp.absmax[ABS_HAT0X] = 1;
	uinp.absfuzz[ABS_HAT0X] = 0;
	uinp.absflat[ABS_HAT0X] = 0;

	uinp.absmin[ABS_HAT0Y] = -1;
	uinp.absmax[ABS_HAT0Y] = 1;
	uinp.absfuzz[ABS_HAT0Y] = 0;
	uinp.absflat[ABS_HAT0Y] = 0;
	ioctl(fd_keyboard , UI_SET_EVBIT, EV_KEY);
	ioctl(fd_keyboard , UI_SET_EVBIT, EV_ABS);
	

	for(int i = 1; i <= 11; i++)
	{
	    ioctl(fd_keyboard , UI_SET_KEYBIT, i);
	}
	for(int i = 59; i < 255; i++)
	{
	    ioctl(fd_keyboard , UI_SET_KEYBIT, i);
	}

	ioctl(fd_keyboard , UI_SET_KEYBIT, KEY_ENTER);
	ioctl(fd_keyboard , UI_SET_KEYBIT, 0x192);
	ioctl(fd_keyboard, UI_SET_KEYBIT, 0x193);
	
	ioctl(fd_keyboard, UI_SET_KEYBIT, KEY_SELECT);
	ioctl(fd_keyboard , UI_SET_ABSBIT, ABS_HAT0X);
	ioctl(fd_keyboard , UI_SET_ABSBIT, ABS_HAT0Y);
    
	if(write(fd_keyboard , &uinp, sizeof(uinp)) != sizeof(uinp))
	{
		close(fd_keyboard);
		fd_keyboard= -1;
	    	return -1;
	}
	if(ioctl(fd_keyboard , UI_DEV_CREATE))
	{
		close(fd_keyboard);
		fd_keyboard= -1;
		return -1;
	}
    	return fd_keyboard;
}

int UinputManager::handleKey(int keycode,int status){
		struct input_event event;
		int ret = -1;
		if(fd_keyboard<0){
			return -1;
		}
		memset(&event, 0, sizeof(event));
		gettimeofday(&event.time, NULL); 
		switch(status)
		{
		case KEYUP:
		    event.type = EV_KEY;
		    event.code = keycode;
		    event.value = 0;
		    write(fd_keyboard, &event, sizeof(event));
		    event.type = EV_SYN;
		    event.code = 0;
		    event.value = 0;
		 ret = write(fd_keyboard, &event, sizeof(event)) ;
		    break;
		case KEYDOWN:
		    event.type = EV_KEY;
		    event.code = keycode;
		    event.value = 1;
		    write(fd_keyboard, &event, sizeof(event));
		    event.type = EV_SYN;
		    event.code = 0;
		    event.value = 0;
		    ret = write(fd_keyboard, &event, sizeof(event));
		    break;
		default:
		    break;
		    
		}
		if(ret < 0){
			close(fd_keyboard);
			fd_keyboard= -1;
			return -1; 
		}
		return ret;
}

int UinputManager::createMouseDevice(){
	struct uinput_user_dev uinp;
	fd_mouse = open("/dev/uinput", O_WRONLY | O_NDELAY);
	 if(fd_mouse <= 0)
	{
	   return -1;
	}
	memset(&uinp, 0x00, sizeof(uinp));
	strncpy(uinp.name, "myMouse", strlen("myMouse"));
	uinp.id.version = 1;
	uinp.id.bustype = BUS_USB;

	ioctl(fd_mouse, UI_SET_EVBIT, EV_KEY);
	ioctl(fd_mouse, UI_SET_EVBIT, EV_REL);
	ioctl(fd_mouse, UI_SET_KEYBIT, BTN_MIDDLE);
	ioctl(fd_mouse, UI_SET_KEYBIT, BTN_LEFT);
	ioctl(fd_mouse, UI_SET_KEYBIT, BTN_RIGHT);
	ioctl(fd_mouse, UI_SET_RELBIT, REL_X);
	ioctl(fd_mouse, UI_SET_RELBIT, REL_Y);

	if(write(fd_mouse, &uinp, sizeof(uinp)) != sizeof(uinp))
	{
		close(fd_mouse);
		fd_mouse = -1;	
		return fd_mouse;
	}

	if(ioctl(fd_mouse, UI_DEV_CREATE))
	{
		close(fd_mouse);
		fd_mouse = -1;	
		return fd_mouse;
	}
	return fd_mouse;

}

int UinputManager::handleMouseMove(int RelX,int RelY){
	if(fd_mouse<0){
		return -1;
	}
	int ret=-1;
	static struct input_event ievent[3];
	static struct timespec now;
	float tv_nsec = 0;
	clock_gettime(CLOCK_MONOTONIC, &now);
	tv_nsec = now.tv_nsec / 1000;

	ievent[0].time.tv_sec = now.tv_sec;
	ievent[0].time.tv_usec = tv_nsec;
	ievent[0].type = EV_REL;
	ievent[0].code = REL_X;
	ievent[0].value = RelX;

	ievent[1].time.tv_sec = now.tv_sec;
	ievent[1].time.tv_usec = tv_nsec;
	ievent[1].type = EV_REL;
	ievent[1].code = REL_Y;
	ievent[1].value = RelY;

	ievent[2].time.tv_sec = now.tv_sec;
	ievent[2].time.tv_usec = tv_nsec;
	ievent[2].type = EV_SYN;
	ievent[2].code = 0;
	ievent[2].value = 0;

	ret=write(fd_mouse, &ievent[0], sizeof(ievent[0]));
	ret=write(fd_mouse, &ievent[1], sizeof(ievent[1]));
	ret=write(fd_mouse, &ievent[2], sizeof(ievent[2]));

	return ret;
}
int UinputManager::handleMouseKey(int status,int keycode){
	if(fd_mouse<0){
		return -1;
	}
	int ret=-1;
	struct input_event event;
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL); 

	event.type = EV_KEY;
	event.code = keycode;
	event.value = status;
	write(fd_mouse, &event, sizeof(event));

	event.type = EV_SYN;
	event.code = 0;
	event.value = 0;
	ret=write(fd_mouse, &event, sizeof(event));

	return ret;
}

int UinputManager::createTouchDevice(int sw,int sh){
	struct uinput_user_dev uinp; 
	width= sw;
	height= sh;

	fd_touch= open("/dev/uinput", O_WRONLY | O_NDELAY);
	if(fd_touch <= 0)
	{
		return false;
	}
	memset(&uinp, 0x00, sizeof(uinp));
	strncpy(uinp.name, "myTouch", strlen("myTouch"));
	uinp.id.version = 1;
	uinp.id.bustype = BUS_USB;

	uinp.absmin[ABS_MT_POSITION_X] = 0;
	uinp.absmax[ABS_MT_POSITION_X] = sw;
	uinp.absfuzz[ABS_MT_POSITION_X] = 0;
	uinp.absflat[ABS_MT_POSITION_X] = 0;

	uinp.absmin[ABS_MT_POSITION_Y] = 0;
	uinp.absmax[ABS_MT_POSITION_Y] = sh;
	uinp.absfuzz[ABS_MT_POSITION_Y] = 0;
	uinp.absflat[ABS_MT_POSITION_Y] = 0;

	uinp.absmin[ABS_MT_PRESSURE] = 0;
	uinp.absmax[ABS_MT_PRESSURE] = 1;
	uinp.absfuzz[ABS_MT_PRESSURE] = 0;
	uinp.absflat[ABS_MT_PRESSURE] = 0;
	uinp.absmin[ABS_MT_TOUCH_MAJOR] = 0;
	uinp.absmax[ABS_MT_TOUCH_MAJOR] = 31;
	uinp.absfuzz[ABS_MT_TOUCH_MAJOR] = 0;
	uinp.absflat[ABS_MT_TOUCH_MAJOR] = 0;
	uinp.absmin[ABS_MT_SLOT] = 0;
	uinp.absmax[ABS_MT_SLOT] = MT_SLOT_T;
	uinp.absfuzz[ABS_MT_SLOT] = 0;
	uinp.absflat[ABS_MT_SLOT] = 0;
	uinp.absmin[ABS_MT_TRACKING_ID] = 0;
	uinp.absmax[ABS_MT_TRACKING_ID] = MT_SLOT_T;
	uinp.absfuzz[ABS_MT_TRACKING_ID] = 0;
	uinp.absflat[ABS_MT_TRACKING_ID] = 0;

	ioctl(uinput_tfd, UI_SET_EVBIT, EV_ABS);
	ioctl(uinput_tfd, UI_SET_ABSBIT, ABS_MT_SLOT);
	ioctl(uinput_tfd, UI_SET_ABSBIT, ABS_MT_TRACKING_ID);
	ioctl(uinput_tfd, UI_SET_ABSBIT, ABS_MT_PRESSURE);
	ioctl(uinput_tfd, UI_SET_ABSBIT, ABS_MT_TOUCH_MAJOR);
	ioctl(uinput_tfd, UI_SET_ABSBIT, ABS_MT_POSITION_X);
	ioctl(uinput_tfd, UI_SET_ABSBIT, ABS_MT_POSITION_Y);
	ioctl(uinput_tfd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);

	if(write(fd_touch, &uinp, sizeof(uinp)) != sizeof(uinp))
	{
		close(fd_touch);
		fd_touch = -1;
		return fd_touch;
	}

	if(ioctl(fd_touch, UI_DEV_CREATE))
	{
		close(fd_touch);
		fd_touch = -1;
		return fd_touch;
	}    
	return 1;
}
void UinputManager::adapterxy(int &x,int &y,int ori)
{
	int tx = x,ty = y;
	switch(ori)
	{
		case ROTATING_90:	
			x  =  width- ty;
			y  =  tx;
			break;
		case ROTATING_180:
			x = width - tx;
			y = height- ty;
			break;
	    case ROTATING_270:
			x = ty;
			y = height -x;
			break;
	}
}

int UinputManager::handleTouch(int type,int x,int y,int ori,int touchcode){
	struct input_event event;

	if(fd_touch<0){
		if(!createTouchDevice(width,height))
		{
			return -1;
		}
	}

	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL); 

	switch (type){
	case TOUCHDOWN:               //Touch
	{
		adapterxy(x,y,ori);//test
		event.type = EV_ABS;
		event.code = ABS_MT_SLOT;
		event.value = touchcode;
		write(fd_touch, &event, sizeof(event));

		event.type = EV_ABS;
		event.code = ABS_MT_TRACKING_ID;
		event.value = touchcode;
		write(fd_touch, &event, sizeof(event));


		event.type = EV_ABS;
		event.code = ABS_MT_TOUCH_MAJOR;
		event.value = MT_MAJOR;
		write(fd_touch, &event, sizeof(event));


		event.type = EV_ABS;
		event.code = ABS_MT_PRESSURE;
		event.value = 1;
		write(fd_touch, &event, sizeof(event));

		event.type = EV_ABS;
		event.code = ABS_MT_POSITION_X;
		event.value = x;
		write(fd_touch, &event, sizeof(event));

		event.type = EV_ABS;
		event.code = ABS_MT_POSITION_Y;
		event.value = y;
		write(fd_touch, &event, sizeof(event));

		event.type = EV_SYN;
		event.code = SYN_REPORT;
		event.value = 0;
		write(fd_touch, &event, sizeof(event));
	}
	break;
		
	case TOUCHUP:                //Left    
	{
		event.type = EV_ABS;
		event.code = ABS_MT_SLOT;
		event.value = touchcode;
		write(fd_touch, &event, sizeof(event));

		event.type = EV_ABS;
		event.code = ABS_MT_TRACKING_ID;
		event.value = -1;
		write(fd_touch, &event, sizeof(event));

		event.type = EV_SYN;
		event.code = SYN_REPORT;
		event.value = 0;
		write(fd_touch, &event, sizeof(event));
	}
	break;
	
	default:
		break;
	}
	return 1;
}
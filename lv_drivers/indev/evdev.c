/**
 * @file evdev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "evdev.h"
#if USE_EVDEV != 0

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>		


/*tslib head*/
#include "config.h"				
		
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include "tslib.h"

/*thread head*/				
#include "task.h"	

	
/*********************
 *      DEFINES
 *********************/

#define USE_TSLIB	1
#define USE_INPUT_API 1
						
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
int map(int x, int in_min, int in_max, int out_min, int out_max);

/**********************
 *  STATIC VARIABLES
 **********************/
int evdev_fd;
int evdev_root_x;
int evdev_root_y;
int evdev_button;

struct tsdev *ts;

static pthread_t tslib_thread_id;	

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if USE_TSLIB == 1

void *tslib_thread(void *pArg)
{	
	printf("tslib_thread\r\n");

    while(1)
    {									
		struct ts_sample samp;
		int ret;			
		
		ret = ts_read(ts, &samp, 1);
												
		if (samp.pressure > 0) {				
			evdev_root_x = samp.x;
			evdev_root_y = samp.y;
			evdev_button = LV_INDEV_STATE_PR;
		} else
			evdev_button = LV_INDEV_STATE_REL;				
				
		printf("x:%d,y:%d,value:%d\n", evdev_root_x, evdev_root_y, evdev_button);		
    }

    return NULL;    
}


				
static int ts_init(void)
{			
	char *tsdevice=NULL;	
			
	if ((tsdevice = getenv("TSLIB_TSDEVICE")) == NULL) {

		printf("TSLIB_TSDEVICE Error\n");				
							
#ifdef USE_INPUT_API
		tsdevice = strdup ("/dev/event0");
#else
		tsdevice = strdup ("/dev/touchscreen/ucb1x00");
#endif /* USE_INPUT_API */
	}			

	ts = ts_open (tsdevice, 0);

	if (!ts) {	
		printf("ts_open error\n");		
		perror (tsdevice);		
		return -1;
	} else {
		printf("ts_open Success\n");
	}

	if (ts_config(ts)) {
		printf("ts_config error\n");
		//perror("ts_config");
		return -1;
	} else {
		printf("ts_config Success\n");
	}

	CreateDetachedTask(&tslib_thread_id, tslib_thread, NULL);							

	return 1;				
}


#endif




/**
 * Initialize the evdev interface
 */
void evdev_init(void)
{	
#if USE_TSLIB == 0		

    evdev_fd = open(EVDEV_NAME, O_RDWR | O_NOCTTY | O_NDELAY);
    if(evdev_fd == -1) {
        perror("unable open evdev interface:");
        return;
    }

    fcntl(evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);


	ts_init();	

#else

	if(ts_init() == -1) {
		return;		
	}

#endif		

    evdev_root_x = 0;	
    evdev_root_y = 0;
    evdev_button = LV_INDEV_STATE_REL;
}


/**
 * reconfigure the device file for evdev
 * @param dev_name set the evdev device filename
 * @return true: the device file set complete
 *         false: the device file doesn't exist current system
 */
bool evdev_set_file(char* dev_name)
{	
     if(evdev_fd != -1) {
        close(evdev_fd);
     }
     evdev_fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);

     if(evdev_fd == -1) {
        perror("unable open evdev interface:");
        return false;
     }

     fcntl(evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);

     evdev_root_x = 0;
     evdev_root_y = 0;
     evdev_button = LV_INDEV_STATE_REL;

     return true;
}
/**
 * Get the current position and state of the evdev
 * @param data store the evdev data here
 * @return false: because the points are not buffered, so no more data to be read
 */

#if USE_TSLIB == 0

bool evdev_read(lv_indev_data_t * data)
{	
    struct input_event in;

    while(read(evdev_fd, &in, sizeof(struct input_event)) > 0) {
        if(in.type == EV_REL) {
            if(in.code == REL_X)
				#if EVDEV_SWAP_AXES
					evdev_root_y += in.value;
				#else
					evdev_root_x += in.value;
				#endif
            else if(in.code == REL_Y)
				#if EVDEV_SWAP_AXES
					evdev_root_x += in.value;
				#else
					evdev_root_y += in.value;
				#endif
        } else if(in.type == EV_ABS) {
            if(in.code == ABS_X)
				#if EVDEV_SWAP_AXES
					evdev_root_y = in.value;
				#else
					evdev_root_x = in.value;
				#endif
            else if(in.code == ABS_Y)
				#if EVDEV_SWAP_AXES
					evdev_root_x = in.value;
				#else
					evdev_root_y = in.value;
				#endif	
        } else if(in.type == EV_KEY) {
            if(in.code == BTN_MOUSE || in.code == BTN_TOUCH) {
                if(in.value == 0)	
                    evdev_button = LV_INDEV_STATE_REL;
                else if(in.value == 1)
                    evdev_button = LV_INDEV_STATE_PR;
            }
        }	
    }

    /*Store the collected data*/

#if EVDEV_SCALE			
    data->point.x = map(evdev_root_x, EVDEV_SCALE_HOR_RES2, EVDEV_SCALE_HOR_RES, 0, LV_HOR_RES);
    data->point.y = map(evdev_root_y, EVDEV_SCALE_VER_RES2, EVDEV_SCALE_VER_RES, 0, LV_VER_RES);
#else
#if EVDEV_CALIBRATE	
	data->point.x = map(evdev_root_x, EVDEV_HOR_MIN, EVDEV_HOR_MAX, 0, LV_HOR_RES);
	data->point.y = map(evdev_root_y, EVDEV_VER_MIN, EVDEV_VER_MAX, 0, LV_VER_RES);
#else
    data->point.x = evdev_root_x;
    data->point.y = evdev_root_y;
#endif
#endif

    data->state = evdev_button;

    if(data->point.x < 0)
      data->point.x = 0;
    if(data->point.y < 0)
      data->point.y = 0;
    if(data->point.x >= LV_HOR_RES)
      data->point.x = LV_HOR_RES - 1;
    if(data->point.y >= LV_VER_RES)
      data->point.y = LV_VER_RES - 1;				
	
	//printf("x:%d,y:%d\n", data->point.x, data->point.y);		

    return false;
}

#else

bool evdev_read(lv_indev_data_t * data)
{											
	data->point.x = evdev_root_x;
	data->point.y = evdev_root_y;
	data->state = evdev_button; 	
			
	if(data->point.x < 0)						
	  data->point.x = 0;
	if(data->point.y < 0)
	  data->point.y = 0;
	if(data->point.x >= LV_HOR_RES)
	  data->point.x = LV_HOR_RES - 1;		
	if(data->point.y >= LV_VER_RES)
	  data->point.y = LV_VER_RES - 1;											
				
	return false;
}



#endif


/**********************
 *   STATIC FUNCTIONS
 **********************/
int map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif

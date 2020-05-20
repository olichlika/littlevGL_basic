#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"	
#include "lv_examples/lv_apps/demo.h"		
#include "lv_examples/lv_tutorial/lv_tutorial_hello_world.h"
#include "lv_examples/lv_tutorial/lv_tutorial_porting.h"
#include "lv_examples/lv_tutorial/lv_tutorial_objects.h"
#include "lv_examples/lv_tutorial/lv_tutorial_styles.h"
#include "lv_examples/lv_tutorial/lv_tutorial_themes.h"		
#include "lv_examples/lv_tests/lv_test_theme_1.h"						
#include "lv_examples/lv_tests/lv_test_preload.h"								
#include "lv_examples/lv_tests/lv_test_task.h"					

int main(void)
{				
    /*LittlevGL init*/
    lv_init();		
								
    /*Linux frame buffer device init*/
    fbdev_init();					
	
    /*Add a display the LittlevGL sing the frame buffer driver*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);		
    disp_drv.disp_flush = fbdev_flush;      /*It flushes the internal graphical buffer to the frame buffer*/
    lv_disp_drv_register(&disp_drv);	
																								
    /* Add the mouse (or touchpad) as input device
     * Use the 'mouse' driver which reads the PC's mouse*/
	evdev_init();		
	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read = evdev_read;         /*This function will be called periodically (by the library) to get the mouse position and state*/
	lv_indev_drv_register(&indev_drv);			
						
    /* Tick init.			
     * You have to call 'lv_tick_handler()' in every milliseconds	
     * Create an SDL thread to do this*/				
	//CreateTask(&thread_id, tick_thread, NULL);							
															
    /*Create a Demo*/		
	//lv_tutorial_hello_world();											
   	//demo_create();																		
	//lv_tutorial_objects();				
	//lv_tutorial_themes();				
	//lv_turorial_porting();		
	//lv_tutorial_styles();			
	//lv_tutorial_themes(); 				
					
	lv_test_theme_1(lv_theme_mono_init(210, NULL));						
										
    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {										
        lv_tick_inc(1);																																						
        lv_task_handler();															
        usleep(100);					
    }

    return 0;
}

	

#define REALLY_SLOW_IO

#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <rtai_lxrt.h>
#include <rtai_sem.h>
#include <rtai_usi.h>
#include <rtai_fifos.h>
#include <time.h>
#include <sys/io.h>

RT_TASK *maint;
int squarethread;


static volatile int exec_end = 1;

// in nanoseconds (0.5 seconds).  Reduce this to something small, and check the 
// LED line with an oscilloscope.
#define PERIOD     500000000            


#define CONTROL 0x80003848
#define DATA 0x8000384c

#define BIT_LED  (1<<16)   // GPIO 16, red LED.

#define LED_LOW  outl(0, 0xcfc);    // start off with it high.
#define LED_HIGH  outl(BIT_LED, 0xcfc);



// Periodic sampling
static void *periodic_task(void *args) {
        RT_TASK *handler;
        RTIME period;
        int led_state=0;

        if (!(handler = rt_task_init_schmod(nam2num("ACHLR"), 
                 0, 0, 0, SCHED_FIFO, 0xF))) {
                printf("Unable to init timer handler task.\n");
                exit(1);
        }

        rt_allow_nonroot_hrt();
        // no swapping
        mlockall(MCL_CURRENT | MCL_FUTURE);

        rt_set_oneshot_mode();
        start_rt_timer(0);
        period = nano2count(PERIOD);
        rt_make_hard_real_time();
        exec_end = 0;
        rt_task_make_periodic(handler, rt_get_time() + period, period);
        rt_task_wait_period();

        while (!exec_end) {
                rt_task_wait_period();

                if (led_state) {
                  LED_LOW;
                  led_state = 0;                
                } else {
                  LED_HIGH;                
                  led_state = 1;
                }
        }
        
        stop_rt_timer();
        rt_make_soft_real_time();
        rt_task_delete(handler);
        return 0;
}

void cleanup(int sig) {
        exec_end = 1;
        return;
}


int main(void) {
        unsigned long tmp;

        // Catch some signals, because we need to clean up        
        signal(SIGTERM, cleanup);
        signal(SIGINT, cleanup);
        signal(SIGKILL, cleanup);
        
        if (!(maint = rt_task_init(nam2num("MAIN"), 1, 0, 0))) {
                printf("Cannot initialise main tastk.\n");
                exit(1);
        }

        // ask for permission to access the port from user-space
        ioperm(0xcf8, 8, 1);
        ioperm(0x80,1,1);  // we need this, for the port delays


        // Set lines as GPIO
        outl(CONTROL, 0xcf8);  // Set control register
        tmp = inl(0xcfc);
        tmp |= BIT_LED;
        outl(tmp, 0xcfc);     
        outl(DATA, 0xcf8);     // leave pointing to register data.
                    

        squarethread = rt_thread_create(periodic_task, NULL, 10000);

        // Wait here until start-up
        while (exec_end) {   
                usleep(100000);
        }
        printf("Task is now realtime!\n");
        while (!exec_end) {
          usleep(200000);
        }

        rt_thread_join(squarethread);
        rt_task_delete(maint);
        return 0;
}


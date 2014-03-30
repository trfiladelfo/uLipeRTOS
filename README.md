***************************************************************************************************************************
                                                    uLipeRTOS

                  A simple, 100% brazilian, preemptive real time kernel written from a scratch :)

    
      By:     Felipe Silva Neves
      Date:           2014/02/02
      
      About lincensing: You are free to copy, use and modify these source files in your application,
                        maitaning the authors name and original RTOS name credits on top of source
                        files.

***************************************************************************************************************************

About uLipeRTOS:

It´s a simple, preemptive real time kernel for microcontrollers, its on building, so this tag is the current first release.


About this version:

Since the RTOS is on development,
this tag comports the first version of the kernel only (services as mutexes, semaphores, timers are in development
and not included), it can run initially tested up 32 tasks with unique priority assigned, more tasks are possible
to run but are not tested and a some bug (tha will be corrected in next version) which initially not run the
Idle task. The current port is a generic ARM Cortex M4/M3/M0 CPUs for GNU GCC toolchain, its implemented currently
in a STM32F407 using the discovery board. More ports are planned but only will implemented when all OS core is
running bug free.



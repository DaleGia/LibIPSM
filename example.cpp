/**
 * @file example.c
 * A simple example writing and reading structs using threads.
 * Copyright (c) 2023 Dale Giancono All rights reserved.
 * 
 * @brief
 * TODO add me
 */
/*****************************************************************************/
/*INLCUDES                                                                   */
/*****************************************************************************/
#include "LibIPSM.hpp"

#include "unistd.h"
#include <cstring>
#include <iostream>
#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <sys/time.h>

/*****************************************************************************/
/*MACROS                                                                     */
/*****************************************************************************/

/*****************************************************************************/
/*OBJECT DECLARATIONS                                                        */
/*****************************************************************************/
struct exampleStruct
{
    long long time;
    char buffer[200];
};

/*****************************************************************************/
/*FUNCTION declarations                                                      */
/*****************************************************************************/
static void* readerFunction(void* arg);

/*****************************************************************************/
/*GLOBAL Data                                                                */
/*****************************************************************************/


/*****************************************************************************/
/* MAIN                                                                      */
/*****************************************************************************/
int main(int argc, const char **argv)
{
    int ret;
    pthread_t readerThread;

    InterProcessSharedMemory<exampleStruct> sharedMemory;  
    /** This removes and unlinks the shared memory and named semaphore
     * incase they were not properly dealt with previously.
     */
    ret = sharedMemory.initialise("/tmp", 0xFF);
    assert(0 == ret && "FAIL: unable to initialise shared memory");
    printf("Initialised shared memory\n");
    /* Start the reader thread*/
    pthread_create(&readerThread, NULL, readerFunction, NULL);

    /* open shared memory at specified path with specific key*/
    ret = sharedMemory.open("/tmp", 0xFF);
    assert(0 == ret && "FAIL: unable to open shared memory");
    printf("Opened shared memory for writing\n");

    for(int i = 0; i < 100; i++)
    {

        /* continually get the current time and use it to write to 
         shared memory 
         */
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);

        // Calculate the time in microseconds
        long long microsSinceEpoch = 
            (long long)currentTime.tv_sec * 1000000 + currentTime.tv_usec;

        struct exampleStruct data;
        data.time = microsSinceEpoch;
        memset(data.buffer, 0, sizeof(data.buffer));
        strcpy(data.buffer, "Hi!");

        /* Write the data to shared memory */
        sharedMemory.write(data);

        /* sleep for a while (20 milliseconds approx)*/
        usleep(20000);
    }
    printf("Finished writing\n");

    /* Wait for the reader to be finished and properly unlink the shared
     memory */
    pthread_join(readerThread, NULL);

    sharedMemory.close();
    sharedMemory.remove();
    return 0;
};

static void* readerFunction(void* arg)
{
    int ret;
    struct exampleStruct data;

    InterProcessSharedMemory<exampleStruct> sharedMemory;

    /* open the shared memory*/
    ret = sharedMemory.open("/tmp", 0xFF);
    assert(0 == ret && "FAIL: unable to open shared memory");
    printf("Opened shared memory for reading\n");

    sleep(1);

    for(int i = 0; i < 20; i++)
    {
        /* read the data*/
        sharedMemory.read(data);

        /** Do something with data here */
    
        usleep(100000);
    }
    printf("Finished reading\n");

    sharedMemory.close();
    
    pthread_exit(NULL);
}

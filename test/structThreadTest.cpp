/**
 * @file structThreadTest.c
 * A simple tests writing and reading structs using multiple threads.
 * Copyright (c) 2023 Dale Giancono All rights reserved.
 * 
 * @brief
 * TODO add me
 */
/*****************************************************************************/
/*INLCUDES                                                                   */
/*****************************************************************************/
#include "../InterProcessSharedMemory.hpp"

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
struct testStruct
{
    long long a;
    int b;
    float c;
    double d;
    uint16_t e;
    bool f;
    char buffer[200];
};

/*****************************************************************************/
/*FUNCTION declarations                                                      */
/*****************************************************************************/
static void* reader(void* arg);

/*****************************************************************************/
/*GLOBAL Data                                                                */
/*****************************************************************************/
InterProcessSharedMemory<testStruct> sharedMemory;

/*****************************************************************************/
/* MAIN                                                                      */
/*****************************************************************************/
int main(int argc, const char **argv)
{
    int ret;
    const int numberOfReaders = 10;
    for(int i = 0; i <  numberOfReaders; i++)
    {
        pthread_t thread;
        pthread_create(&thread, NULL, reader, NULL);
    }

    ret = sharedMemory.open(".", 0xFF);

    for(int i = 0; i < 100000; i++)
    {
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);

        // Calculate the time in microseconds
        long long microsSinceEpoch = 
            (long long)currentTime.tv_sec * 1000000 + currentTime.tv_usec;

        struct testStruct data;
        data.a = microsSinceEpoch;
        data.b = microsSinceEpoch;
        data.c = microsSinceEpoch;
        data.d = microsSinceEpoch;
        data.e = microsSinceEpoch;
        data.f = false;
        memset(data.buffer, 0, sizeof(data.buffer));
        strcpy(data.buffer, "Hi!");
        sharedMemory.write(data);
        usleep(100000);
    }

    sharedMemory.close();
    sharedMemory.remove();
    
    return 0;
};

static void* reader(void* arg)
{
    static int readercount = 0;

    int ret;
    struct testStruct data;
    ret = sharedMemory.open(".", 0xFF);
    
    assert(0 == ret && "FAIL: unable to open shared memory");
    int thisReaderCount = readercount++;
    for(int i = 0; i < 100000; i++)
    {

        sharedMemory.read(data);
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);

        // Calculate the time in microseconds
        long long microsSinceEpoch = (long long)currentTime.tv_sec * 1000000 + currentTime.tv_usec;

        printf(
            "reader %d: %llu %d %f %lf %u %d %s\n", 
            thisReaderCount,
            data.a, 
            data.b, 
            data.c, 
            data.d, 
            data.e, 
            data.f,
            data.buffer);
        usleep(100000);
    }

    sharedMemory.close();
    pthread_exit(NULL);
}

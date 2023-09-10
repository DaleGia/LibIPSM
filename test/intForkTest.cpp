/**
 * @file intTestFork.c
 * Tests writing and reading integers using a parent and child process.
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

/*****************************************************************************/
/*MACROS                                                                     */
/*****************************************************************************/

/*****************************************************************************/
/*OBJECT DECLARATIONS                                                        */
/*****************************************************************************/

/*****************************************************************************/
/*FUNCTION declarations                                                      */
/*****************************************************************************/
void writerOne();
void reader();
void readerTwo();
void readerThree();

/*****************************************************************************/
/*GLOBAL Data                                                                */
/*****************************************************************************/
InterProcessSharedMemory<int> sharedMemory;

/*****************************************************************************/
/* MAIN                                                                      */
/*****************************************************************************/
int main(int argc, const char **argv)
{
    int writeData;
    int readData;
    int ret;

    pid_t parentPid;
    pid_t childPid1;
    pid_t childPid2;
    pid_t childPid3;

    parentPid = getpid();
    childPid1 = fork();
    assert((-1 != childPid1) && "Fork 1 FAILED");
    if(0 == childPid1)
    {
        reader();
    }
    else
    {
        writerOne();
        kill(childPid1, SIGKILL);
    }


};

void writerOne()
{
    int ret;
    ret = sharedMemory.open(".", 0);
    
    assert(0 == ret && "FAIL: unable to open shared memory");

    for(int i = INT_MIN; i < INT_MAX; i++)
    {
        sharedMemory.write(i);
    }

    sharedMemory.close();
    sharedMemory.remove();


}

void reader()
{
    int ret;
    int i;
    ret = sharedMemory.open(".", 0);
    
    assert(0 == ret && "FAIL: unable to open shared memory");
    while(1)
    {
        sharedMemory.read(i);
        std::cout << "reader one: " << i << std::endl;
        usleep(100000);
    }

    sharedMemory.close();
}

/**
 * @file Main.c
 * Copyright (c) 2023 Dale Giancono All rights reserved.
 * 
 * @brief
 * TODO add me
 */
/*****************************************************************************/
/*INLCUDES                                                                   */
/*****************************************************************************/
#include "InterProcessCommunication.hpp"

#include "unistd.h"

/*****************************************************************************/
/*MACROS                                                                     */
/*****************************************************************************/

/*****************************************************************************/
/*OBJECT DECLARATIONS                                                        */
/*****************************************************************************/

/*****************************************************************************/
/*FUNCTION declarations                                                      */
/*****************************************************************************/

/*****************************************************************************/
/*GLOBAL Data                                                                */
/*****************************************************************************/

/*****************************************************************************/
/* MAIN                                                                      */
/*****************************************************************************/
int main(int argc, const char **argv)
{
    InterProcessCommunication<int> comms;
    int writeData;
    int readData;
    int ret;
    ret = comms.open("/tmp");
    if(0 != ret)
    {
        perror(strerror(ret));
        return -1;
    }
    else
    {
        printf("open success\n");
    }

    writeData = 10;
    ret = comms.write(&writeData);
    if(0 != ret)
    {
        perror(strerror(ret));
    }

    ret = comms.read(&readData);
    if(0 != ret)
    {
        perror(strerror(ret));
    }
    else
    {
        printf("%d\n", readData);
    }

    writeData = 20;
    comms.write(&writeData);
    if(0 != ret)
    {
        perror(strerror(ret));
    }
    comms.read(&readData);
    if(0 != ret)
    {
        perror(strerror(ret));
    }
    else
    {
        printf("%d\n", readData);
    }
    
    writeData = 30;
    comms.write(&writeData);
    if(0 != ret)
    {
        perror(strerror(ret));
    }
    comms.read(&readData);
    if(0 != ret)
    {
        perror(strerror(ret));
    }
    else
    {
        printf("%d\n", readData);
    }
    
    comms.close();
};
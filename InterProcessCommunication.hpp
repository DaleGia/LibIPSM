/*
  InterProcessCommunication.h
  Copyright (c) 2023 Dale Giancono. All rights reserved..

  This class implements a way to represent image data.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*****************************************************************************/
/*INLCUDE GUARD                                                              */
/*****************************************************************************/
#ifndef INTERPROCESSCOMMUNICATION_H_
#define INTERPROCESSCOMMUNICATION_H_

/*****************************************************************************/
/*INLCUDES                                                                   */
/*****************************************************************************/
#include <sys/shm.h>
#include <fcntl.h>     
#include <sys/stat.h>    
#include <semaphore.h>
#include <cstring>

#include <string>

/*****************************************************************************/
/*MACROS                                                                     */
/*****************************************************************************/

/*****************************************************************************/
/*GLOBAL Data                                                                */
/*****************************************************************************/

/*****************************************************************************/
/*PRIVATE Data                                                                */
/*****************************************************************************/
static const int interProcessCommunicationKey = 0xFF;

/*****************************************************************************/
/*CLASS DECLARATION                                                          */
/*****************************************************************************/
enum InterProcessCommunicationStatus
{
    OK = 0,
    GENERIC_ERROR = 1,
    IPC_KEY_FAILURE = 2,
    SHM_BAD_ACCESS = 3,
    SHM_INVALID_SIZE = 4,
    SHM_NO_MEMORY = 5,
    SHM_MAX_IDENTIFIERS = 6,
    SHM_INVALID_ID = 7,
    SHM_MAX_ATTACHMENTS = 8
};

template<class T> class InterProcessCommunication
{
    public:
        int open(std::string filepath);
        int write(T *data);
        int read(T *data);
        int close(void);

    private:
        std::string filepath;
        std::string semFilepath;
        std::string semReadFilepath;

        key_t key;
        int shmId;
        sem_t *semaphore;
        T* sharedMemory;
};

template<class T>  int InterProcessCommunication<T>::open(std::string filepath)
{
    this->filepath = filepath;
    this->semFilepath = this->filepath + "_sem";

    this->key = ftok(filepath.c_str(), interProcessCommunicationKey);
    if(-1 == this->key)
    {
        return -1;  
    }   

    this->shmId = shmget(this->key, sizeof(T), IPC_CREAT | 0666);
    if(-1 == this->shmId ) 
    {
        return errno;
    }

    this->sharedMemory = (T*)shmat(this->shmId, NULL, 0);
    if(-1 == (long int)this->sharedMemory) 
    {
        return errno;
    }

    this->semaphore = sem_open(this->semFilepath.c_str(), O_CREAT, 0666, 1);
    if(SEM_FAILED == this->semaphore) 
    {
        return errno;
    }

    return 0;
}

template<class T>  int InterProcessCommunication<T>::write(T *data)
{
    int ret;

    ret = sem_wait(this->semaphore); // Wait for the semaphore to be available

    if(-1 == ret)
    {
        return errno;
    }

    memcpy(this->sharedMemory, (void*)data, sizeof(T));
    
    ret = sem_post(this->semaphore); // Release the semaphore
    if(-1 == ret)
    {
        return errno;
    }

    return 0;
}

template<class T>  int InterProcessCommunication<T>::read(T *buffer)
{
    int ret;
    ret = sem_wait(this->semaphore); // Wait for the semaphore to be available
    if(-1 == ret)
    {
        return errno;
    }

    memcpy(buffer, (void*)this->sharedMemory, sizeof(T));

    ret = sem_post(this->semaphore); // Release the semaphore
    if(-1 == ret)
    {
        return errno;
    }

    return 0;
}

template<class T>  int InterProcessCommunication<T>::close(void)
{
    int ret;
    ret = shmdt(this->sharedMemory);
    if(-1 == ret)
    {
        return errno;
    }

    ret = sem_close(this->semaphore); 
    if(-1 == ret)
    {
        return errno;
    }

    ret = sem_unlink(this->semFilepath.c_str());
    if(-1 == ret)
    {
        return errno;
    }

    return 0;
}

#endif /* INTERPROCESSCOMMUNICATION_H_ */
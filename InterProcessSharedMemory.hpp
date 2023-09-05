/*
  InterProcessSharedMemory.h
  Copyright (c) 2023 Dale Giancono. All rights reserved..

  This class implements a way to read and write to shared memory
  accross different programs.

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

#ifndef INTERPROCESSSHAREDMEMORY_H_
#define INTERPROCESSSHAREDMEMORY_H_

#include <sys/shm.h>
#include <fcntl.h>     
#include <semaphore.h>
#include <string>

/** This class implements a way to read and write to shared memory accross 
 * different programs. 
*/
template<class T> class InterProcessSharedMemory
{
    public:
        /**
         *  @brief Opens a shared memory value to read or write from 
         *   
         *  @param filepath filepath specifier for the shared memory. (see ftok)
         *  @param key a key value to access the shared memory (see ftok).
         *  @return 0 if successful, otherwise errno value
        */
        int open(std::string filepath, const int key);

        /**
         * @brief Writes to the shared memory
         * 
         * @param data Data to write to shared memory
         * @return 0 if successful, otherwise errno value
         */
        int write(T &data);
        
        /**
         * @brief Reads the shared memory
         * 
         * @param data Buffer to write the data to
         * @return 0 if successful, otherwise errno value
         */
        int read(T &data);

        /**
         * @brief Detaches the shared memory
         * 
         * @return 0 if successful, otherwise errno value
         */
        int close(void);

        /**
         * @brief Marks the shared memory segment and named semaphore for 
         * deletion.
         * 
         * @return 0 if successful, otherwise errno value
         */
        int remove(void);

    private:
        /** Filepath for the shared memory segment */
        std::string filepath;
        /** Filepath for the named semaphore */
        std::string semFilepath;
        /** Key for the shared memory segment */
        key_t key;
        /** Id for the shared memory segment */
        int shmId;
        /** Named semaphore opened with \sa open */
        sem_t *semaphore;
        /** Shared memory opened with \sa */
        T* sharedMemory;
};

template<class T>  int InterProcessSharedMemory<T>::open(
    std::string filepath, 
    const int key)
{
    /* Gets the filepath and creates the semaphore filepath to open */
    this->filepath = filepath;
    this->semFilepath = this->filepath + "_sem";

    /* Gets the key for the shared memory segment */
    this->key = ftok(filepath.c_str(), key);
    if(-1 == this->key)
    {
        return -1;  
    }   

    /* Gets the id for the shared memory segment. If the shared memory segment
     * does not yet exist, it is created.
     */
    this->shmId = shmget(this->key, sizeof(T), IPC_CREAT | 0666);
    if(-1 == this->shmId ) 
    {
        return errno;
    }
    /* Gets the pointer to the shared memory */
    this->sharedMemory = (T*)shmat(this->shmId, NULL, 0);
    if(-1 == (long int)this->sharedMemory) 
    {
        return errno;
    }

    /* Opens the named semaphore that protects the shared memory segment.
     * if the named semaphore does not exist, it creates it.
     */
    this->semaphore = sem_open(this->semFilepath.c_str(), O_CREAT, 0666, 1);
    if(SEM_FAILED == this->semaphore) 
    {
        return errno;
    }

    return 0;
}

template<class T>  int InterProcessSharedMemory<T>::write(T &data)
{
    int ret;

    /* Waits for the shared memory segment to be available via named semaphore 
     */
    ret = sem_wait(this->semaphore);
    if(-1 == ret)
    {
        return errno;
    }
    
    *this->sharedMemory = data;

    ret = sem_post(this->semaphore);
    
    if(-1 == ret)
    {
        return errno;
    }

    return 0;
}

template<class T>  int InterProcessSharedMemory<T>::read(T &buffer)
{
    int ret;
    /* Waits for the shared memory segment to be available via named semaphore 
     */
    ret = sem_wait(this->semaphore); 
    if(-1 == ret)
    {
        return errno;
    }

    buffer = *this->sharedMemory;

    ret = sem_post(this->semaphore);
    if(-1 == ret)
    {
        return errno;
    }

    return 0;
}

template<class T>  int InterProcessSharedMemory<T>::close(void)
{
    int ret;
    /* Detaches the shared memory segment. */
    ret = shmdt(this->sharedMemory);
    if(-1 == ret)
    {
        return errno;
    }

    /* Closes the named semaphore*/
    ret = sem_close(this->semaphore); 
    if(-1 == ret)
    {
        return errno;
    }


    return 0;
}


template<class T>  int InterProcessSharedMemory<T>::remove(void)
{
    int ret;
    /** marks shared memory segment for deletion*/
    ret = shmctl(this->shmId, IPC_RMID, NULL);
    if(-1 == ret)
    {
        return errno;
    }

    /** Removes the named semaphore*/
    ret = sem_unlink(this->semFilepath.c_str());
    if(-1 == ret)
    {
        return errno;
    }

    return 0;
}

#endif /* INTERPROCESSSHAREDMEMORY_H_ */
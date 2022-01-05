/*
 * mutex.c
 * Copyright (C) 2012 xent
 * Project is distributed under the terms of the MIT License
 */

#include <osw/mutex.h>
#include <time.h>
/*----------------------------------------------------------------------------*/
enum Result mutexInit(struct Mutex *) __attribute__((weak));
/*----------------------------------------------------------------------------*/
enum Result mutexInit(struct Mutex *mutex)
{
  return pthread_mutex_init(&mutex->handle, 0) == 0 ? E_OK : E_ERROR;
}
/*----------------------------------------------------------------------------*/
void mutexDeinit(struct Mutex *mutex)
{
  pthread_mutex_destroy(&mutex->handle);
}
/*----------------------------------------------------------------------------*/
bool mutexTryLock(struct Mutex *mutex, unsigned int interval)
{
  int res;

  if (interval)
  {
    struct timespec timestamp;

    clock_gettime(CLOCK_REALTIME, &timestamp);
    timestamp.tv_sec += interval / 1000;
    timestamp.tv_nsec += (interval % 1000) * 1000000;

    if (timestamp.tv_nsec >= 1000000000)
    {
      timestamp.tv_nsec -= 1000000000;
      ++timestamp.tv_sec;
    }

    res = pthread_mutex_timedlock(&mutex->handle, &timestamp);
  }
  else
  {
    res = pthread_mutex_trylock(&mutex->handle);
  }

  return res == 0;
}

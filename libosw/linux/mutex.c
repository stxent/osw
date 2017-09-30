/*
 * mutex.c
 * Copyright (C) 2012 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#include <time.h>
#include <osw/mutex.h>
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

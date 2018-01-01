/*
 * semaphore.c
 * Copyright (C) 2014 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#include <time.h>
#include <osw/semaphore.h>
/*----------------------------------------------------------------------------*/
bool semTryWait(struct Semaphore *semaphore, unsigned int timeout)
{
  int res;

  if (timeout)
  {
    struct timespec timestamp;

    clock_gettime(CLOCK_REALTIME, &timestamp);
    timestamp.tv_sec += timeout / 1000;
    timestamp.tv_nsec += (timeout % 1000) * 1000000;

    if (timestamp.tv_nsec >= 1000000000)
    {
      timestamp.tv_nsec -= 1000000000;
      ++timestamp.tv_sec;
    }

    res = sem_timedwait(&semaphore->handle, &timestamp);
  }
  else
  {
    res = sem_trywait(&semaphore->handle);
  }

  return res == 0;
}

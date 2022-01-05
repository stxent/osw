/*
 * semaphore.c
 * Copyright (C) 2014 xent
 * Project is distributed under the terms of the MIT License
 */

#include <osw/semaphore.h>
#include <time.h>
/*----------------------------------------------------------------------------*/
enum Result semInit(struct Semaphore *, int) __attribute__((weak));
/*----------------------------------------------------------------------------*/
enum Result semInit(struct Semaphore *sem, int value)
{
  return sem_init(&sem->handle, 0, value) == 0 ? E_OK : E_ERROR;
}
/*----------------------------------------------------------------------------*/
void semDeinit(struct Semaphore *sem)
{
  sem_destroy(&sem->handle);
}
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

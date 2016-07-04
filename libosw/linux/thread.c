/*
 * thread.c
 * Copyright (C) 2014 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#define _POSIX_C_SOURCE 200809L
#define _BSD_SOURCE

#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <osw/thread.h>
/*----------------------------------------------------------------------------*/
struct ThreadPrivateData
{
  pthread_t handle;

  void *argument;
  void (*run)(void *);

  unsigned int size;
  bool running;
};
/*----------------------------------------------------------------------------*/
void *threadLauncher(void *object)
{
  struct Thread * const thread = object;
  struct ThreadPrivateData * const data = thread->handle;

  /* Configure the thread */
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);

  /* Run user defined function */
  data->running = true;
  data->run(data->argument);

  return 0;
}
/*----------------------------------------------------------------------------*/
enum result threadInit(struct Thread *thread, unsigned int size,
    short int priority __attribute__((unused)), void (*run)(void *),
    void *argument)
{
  struct ThreadPrivateData * const data =
      malloc(sizeof(struct ThreadPrivateData));

  if (!data)
    return E_MEMORY;

  data->argument = argument;
  data->run = run;
  data->size = size;
  data->running = false;
  thread->handle = data;

  return E_OK;
}
/*----------------------------------------------------------------------------*/
void threadDeinit(struct Thread *thread)
{
  threadTerminate(thread);
  free(thread->handle);
}
/*----------------------------------------------------------------------------*/
void threadOnTerminateCallback(struct Thread *thread, void (*callback)(void *),
    void *argument)
{
  thread->onTerminateArgument = argument;
  thread->onTerminateCallback = callback;
}
/*----------------------------------------------------------------------------*/
enum result threadStart(struct Thread *thread)
{
  struct ThreadPrivateData * const data = thread->handle;

  /* Check whether the thread is already started */
  if (data->running)
    return E_BUSY;

  pthread_attr_t attributes;
  int result;

  result = pthread_attr_init(&attributes);
  if (result != 0)
    return E_ERROR;

  /* Set a new stack size */
  result = pthread_attr_setstacksize(&attributes,
      PTHREAD_STACK_MIN + data->size);
  if (result != 0)
    return E_VALUE;

  /* Create and start a new thread */
  result = pthread_create(&data->handle, &attributes, threadLauncher, thread);
  pthread_attr_destroy(&attributes);

  if (result != 0)
    return E_ERROR;

  return E_OK;
}
/*----------------------------------------------------------------------------*/
void threadTerminate(struct Thread *thread)
{
  struct ThreadPrivateData * const data = thread->handle;

  if (!data->running)
    return;

  if (thread->onTerminateCallback)
    thread->onTerminateCallback(thread->onTerminateArgument);
  else
    pthread_cancel(data->handle);

  pthread_join(data->handle, 0);
  data->running = false;
}
/*----------------------------------------------------------------------------*/
void msleep(unsigned int interval)
{
  const struct timespec timestamp = {
      .tv_sec = interval / 1000,
      .tv_nsec = (interval % 1000) * 1000000
  };

  nanosleep(&timestamp, 0);
}

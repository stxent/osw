/*
 * thread.c
 * Copyright (C) 2014 xent
 * Project is distributed under the terms of the MIT License
 */

#include <limits.h>
#include <osw/thread.h>
/*----------------------------------------------------------------------------*/
static void *threadLauncher(void *object)
{
  struct Thread * const thread = object;

  /* Configure the thread */
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);

  /* Run user defined function */
  thread->running = true;
  thread->function(thread->functionArgument);

  return 0;
}
/*----------------------------------------------------------------------------*/
void threadInit(struct Thread *thread, size_t size,
    int priority __attribute__((unused)), void (*function)(void *),
    void *functionArgument)
{
  thread->functionArgument = functionArgument;
  thread->function = function;
  thread->stackSize = size;
  thread->running = false;
}
/*----------------------------------------------------------------------------*/
void threadDeinit(struct Thread *thread)
{
  threadTerminate(thread);
}
/*----------------------------------------------------------------------------*/
void threadOnTerminateCallback(struct Thread *thread, void (*callback)(void *),
    void *callbackArgument)
{
  thread->onTerminateArgument = callbackArgument;
  thread->onTerminateCallback = callback;
}
/*----------------------------------------------------------------------------*/
enum Result threadStart(struct Thread *thread)
{
  /* Check whether the thread is already started */
  if (thread->running)
    return E_BUSY;

  pthread_attr_t attributes;
  int result;

  result = pthread_attr_init(&attributes);
  if (result != 0)
    return E_ERROR;

  /* Set a new stack size */
  result = pthread_attr_setstacksize(&attributes,
      PTHREAD_STACK_MIN + thread->stackSize);
  if (result != 0)
    return E_VALUE;

  /* Create and start a new thread */
  result = pthread_create(&thread->handle, &attributes, threadLauncher, thread);
  pthread_attr_destroy(&attributes);

  return result == 0 ? E_OK : E_ERROR;
}
/*----------------------------------------------------------------------------*/
void threadTerminate(struct Thread *thread)
{
  if (!thread->running)
    return;

  if (thread->onTerminateCallback)
    thread->onTerminateCallback(thread->onTerminateArgument);
  else
    pthread_cancel(thread->handle);

  pthread_join(thread->handle, 0);
  thread->running = false;
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

/*
 * osw/linux/thread.h
 * Copyright (C) 2017 xent
 * Project is distributed under the terms of the MIT License
 */

#ifndef OSW_LINUX_THREAD_H_
#define OSW_LINUX_THREAD_H_
/*----------------------------------------------------------------------------*/
#include <pthread.h>
#include <stdbool.h>
#include <xcore/helpers.h>
#include <xcore/error.h>
/*----------------------------------------------------------------------------*/
struct Thread
{
  pthread_t handle;

  void (*onTerminateCallback)(void *);
  void *onTerminateArgument;

  void (*function)(void *);
  void *functionArgument;

  size_t stackSize;
  bool running;
};
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

void threadInit(struct Thread *, size_t, int, void (*)(void *), void *);
void threadDeinit(struct Thread *);
void threadOnTerminateCallback(struct Thread *, void (*)(void *), void *);
enum Result threadStart(struct Thread *);
void threadTerminate(struct Thread *);
void msleep(unsigned int);

END_DECLS
/*----------------------------------------------------------------------------*/
#endif /* OSW_LINUX_THREAD_H_ */

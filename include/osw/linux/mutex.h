/*
 * osw/linux/mutex.h
 * Copyright (C) 2017 xent
 * Project is distributed under the terms of the MIT License
 */

#ifndef OSW_LINUX_MUTEX_H_
#define OSW_LINUX_MUTEX_H_
/*----------------------------------------------------------------------------*/
#include <xcore/helpers.h>
#include <xcore/error.h>
#include <pthread.h>
#include <stdbool.h>
/*----------------------------------------------------------------------------*/
struct Mutex
{
  pthread_mutex_t handle;
};
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

enum Result mutexInit(struct Mutex *);
void mutexDeinit(struct Mutex *);
bool mutexTryLock(struct Mutex *, unsigned int);

END_DECLS
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

static inline void mutexLock(struct Mutex *mutex)
{
  pthread_mutex_lock(&mutex->handle);
}

static inline void mutexUnlock(struct Mutex *mutex)
{
  pthread_mutex_unlock(&mutex->handle);
}

END_DECLS
/*----------------------------------------------------------------------------*/
#endif /* OSW_LINUX_MUTEX_H_ */

/*
 * osw/linux/mutex.h
 * Copyright (C) 2017 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#ifndef OSW_LINUX_MUTEX_H_
#define OSW_LINUX_MUTEX_H_
/*----------------------------------------------------------------------------*/
#include <pthread.h>
#include <stdbool.h>
#include <xcore/helpers.h>
#include <xcore/error.h>
/*----------------------------------------------------------------------------*/
struct Mutex
{
  pthread_mutex_t handle;
};
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

bool mutexTryLock(struct Mutex *, unsigned int);

END_DECLS
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

static inline enum Result mutexInit(struct Mutex *mutex)
{
  return pthread_mutex_init(&mutex->handle, 0) == 0 ? E_OK : E_ERROR;
}

static inline void mutexDeinit(struct Mutex *mutex)
{
  pthread_mutex_destroy(&mutex->handle);
}

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

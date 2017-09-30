/*
 * osw/linux/semaphore.h
 * Copyright (C) 2017 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#ifndef OSW_LINUX_SEMAPHORE_H_
#define OSW_LINUX_SEMAPHORE_H_
/*----------------------------------------------------------------------------*/
#include <semaphore.h>
#include <stdbool.h>
#include <xcore/helpers.h>
#include <xcore/error.h>
/*----------------------------------------------------------------------------*/
struct Semaphore
{
  sem_t handle;
};
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

bool semTryWait(struct Semaphore *, unsigned int);

END_DECLS
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

static inline enum Result semInit(struct Semaphore *semaphore, int value)
{
  return sem_init(&semaphore->handle, 0, value) == 0 ? E_OK : E_ERROR;
}

static inline void semDeinit(struct Semaphore *semaphore)
{
  sem_destroy(&semaphore->handle);
}

static inline void semPost(struct Semaphore *semaphore)
{
  sem_post(&semaphore->handle);
}

static inline int semValue(struct Semaphore *semaphore)
{
  int value = 1;

  sem_getvalue(&semaphore->handle, &value);
  return value;
}

static inline void semWait(struct Semaphore *semaphore)
{
  sem_wait(&semaphore->handle);
}

END_DECLS
/*----------------------------------------------------------------------------*/
#endif /* OSW_LINUX_SEMAPHORE_H_ */

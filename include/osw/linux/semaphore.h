/*
 * osw/linux/semaphore.h
 * Copyright (C) 2017 xent
 * Project is distributed under the terms of the MIT License
 */

#ifndef OSW_LINUX_SEMAPHORE_H_
#define OSW_LINUX_SEMAPHORE_H_
/*----------------------------------------------------------------------------*/
#include <assert.h>
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

static inline enum Result semInit(struct Semaphore *sem, int value)
{
  return sem_init(&sem->handle, 0, value) == 0 ? E_OK : E_ERROR;
}

static inline void semDeinit(struct Semaphore *sem)
{
  sem_destroy(&sem->handle);
}

static inline void semPost(struct Semaphore *sem)
{
  sem_post(&sem->handle);
}

static inline int semValue(struct Semaphore *sem)
{
  int value;

  const int result = sem_getvalue(&sem->handle, &value);
  assert(result == 0);
  (void)result;

  return value;
}

static inline void semWait(struct Semaphore *sem)
{
  sem_wait(&sem->handle);
}

END_DECLS
/*----------------------------------------------------------------------------*/
#endif /* OSW_LINUX_SEMAPHORE_H_ */

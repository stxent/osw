/*
 * osw/linux/semaphore.h
 * Copyright (C) 2017 xent
 * Project is distributed under the terms of the MIT License
 */

#ifndef OSW_LINUX_SEMAPHORE_H_
#define OSW_LINUX_SEMAPHORE_H_
/*----------------------------------------------------------------------------*/
#include <xcore/helpers.h>
#include <xcore/error.h>
#include <assert.h>
#include <semaphore.h>
#include <stdbool.h>
/*----------------------------------------------------------------------------*/
struct Semaphore
{
  sem_t handle;
};
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

enum Result semInit(struct Semaphore *, int);
void semDeinit(struct Semaphore *);
bool semTryWait(struct Semaphore *, unsigned int);

END_DECLS
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

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

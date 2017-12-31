/*
 * osw/generic/semaphore.h
 * Copyright (C) 2017 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#ifndef OSW_GENERIC_SEMAPHORE_H_
#define OSW_GENERIC_SEMAPHORE_H_
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <xcore/helpers.h>
#include <xcore/error.h>
/*----------------------------------------------------------------------------*/
struct Semaphore
{
  int value;
};
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

static inline enum Result semInit(struct Semaphore *sem, int value)
{
  sem->value = value;
  return E_OK;
}

static inline void semDeinit(struct Semaphore *sem
    __attribute__((unused)))
{
}

static inline void semPost(struct Semaphore *sem)
{
  __atomic_fetch_add(&sem->value, 1, __ATOMIC_SEQ_CST);
}

static inline bool semTryWait(struct Semaphore *sem __attribute__((unused)),
    unsigned int value __attribute__((unused)))
{
  while (1); /* TODO Unimplemented */

  return false;
}

static inline int semValue(struct Semaphore *sem)
{
  int value;

  __atomic_load(&sem->value, &value, __ATOMIC_SEQ_CST);
  return value;
}

static inline void semWait(struct Semaphore *sem)
{
  while (semValue(sem) <= 0);
  __atomic_fetch_sub(&sem->value, 1, __ATOMIC_SEQ_CST);
}

END_DECLS
/*----------------------------------------------------------------------------*/
#endif /* OSW_GENERIC_SEMAPHORE_H_ */
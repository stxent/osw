/*
 * osw/generic/semaphore.h
 * Copyright (C) 2017 xent
 * Project is distributed under the terms of the MIT License
 */

#ifndef OSW_GENERIC_SEMAPHORE_H_
#define OSW_GENERIC_SEMAPHORE_H_
/*----------------------------------------------------------------------------*/
#include <xcore/helpers.h>
#include <xcore/error.h>
#include <stdbool.h>
/*----------------------------------------------------------------------------*/
struct Semaphore
{
  int value;
};
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

enum Result semInit(struct Semaphore *, int);

END_DECLS
/*----------------------------------------------------------------------------*/
BEGIN_DECLS

static inline void semDeinit(struct Semaphore *sem __attribute__((unused)))
{
}

static inline void semPost(struct Semaphore *sem)
{
  __atomic_fetch_add(&sem->value, 1, __ATOMIC_SEQ_CST);
}

static inline bool semTryWait(struct Semaphore *sem,
    unsigned int timeout)
{
  bool acquired = true;

  if (!timeout)
  {
    int value;

    do
    {
      __atomic_load(&sem->value, &value, __ATOMIC_SEQ_CST);

      if (value <= 0)
      {
        acquired = false;
        break;
      }
    }
    while (!__atomic_compare_exchange_n(&sem->value, &value,
        value - 1, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));
  }
  else
  {
    /* TODO Unimplemented */
    while (1);
  }

  return acquired;
}

static inline int semValue(struct Semaphore *sem)
{
  int value;
  __atomic_load(&sem->value, &value, __ATOMIC_SEQ_CST);
  return value;
}

static inline void semWait(struct Semaphore *sem)
{
  int value;

  do
  {
    __atomic_load(&sem->value, &value, __ATOMIC_SEQ_CST);
  }
  while (value <= 0 || !__atomic_compare_exchange_n(&sem->value, &value,
      value - 1, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));
}

END_DECLS
/*----------------------------------------------------------------------------*/
#endif /* OSW_GENERIC_SEMAPHORE_H_ */

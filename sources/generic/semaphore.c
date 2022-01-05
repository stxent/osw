/*
 * semaphore.c
 * Copyright (C) 2014 xent
 * Project is distributed under the terms of the MIT License
 */

#include <osw/semaphore.h>
/*----------------------------------------------------------------------------*/
enum Result semInit(struct Semaphore *, int) __attribute__((weak));
/*----------------------------------------------------------------------------*/
enum Result semInit(struct Semaphore *sem, int value)
{
  sem->value = value;
  return E_OK;
}

/*
 * libosw/semaphore.h
 * Copyright (C) 2014 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#ifndef LIBOSW_SEMAPHORE_H_
#define LIBOSW_SEMAPHORE_H_
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <error.h>
/*----------------------------------------------------------------------------*/
struct Semaphore
{
  void *handle;
};
/*----------------------------------------------------------------------------*/
enum result semInit(struct Semaphore *, int);
void semDeinit(struct Semaphore *);
/*----------------------------------------------------------------------------*/
void semPost(struct Semaphore *);
bool semTryWait(struct Semaphore *, unsigned int);
int semValue(struct Semaphore *);
void semWait(struct Semaphore *);
/*----------------------------------------------------------------------------*/
#endif /* LIBOSW_SEMAPHORE_H_ */

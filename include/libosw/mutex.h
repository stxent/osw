/*
 * libosw/mutex.h
 * Copyright (C) 2012 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#ifndef LIBOSW_MUTEX_H_
#define LIBOSW_MUTEX_H_
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <error.h>
/*----------------------------------------------------------------------------*/
struct Mutex
{
  void *handle;
};
/*----------------------------------------------------------------------------*/
enum result mutexInit(struct Mutex *);
void mutexDeinit(struct Mutex *);
/*----------------------------------------------------------------------------*/
void mutexLock(struct Mutex *);
bool mutexTryLock(struct Mutex *, unsigned int);
void mutexUnlock(struct Mutex *);
/*----------------------------------------------------------------------------*/
#endif /* LIBOSW_MUTEX_H_ */

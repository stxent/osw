/*
 * osw/mutex.h
 * Copyright (C) 2012 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#ifndef OSW_MUTEX_H_
#define OSW_MUTEX_H_
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <xcore/error.h>
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
#endif /* OSW_MUTEX_H_ */

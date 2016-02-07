/*
 * libosw/helpers/udp_stream.h
 * Copyright (C) 2013 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#ifndef LIBOSW_HELPERS_UDP_STREAM_H_
#define LIBOSW_HELPERS_UDP_STREAM_H_
/*----------------------------------------------------------------------------*/
#include <containers/byte_queue.h>
#include <interface.h>
#include <libosw/mutex.h>
#include <libosw/semaphore.h>
#include <libosw/thread.h>
/*----------------------------------------------------------------------------*/
extern const struct InterfaceClass * const UdpStream;
/*----------------------------------------------------------------------------*/
struct UdpStreamConfig
{
  const char *clientAddress;
  uint16_t clientPort;
  uint16_t serverPort;
};
/*----------------------------------------------------------------------------*/
struct UdpStream
{
  struct Interface parent;

  void (*callback)(void *);
  void *callbackArgument;

  struct ByteQueue rxQueue;

  int clientSocket;
  int serverSocket;

  struct Mutex mutex;
  struct Semaphore semaphore;
  struct Thread thread;

  bool terminated;
};
/*----------------------------------------------------------------------------*/
#endif /* LIBOSW_HELPERS_UDP_STREAM_H_ */

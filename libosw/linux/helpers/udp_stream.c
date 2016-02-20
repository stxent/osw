/*
 * udp_stream.c
 * Copyright (C) 2013 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#include <arpa/inet.h>
#include <poll.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <libosw/helpers/udp_stream.h>
/*----------------------------------------------------------------------------*/
#ifdef CONFIG_DEBUG
#include <stdio.h>
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) do {} while (0)
#endif
/*----------------------------------------------------------------------------*/
#define BUFFER_SIZE     1536
#define QUEUE_SIZE      2048
#define THREAD_PRIORITY 0
#define THREAD_SIZE     (1024 + BUFFER_SIZE)
/*----------------------------------------------------------------------------*/
enum cleanup
{
  CLEANUP_ALL,
  CLEANUP_THREAD,
  CLEANUP_SOCKETS,
  CLEANUP_MUTEX,
  CLEANUP_SEMAPHORE,
  CLEANUP_QUEUE
};
/*----------------------------------------------------------------------------*/
static void cleanup(struct UdpStream *, enum cleanup);
static enum result setupSockets(struct UdpStream *,
    const struct UdpStreamConfig *);
static void streamTerminateHandler(void *);
static void streamThread(void *);
/*----------------------------------------------------------------------------*/
static enum result streamInit(void *, const void *);
static void streamDeinit(void *);
static enum result streamCallback(void *, void (*)(void *), void *);
static enum result streamGet(void *, enum ifOption, void *);
static enum result streamSet(void *, enum ifOption, const void *);
static size_t streamRead(void *, void *, size_t);
static size_t streamWrite(void *, const void *, size_t);
/*----------------------------------------------------------------------------*/
static const struct InterfaceClass streamTable = {
    .size = sizeof(struct UdpStream),
    .init = streamInit,
    .deinit = streamDeinit,

    .callback = streamCallback,
    .get = streamGet,
    .set = streamSet,
    .read = streamRead,
    .write = streamWrite
};
/*----------------------------------------------------------------------------*/
const struct InterfaceClass * const UdpStream = &streamTable;
/*----------------------------------------------------------------------------*/
static void cleanup(struct UdpStream *interface, enum cleanup step)
{
  switch (step)
  {
    case CLEANUP_ALL:
    case CLEANUP_THREAD:
      threadDeinit(&interface->thread);
      /* No break */
    case CLEANUP_SOCKETS:
      close(interface->serverSocket);
      close(interface->clientSocket);
      /* No break */
    case CLEANUP_MUTEX:
      mutexDeinit(&interface->mutex);
      /* No break */
    case CLEANUP_SEMAPHORE:
      semDeinit(&interface->semaphore);
      /* No break */
    case CLEANUP_QUEUE:
      byteQueueDeinit(&interface->rxQueue);
      break;
  }
}
/*----------------------------------------------------------------------------*/
static enum result setupSockets(struct UdpStream *interface,
    const struct UdpStreamConfig *config)
{
  const struct timeval timeout = {
      .tv_sec = 0,
      .tv_usec = 100
  };
  struct sockaddr_in address;
  enum result res = E_OK;

  /* Initialize output socket */
  interface->clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (interface->clientSocket == -1)
    return E_INTERFACE;

  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(config->clientPort);
  if (inet_pton(AF_INET, config->clientAddress, &address.sin_addr) <= 0)
  {
    res = E_VALUE;
    goto close_client;
  }
  if (connect(interface->clientSocket, (struct sockaddr *)&address,
      sizeof(address)) == -1)
  {
    res = E_BUSY;
    goto close_client;
  }

  /* Initialize input socket */
  interface->serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (interface->serverSocket == -1)
  {
    res = E_INTERFACE;
    goto close_client;
  }

  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(config->serverPort);
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(interface->serverSocket, (struct sockaddr *)&address,
      sizeof(address)) == -1)
  {
    res = E_BUSY;
    goto close_server;
  }

  if (setsockopt(interface->serverSocket, SOL_SOCKET, SO_RCVTIMEO,
      &timeout, sizeof(struct timeval)) == -1)
  {
    res = E_INTERFACE;
    goto close_server;
  }

  return E_OK;

close_server:
  close(interface->serverSocket);
close_client:
  close(interface->clientSocket);
  return res;
}
/*----------------------------------------------------------------------------*/
static void streamTerminateHandler(void *argument)
{
  struct UdpStream * const interface = argument;

  interface->terminated = true;
}
/*----------------------------------------------------------------------------*/
static void streamThread(void *argument)
{
  struct UdpStream * const interface = argument;
  bool terminateEvent = false;

  do
  {
    struct pollfd descriptor = {
        .fd = interface->serverSocket,
        .events = POLLIN
    };
    const int pollResult = poll(&descriptor, 1, 100);
    bool userEvent = false;

    if (pollResult == -1)
    {
      terminateEvent = true;
    }
    else if (pollResult)
    {
      uint8_t inputBuffer[BUFFER_SIZE];
      const ssize_t bytesRead = recv(interface->serverSocket,
          inputBuffer, sizeof(inputBuffer), 0);

      if (bytesRead == -1)
      {
        terminateEvent = true;
      }
      else
      {
        unsigned int currentSize;

        mutexLock(&interface->mutex);
        byteQueuePushArray(&interface->rxQueue, inputBuffer,
            (unsigned int)bytesRead);
        currentSize = byteQueueSize(&interface->rxQueue);
        mutexUnlock(&interface->mutex);

        if (currentSize >= byteQueueCapacity(&interface->rxQueue) / 2)
          userEvent = true;
      }
    }

    if (userEvent && interface->callback)
      interface->callback(interface->callbackArgument);

    mutexLock(&interface->mutex);
    terminateEvent = terminateEvent || interface->terminated;
    mutexUnlock(&interface->mutex);
  }
  while (!terminateEvent);

  mutexLock(&interface->mutex);
  interface->terminated = true;
  mutexUnlock(&interface->mutex);

  if (interface->callback)
    interface->callback(interface->callbackArgument);
}
/*----------------------------------------------------------------------------*/
static enum result streamInit(void *object, const void *configBase)
{
  const struct UdpStreamConfig * const config = configBase;
  struct UdpStream * const interface = object;
  enum result res;

  interface->callback = 0;
  interface->terminated = false;

  if ((res = byteQueueInit(&interface->rxQueue, QUEUE_SIZE)) != E_OK)
    return res;

  if ((res = semInit(&interface->semaphore, 0)) != E_OK)
  {
    cleanup(interface, CLEANUP_QUEUE);
    return res;
  }

  if ((res = mutexInit(&interface->mutex)) != E_OK)
  {
    cleanup(interface, CLEANUP_SEMAPHORE);
    return res;
  }

  if ((res = setupSockets(interface, config)) != E_OK)
  {
    cleanup(interface, CLEANUP_MUTEX);
    return res;
  }

  /* Initialize and start thread */
  res = threadInit(&interface->thread, THREAD_SIZE, THREAD_PRIORITY,
      streamThread, interface);
  if (res != E_OK)
  {
    cleanup(interface, CLEANUP_SOCKETS);
    return res;
  }

  threadOnTerminateCallback(&interface->thread, streamTerminateHandler,
      interface);

  if ((res = threadStart(&interface->thread)) != E_OK)
  {
    cleanup(interface, CLEANUP_THREAD);
    return res;
  }

  return E_OK;
}
/*----------------------------------------------------------------------------*/
static void streamDeinit(void *object)
{
  struct UdpStream * const interface = object;

  cleanup(interface, CLEANUP_ALL);
}
/*----------------------------------------------------------------------------*/
static enum result streamCallback(void *object, void (*callback)(void *),
    void *argument)
{
  struct UdpStream * const interface = object;

  interface->callbackArgument = argument;
  interface->callback = callback;
  return E_OK;
}
/*----------------------------------------------------------------------------*/
static enum result streamGet(void *object, enum ifOption option, void *data)
{
  struct UdpStream *interface = object;

  switch (option)
  {
    case IF_AVAILABLE:
      mutexLock(&interface->mutex);
      *(uint32_t *)data = byteQueueSize(&interface->rxQueue);
      mutexUnlock(&interface->mutex);
      return E_OK;

    case IF_PENDING:
      *(uint32_t *)data = 0;
      return E_OK;

    case IF_STATUS:
    {
      enum result res;

      mutexLock(&interface->mutex);
      if (interface->terminated)
        res = E_IDLE;
      else
        res = E_OK;
      mutexUnlock(&interface->mutex);

      return res;
    }

    default:
      return E_ERROR;
  }
}
/*----------------------------------------------------------------------------*/
static enum result streamSet(void *object __attribute__((unused)),
    enum ifOption option __attribute__((unused)),
    const void *data __attribute__((unused)))
{
  return E_ERROR;
}
/*----------------------------------------------------------------------------*/
static size_t streamRead(void *object, void *buffer, size_t length)
{
  struct UdpStream * const interface = object;
  unsigned int bytesRead;

  mutexLock(&interface->mutex);
  bytesRead = byteQueuePopArray(&interface->rxQueue, buffer, length);
  mutexUnlock(&interface->mutex);

  return (uint32_t)bytesRead;
}
/*----------------------------------------------------------------------------*/
static size_t streamWrite(void *object, const void *buffer, size_t length)
{
  struct UdpStream * const interface = object;
  const int bytesWritten = send(interface->clientSocket, buffer, length, 0);

  return bytesWritten == -1 ? 0 : (uint32_t)bytesWritten;
}

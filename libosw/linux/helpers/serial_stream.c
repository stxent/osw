/*
 * serial_stream.c
 * Copyright (C) 2013 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <osw/helpers/serial_stream.h>
/*----------------------------------------------------------------------------*/
#ifdef CONFIG_DEBUG
#include <stdio.h>
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) do {} while (0)
#endif
/*----------------------------------------------------------------------------*/
struct StreamRateEntry
{
  uint32_t key;
  uint32_t value;
};
/*----------------------------------------------------------------------------*/
static const struct StreamRateEntry rateList[] = {
    {
        .key = 1200,
        .value = B1200
    }, {
        .key = 2400,
        .value = B2400
    }, {
        .key = 4800,
        .value = B4800
    }, {
        .key = 9600,
        .value = B9600
    }, {
        .key = 19200,
        .value = B19200
    }, {
        .key = 38400,
        .value = B38400
    }, {
        .key = 57600,
        .value = B57600
    }, {
        .key = 115200,
        .value = B115200
    }, {
        .key = 230400,
        .value = B230400
    }, {
        .key = 460800,
        .value = B460800
    }, {
        .key = 500000,
        .value = B500000
    }, {
        .key = 576000,
        .value = B576000
    }, {
        .key = 921600,
        .value = B921600
    }, {
        .key = 1000000,
        .value = B1000000
    }, {
        .key = 0
    }
};
/*----------------------------------------------------------------------------*/
static void configurePort(struct SerialStream *);
static enum Result setPortParameters(struct SerialStream *,
    const struct SerialStreamConfig *);
/*----------------------------------------------------------------------------*/
static enum Result streamInit(void *, const void *);
static void streamDeinit(void *);
static enum Result streamSetCallback(void *, void (*)(void *), void *);
static enum Result streamGetParam(void *, enum IfParameter, void *);
static enum Result streamSetParam(void *, enum IfParameter, const void *);
static size_t streamRead(void *, void *, size_t);
static size_t streamWrite(void *, const void *, size_t);
/*----------------------------------------------------------------------------*/
static const struct InterfaceClass streamTable = {
    .size = sizeof(struct SerialStream),
    .init = streamInit,
    .deinit = streamDeinit,

    .setCallback = streamSetCallback,
    .getParam = streamGetParam,
    .setParam = streamSetParam,
    .read = streamRead,
    .write = streamWrite
};
/*----------------------------------------------------------------------------*/
const struct InterfaceClass * const SerialStream = &streamTable;
/*----------------------------------------------------------------------------*/
static void configurePort(struct SerialStream *interface)
{
  struct termios options;

  tcgetattr(interface->descriptor, &options);

  /* Enable raw mode */
  options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR
      | ICRNL | IXON);
  options.c_oflag &= ~OPOST;
  options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;

  options.c_cflag |= CLOCAL; /* Ignore modem control lines */
  options.c_cflag |= CREAD; /* Enable receiver */

  options.c_cc[VMIN] = 0; /* Minimal data packet length */
  options.c_cc[VTIME] = 1; /* Time to wait for data */

  tcsetattr(interface->descriptor, TCSANOW, &options);
}
/*----------------------------------------------------------------------------*/
static enum Result setPortParameters(struct SerialStream *interface,
    const struct SerialStreamConfig *config)
{
  const struct StreamRateEntry *entry = rateList;
  unsigned int rate = 0;
  struct termios options;

  while (entry->key)
  {
    if (config->rate == entry->key)
    {
      rate = entry->value;
      break;
    }
    ++entry;
  }
  if (!rate)
    return E_VALUE;

  tcgetattr(interface->descriptor, &options);

  switch ((enum SerialStreamParity)config->parity)
  {
    case SERIAL_PARITY_NONE:
      options.c_cflag &= ~PARENB; /* Disable parity */
      break;

    case SERIAL_PARITY_ODD:
      options.c_cflag |= PARENB; /* Use parity */
      options.c_cflag |= PARODD; /* Odd parity */
      break;

    case SERIAL_PARITY_EVEN:
      options.c_cflag |= PARENB; /* Use parity */
      options.c_cflag &= ~PARODD; /* Even parity */
      break;

    default:
      return E_VALUE;
  }

  cfsetispeed(&options, rate);
  cfsetospeed(&options, rate);

  tcsetattr(interface->descriptor, TCSANOW, &options);

  return E_OK;
}
/*----------------------------------------------------------------------------*/
static enum Result streamInit(void *object, const void *configBase)
{
  const struct SerialStreamConfig * const config = configBase;
  struct SerialStream * const interface = object;
  enum Result res;

  interface->descriptor = open(config->device, O_RDWR | O_NOCTTY | O_NDELAY);
  if (interface->descriptor != -1)
    fcntl(interface->descriptor, F_SETFL, 0);
  else
    return E_INTERFACE;
  DEBUG_PRINT("Port opened: %s\n", config->device);

  if ((res = setPortParameters(interface, config)) != E_OK)
  {
    close(interface->descriptor);
    return res;
  }
  DEBUG_PRINT("Port parameters set\n");

  configurePort(interface);
  DEBUG_PRINT("Port configured\n");

  return E_OK;
}
/*----------------------------------------------------------------------------*/
static void streamDeinit(void *object)
{
  struct SerialStream * const interface = object;

  close(interface->descriptor);
}
/*----------------------------------------------------------------------------*/
static enum Result streamSetCallback(void *object __attribute__((unused)),
    void (*callback)(void *) __attribute__((unused)),
    void *argument __attribute__((unused)))
{
  /* Not implemented */
  return E_INVALID;
}
/*----------------------------------------------------------------------------*/
static enum Result streamGetParam(void *object, enum IfParameter option,
    void *data)
{
  struct SerialStream * const interface = object;

  switch ((enum SerialStreamOption)option)
  {
    case IF_SERIAL_CTS:
    {
      int value;

      if (ioctl(interface->descriptor, TIOCMGET, &value) != -1)
      {
        *(unsigned int *)data = (value & TIOCM_CTS) ? 1 : 0;
        return E_OK;
      }
      else
      {
        return E_INTERFACE;
      }
    }

    default:
      return E_INVALID;
  }
}
/*----------------------------------------------------------------------------*/
static enum Result streamSetParam(void *object, enum IfParameter option,
    const void *data)
{
  struct SerialStream * const interface = object;

  switch ((enum SerialStreamOption)option)
  {
    case IF_SERIAL_RTS:
    {
      int value;

      if (ioctl(interface->descriptor, TIOCMGET, &value) == -1)
        return E_INTERFACE;

      if (*(const unsigned int *)data)
        value |= TIOCM_RTS;
      else
        value &= ~TIOCM_RTS;

      if (ioctl(interface->descriptor, TIOCMSET, &value) == -1)
        return E_INTERFACE;
      else
        return E_OK;
    }

    default:
      return E_INVALID;
  }
}
/*----------------------------------------------------------------------------*/
static size_t streamRead(void *object, void *buffer, size_t length)
{
  struct SerialStream * const interface = object;
  const ssize_t result = read(interface->descriptor, buffer, length);

  if (result != -1)
    return result;
  else
    return 0;
}
/*----------------------------------------------------------------------------*/
static size_t streamWrite(void *object, const void *buffer, size_t length)
{
  struct SerialStream * const interface = object;
  const ssize_t result = write(interface->descriptor, buffer, length);

  if (result != -1)
    return result;
  else
    return 0;
}

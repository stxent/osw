/*
 * libosw/helpers/serial_stream.h
 * Copyright (C) 2013 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#ifndef LIBOSW_HELPERS_SERIAL_STREAM_H_
#define LIBOSW_HELPERS_SERIAL_STREAM_H_
/*----------------------------------------------------------------------------*/
#include <interface.h>
/*----------------------------------------------------------------------------*/
extern const struct InterfaceClass * const SerialStream;
/*----------------------------------------------------------------------------*/
enum serialStreamParity
{
  SERIAL_PARITY_NONE = 0,
  SERIAL_PARITY_ODD,
  SERIAL_PARITY_EVEN
};
/*----------------------------------------------------------------------------*/
enum serialStreamOption
{
  IF_SERIAL_CTS = IF_OPTION_END,
  IF_SERIAL_RTS
};
/*----------------------------------------------------------------------------*/
struct SerialStreamConfig
{
  const char *device;
  uint32_t rate;
  enum serialStreamParity parity;
};
/*----------------------------------------------------------------------------*/
struct SerialStream
{
  struct Interface parent;

  int descriptor;
};
/*----------------------------------------------------------------------------*/
#endif /* LIBOSW_HELPERS_SERIAL_STREAM_H_ */

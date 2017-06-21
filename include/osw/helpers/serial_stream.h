/*
 * osw/helpers/serial_stream.h
 * Copyright (C) 2013 xent
 * Project is distributed under the terms of the GNU General Public License v3.0
 */

#ifndef OSW_HELPERS_SERIAL_STREAM_H_
#define OSW_HELPERS_SERIAL_STREAM_H_
/*----------------------------------------------------------------------------*/
#include <stdint.h>
#include <xcore/interface.h>
/*----------------------------------------------------------------------------*/
extern const struct InterfaceClass * const SerialStream;
/*----------------------------------------------------------------------------*/
enum SerialStreamParity
{
  SERIAL_PARITY_NONE = 0,
  SERIAL_PARITY_ODD,
  SERIAL_PARITY_EVEN
};
/*----------------------------------------------------------------------------*/
enum SerialStreamOption
{
  IF_SERIAL_CTS = IF_PARAMETER_END,
  IF_SERIAL_RTS
};
/*----------------------------------------------------------------------------*/
struct SerialStreamConfig
{
  const char *device;
  uint32_t rate;
  enum SerialStreamParity parity;
};
/*----------------------------------------------------------------------------*/
struct SerialStream
{
  struct Interface parent;

  int descriptor;
};
/*----------------------------------------------------------------------------*/
#endif /* OSW_HELPERS_SERIAL_STREAM_H_ */

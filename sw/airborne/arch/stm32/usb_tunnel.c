/*
 * Copyright (C) 2009  Martin Mueller
 *               2014  Felix Ruess <felix.ruess@gmail.com
 *
 * This file is part of paparazzi.
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

/**
 * @file usb_tunnel.c
 *
 * USB tunnel application.
 *
 * This creates a USB serial port that connects to one of the UARTs.
 * This enables you to e.g. configure the gps receiver or the modem without removing it.
 */

#include "std.h"
#include "mcu.h"
#include "mcu_periph/sys_time.h"
#include "led.h"
#include "mcu_periph/uart.h"
#include "mcu_periph/usb_serial.h"

#ifndef USB_TUNNEL_UART
#error USB_TUNNEL_UART not defined. Add <configure name="TUNNEL_PORT" value="UARTx"/>
#endif

INFO_VAR(USB_TUNNEL_UART)


static void tunnel_event(void)
{
  static unsigned char inc;

  if (uart_char_available(&USB_TUNNEL_UART) && VCOM_check_free_space(1)) {
    inc = uart_getch(&USB_TUNNEL_UART);
    VCOM_putchar(inc);
  }
  if (VCOM_check_available() && uart_check_free_space(&USB_TUNNEL_UART, 1)) {
    inc = VCOM_getchar();
    uart_transmit(&USB_TUNNEL_UART, inc);
  }
}

int main(void)
{
  mcu_init();
  sys_time_init();
  led_init();

  VCOM_allow_linecoding(1);

  VCOM_init();

  mcu_int_enable();

  while(1) {
    VCOM_event();
    tunnel_event();
  }

  return 0;
}
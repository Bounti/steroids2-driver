/*******************************************************************************
@Author: Corteggiani Nassim <Corteggiani>
@Email:  nassim.corteggiani@maximintegrated.com
@Filename: Device.cpp
@Last modified by:   noname
@Last modified time: 16-Mar-2017
@License: GPLv3

Copyright (C) 2017 Maxim Integrated Products, Inc., All Rights Reserved.
Copyright (C) 2017 Corteggiani Nassim <Corteggiani>

*
*    This program is free software: you can redistribute it and/or modify      *
*    it under the terms of the GNU General Public License as published by      *
*    the Free Software Foundation, either version 3 of the License, or         *
*    (at your option) any later version.                                       *
*    This program is distributed in the hope that it will be useful,           *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*    GNU General Public License for more details.                              *
*    You should have received a copy of the GNU General Public License         *
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
*                                                                              *
*                                                                              *
********************************************************************************/
#include "device.hpp"
#include "termcolor.hpp"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

libusb_device_handle *device::handle = NULL;

bool device::initialized = false;

device::device(uint16_t p_vid, uint16_t p_pid, uint32_t p_interface,
               uint8_t out, uint8_t in) {

  timeout = 500;

  entrypoint_download = out;
  entrypoint_upload = in;

  vid = p_vid;

  pid = p_pid;

  interface = p_interface;

  buffer_limit = 1024;
}

device::~device() {}

void device::close(void) { device_close(); }

void device::device_open() {

  int cnt, idx, errCode;
  libusb_device **devs;

  if (libusb_init(&context) < 0)
    return;

  cnt = libusb_get_device_list(context, &devs);

  for (idx = 0; idx < cnt; idx++) {

    // cout << termcolor::blue << "Detecting device idVendor = " << hex
    //      << descriptor.idVendor << "(Expected idProduct = " << hex
    //      << descriptor.idProduct << ")" << endl;

    if (libusb_get_device_descriptor(devs[idx], &descriptor) != 0)
      continue;

    if (descriptor.idVendor != vid || descriptor.idProduct != pid) {
      continue;
    }

    dev = devs[idx];

    errCode = libusb_open(devs[idx], &device::handle);

    if (errCode) {
      device::handle = NULL;
      cout << termcolor::red << "libusb_open() failed with "
           << libusb_error_name(errCode) << endl;
      continue;
    }

    break;
  }
  if (cnt >= 0)
    libusb_free_device_list(devs, 1);
}

void device::device_close() {
  cout << termcolor::blue << "closing device " << endl;
  /* Close device */
  libusb_close(device::handle);
  libusb_exit(context);
}

void device::init(void) {

  if (device::initialized)
    return;

  device::initialized = true;

  int32_t retval;

  device_open();

  if (!device::handle) {
    cout << termcolor::red << "Inception debugger driver doesn't find device " << hex << vid << ":"
         << hex << pid << endl;
    cout << termcolor::white << "\n";
    throw std::runtime_error("Inception debugger driver doesn't find device\n");
    return;
  }

  dev = libusb_get_device(device::handle);

  if (vid != descriptor.idVendor) {
    cout << termcolor::red << "ID Vendor " << hex << vid << ":" << hex
         << descriptor.idVendor << endl;
    exit(0);
  }

  if (pid != descriptor.idProduct) {
    cout << termcolor::red << "ID Product " << hex << pid << ":" << hex
         << descriptor.idProduct << endl;
    exit(0);
  }

  is_open = 1;

  busnum = libusb_get_bus_number(dev);
  devaddr = libusb_get_device_address(dev);

  retval = libusb_claim_interface(device::handle, interface);
  if (retval == 0) {
    cout << termcolor::blue << "driver successfully claimed interface" << endl;
  } else {

    cout << termcolor::red << "device " << hex << vid << ":" << hex << pid
         << endl;

    switch (retval) {
    case LIBUSB_ERROR_NOT_FOUND:
      cout << termcolor::red
           << "the interface requested by avatar driver does not exist" << endl;
      throw std::runtime_error("another driver has claimed the interface\n");
      break;

    case LIBUSB_ERROR_BUSY:
      cout << termcolor::red << "another driver has claimed the interface"
           << endl;
      throw std::runtime_error("another driver "
                               "has claimed the interface\n");
      break;

    case LIBUSB_ERROR_NO_DEVICE:
      cout << termcolor::red
           << "driver detected that the device has been disconnected" << endl;
      throw std::runtime_error(
          "driver detected that the device has been disconnected\n");
      break;

    default:
      cout << termcolor::red << "driver was not able to claimed interface..."
           << endl;
      throw std::runtime_error("driver was not able to claimed interface...\n");
      break;
    }
    device::handle = NULL;
    return;
  }

  // libusb_clear_halt(this->handle, 0x81);
  // libusb_clear_halt(this->handle, 0x01);

  return;
}

uint32_t device::io(uint8_t endpoint, uint8_t *buffer, uint32_t size) {

  int32_t retval;
  int32_t transferred;
  int32_t attempt = 0;

  // do {
  if ((retval = libusb_bulk_transfer(device::handle, endpoint, buffer, size,
                                     &transferred, timeout)) != 0) {
    cout << termcolor::red
         << "USB driver failed : " << libusb_error_name(retval) << endl;

    switch (retval) {
    case LIBUSB_ERROR_TIMEOUT:
      cout << termcolor::red << "USB driver failed : timeout error" << endl;
      break;
    case LIBUSB_ERROR_PIPE:
      cout << termcolor::red << "USB driver failed : pipe error" << endl;
      break;
    case LIBUSB_ERROR_OVERFLOW:
      cout << termcolor::red << "USB driver failed : overflow" << endl;
      break;
    case LIBUSB_ERROR_NO_DEVICE:
      cout << termcolor::red << "USB driver failed : no device" << endl;
      break;
    default:
      cout << termcolor::red << "USB driver failed : error code " << retval
           << endl;
      break;
    }
    // sleep(1);
    // attempt++;
    //   } else if (size == 0)
    //     attempt++;
    //   else
    //     break;
    // } while (attempt < 2);

    // if (attempt >= 1) {
    // return 0;
    // cout << termcolor::red
    //      << "driver failed to communicate with device ... endpoint : " <<
    //      hex
    //      << endpoint << endl;
    // throw std::runtime_error(
    //     "driver failed to communicate with device ... endpoint\n");
  }

  return transferred;
}

void device::send(uint8_t *data, uint32_t size) {

  size = io(entrypoint_download, data, size);

  //std::stringstream info;

  //info << "0x" << std::hex << std::setfill('0');
  //for (unsigned int i = 0; i < size; i++) {
  //  info << std::setw(2) << static_cast<unsigned>(data[i]);
  //  info << " ";
  //}
  //cout << termcolor::green << "JTAG > " << info.str() << endl;
}

void device::receive(uint8_t *data, uint32_t size) {

  std::stringstream info;
  uint32_t received = 0;
  int32_t attempt = 0;
  uint32_t recv_size = 0;

  do {
    recv_size = io(entrypoint_upload, data + received, size - received);
    if (recv_size == 0) {
      attempt++;
    } else {
      received += recv_size;
      //cout << termcolor::white << "received :" << received << endl;
    }
  } while (received < size && attempt < 1);

  //info << "0x" << std::hex << std::setfill('0');
  //for (unsigned int i = (received-1); i>=0; --i) {
  //  info << std::setw(2) << static_cast<unsigned>(data[i]);
  //  info << " ";
  //}
  //cout << termcolor::green << "JTAG < " << info.str() << endl;
}

#ifndef COMMAND_H
#define COMMAND_H
typedef enum {
  c_read,
  c_write,
  c_reset,
  c_scan,
  c_scan_start,
  c_scan_stop,
  c_scan_loopback
} COMMAND;
#endif

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdbool.h>
#include <stdint.h>

struct accelerometer {
  int current_register_;
  int current_write_value_;
  bool repeated_start_;
  bool write_value_sent_;
  bool write_;
  bool pending_;
  int8_t x_, y_, z_;
  int8_t prev_x_, prev_y_, prev_z_;
  void (*callback_read_)(uint8_t);
  void (*callback_write_)();
} accelerometer;

int8_t read_accelerometer_register(int);
void write_accelerometer_register(int, int);
void write_accelerometer_register_async(int reg, int value, void (*callback)());
void read_accelerometer_register_async(int reg, void (*callback)(uint8_t));
void initialize_accelerometer();
void read_accelelerometer_register_irq();
void write_accelerometer_register_irq();
void read_accelerometer_state();

#endif // ACCELEROMETER_H

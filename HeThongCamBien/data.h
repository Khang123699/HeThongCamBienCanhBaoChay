#ifndef DATA_H
#define DATA_H

#include <Arduino.h>

extern String trangThai;
extern unsigned long mq2;
extern int temp;
extern int hum;
extern int fire;

// flag báo có dữ liệu mới
extern bool dataChanged;

#endif
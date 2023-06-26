#ifndef __BMP_H
#define __BMP_H

#include "oled.h"
#include "delay.h"

extern unsigned char logo[];
extern unsigned char Test_BMP[];
extern unsigned char progress[][320];
extern unsigned char Pencil[][512]; //共31帧，一帧大小为64*64，数组有512个
extern unsigned char Rocket[][512]; //共18帧，一帧大小为64*64，数组有512个
extern unsigned char Bell[][512]; //共28帧，一帧大小为64*64，数组有512个


#endif

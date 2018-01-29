#ifndef IOCONTROLLER_H
#define IOCONTROLLER_H

#include <stdio.h>
#include <stdlib.h>

#define K1Open() setRelayState(1, 1)
#define K2Open() setRelayState(2, 1)
#define K3Open() setRelayState(3, 1)
#define K4Open() setRelayState(4, 1)
#define K5Open() setRelayState(5, 1)
#define K1Close() setRelayState(1, 0)
#define K2Close() setRelayState(2, 0)
// 下水阀
#define K3Close() setRelayState(3, 0)
#define K4Close() setRelayState(4, 0)
#define K5Close() setRelayState(5, 0)

//All switch outputs are on GPF
//Return non-zero value means error
void setRelayState(int n, int state);

//Get valve n status,
GPIO_PinState getRelayState(int n);
// IOCONTROLLER_H
#endif

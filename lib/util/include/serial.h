/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#define SERIAL_PARITY_NONE      0
#define SERIAL_PARITY_ODD       3
#define SERIAL_PARITY_EVEN      6

int set_serial_baudrate(int speed);
int set_serial_databits(int databits);
int set_serial_stopbits(int stopbits);
int set_serial_parity(int parity);
int get_serial_baudrate(void);
int get_serial_databits(void);
int get_serial_stopbits(void);
int get_serial_parity(void);

#endif

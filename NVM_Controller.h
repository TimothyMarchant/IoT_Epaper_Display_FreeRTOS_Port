/* 
 * File:   NVM_Controller.h
 * Author: Timothy Marchant
 *
 * Created on June 1, 2025, 9:04 PM
 */

#ifndef NVM_CONTROLLER_H
#define	NVM_CONTROLLER_H
void WritePageTo_DATAFLASH(unsigned char* Data,unsigned short pagenum);
void ReadPageFrom_DATAFLASH(unsigned char* Readbuffer,unsigned short pagenum);
void ReadStringFrom_DATAFLASH(unsigned char* Readbuffer,unsigned char stringlength,unsigned int offset);
void ReadIntFrom_DATAFLASH(unsigned int* data,unsigned int offset);
#endif
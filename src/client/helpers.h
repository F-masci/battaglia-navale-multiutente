#pragma once

#include "../shared/lib.h"

bool waitNum(uint32_t * const _ptr);
bool writeNum(uint32_t _num);
bool waitString(char ** const _buff);
bool writeString(const char * _buff);
#ifndef _ae_util_h
#define _ae_util_h

#include <stdint.h>
#include <fstream>
#include <string>

uint32_t swap32(uint32_t num);
uint16_t swap16(uint16_t num);
uint32_t read_variable_quantity(std::ifstream *f);
std::string read_string(std::ifstream *f, uint32_t length);

#endif

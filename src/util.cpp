#include "util.hpp"

uint32_t swap32(uint32_t num)
{
    return ((num >> 24) & 0xff) | ((num << 8) & 0xff0000) | ((num >> 8) & 0xff00) | ((num << 24) & 0xff000000);
}

uint16_t swap16(uint16_t num)
{
    return (num >> 8) | (num << 8);
}

uint32_t read_variable_quantity(std::ifstream *f)
{
    uint32_t val = f->get();
    uint8_t byte = 0;

    if (val & 0x80)
    {
        val &= 0x7f;

        do
        {
            byte = f->get();
            val = (val << 7) | (byte & 0x7f);
        } while (byte & 0x80);
    }

    return val;
}

std::string read_string(std::ifstream *f, uint32_t length)
{
    std::string s = "";

    for (uint32_t i = 0; i < length; i++)
    {
        s += f->get();
    }

    return s;
}

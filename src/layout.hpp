#ifndef _ca_layout_h
#define _ca_layout_h

#include <vector>

#define KEY_WIDTH 17.07

typedef struct
{
    double left;
    double right;
} Key;

class Layout
{
public:
    Layout(int first_key, int last_key);

    std::vector<Key> keys;
    std::vector<bool> black_key;
};

#endif

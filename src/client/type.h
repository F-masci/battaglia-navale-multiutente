#pragma once

typedef char cell_t;

typedef struct _ship_t{
    uint8_t dim;
    uint8_t x;
    uint8_t y;
    char dir;
} ship_t;
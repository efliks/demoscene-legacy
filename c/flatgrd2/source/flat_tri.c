/*
 * FlatGrd2 06/02/02
 * Mikolaj Felix a.k.a. Majuma
 * mfelix@polbox.com
 */

#include "common.h"


void flat_line(int x1, int x2, int y, unsigned char color,
    unsigned char* buffer)
{
    int i;
    unsigned char* ofs;

    if (y < 0 || y > 199 || x1 == x2) {
        return;
    }

    if (x1 > x2) {
        i = x1;
        x1 = x2;
        x2 = i;
    }

    ofs = buffer;
    ofs += ((y << 6) + (y << 8) + x1);

    i = 0;
    while ((i++) < (x2 - x1)) {
        *ofs = color;
        ofs++;
    }
}

void flat_triangle(int x1, int y1, int x2, int y2, int x3, int y3,
    unsigned char color, unsigned char* buffer)
{
    long dx12, dx13, dx23, scan_x1, scan_x2;
    int i;

    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;
    }
    if (y2 > y3) {
        i = y2;
        y2 = y3;
        y3 = i;

        i = x2;
        x2 = x3;
        x3 = i;
    }
    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;
    }

    if ((i = y2 - y1) != 0)
        dx12 = ((x2 - x1) << SHIFT_CONST) / i;
    else
        dx12 = 0;

    if ((i = y3 - y1) != 0)
        dx13 = ((x3 - x1) << SHIFT_CONST) / i;
    else
        dx13 = 0;

    if ((i = y3 - y2) != 0)
        dx23 = ((x3 - x2) << SHIFT_CONST) / i;
    else
        dx23 = 0;

    scan_x1 = scan_x2 = x1 << SHIFT_CONST;
    for (i = y1; i < y2; i++) {
        flat_line(scan_x1 >> SHIFT_CONST,
            scan_x2 >> SHIFT_CONST, i, color, buffer);

        scan_x1 += dx13;
        scan_x2 += dx12;
    }

    scan_x2 = x2 << SHIFT_CONST;
    for (i = y2; i < y3; i++) {
        flat_line(scan_x1 >> SHIFT_CONST,
            scan_x2 >> SHIFT_CONST, i, color, buffer);

        scan_x1 += dx13;
        scan_x2 += dx23;
    }
}


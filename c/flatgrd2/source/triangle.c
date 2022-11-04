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

void gouraud_line(int x1, int x2, int y, unsigned char c1, unsigned char c2,
    unsigned char* buffer)
{
    long delta_c, curr_c;
    int i;
    unsigned char* ofs;

    if (y < 0 || y > 199 || x1 == x2) {
        return;
    }

    if (x1 > x2) {
        i = x1;
        x1 = x2;
        x2 = i;

        i = c1;
        c1 = c2;
        c2 = i;
    }

    delta_c = ((c2 - c1) << SHIFT_CONST) / (x2 - x1);
    curr_c = c1 << SHIFT_CONST;

    ofs = buffer;
    ofs += ((y << 6) + (y << 8) + x1);

    i = 0;
    while ((i++) < (x2 - x1)) {
        *ofs = curr_c >> SHIFT_CONST;
        curr_c += delta_c;
        ofs++;
    }
}

void gouraud_triangle(int x1, int y1, int x2, int y2, int x3, int y3,
    unsigned char c1, unsigned char c2, unsigned char c3,
    unsigned char* buffer)
{
    long dx12, dx13, dx23, scan_x1, scan_x2;
    long dc12, dc13, dc23, scan_c1, scan_c2;
    int i;

    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;

        i = c1;
        c1 = c2;
        c2 = i;
    }
    if (y2 > y3) {
        i = y2;
        y2 = y3;
        y3 = i;

        i = x2;
        x2 = x3;
        x3 = i;

        i = c2;
        c2 = c3;
        c3 = i;
    }
    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;

        i = c1;
        c1 = c2;
        c2 = i;
    }

    if ((i = y2 - y1) != 0) {
        dx12 = ((x2 - x1) << SHIFT_CONST) / i;
        dc12 = ((c2 - c1) << SHIFT_CONST) / i;
    } else {
        dx12 = 0;
        dc12 = 0;
    }

    if ((i = y3 - y1) != 0) {
        dx13 = ((x3 - x1) << SHIFT_CONST) / i;
        dc13 = ((c3 - c1) << SHIFT_CONST) / i;
    } else {
        dx13 = 0;
        dc13 = 0;
    }

    if ((i = y3 - y2) != 0) {
        dx23 = ((x3 - x2) << SHIFT_CONST) / i;
        dc23 = ((c3 - c2) << SHIFT_CONST) / i;
    } else {
        dx23 = 0;
        dc23 = 0;
    }

    scan_x1 = scan_x2 = x1 << SHIFT_CONST;
    scan_c1 = scan_c2 = c1 << SHIFT_CONST;

    for (i = y1; i < y2; i++) {
        gouraud_line(scan_x1 >> SHIFT_CONST, scan_x2 >> SHIFT_CONST, i,
            scan_c1 >> SHIFT_CONST, scan_c2 >> SHIFT_CONST, buffer);

        scan_x1 += dx13;
        scan_x2 += dx12;
        scan_c1 += dc13;
        scan_c2 += dc12;
    }

    scan_x2 = x2 << SHIFT_CONST;
    scan_c2 = c2 << SHIFT_CONST;

    for (i = y2; i < y3; i++) {
        gouraud_line(scan_x1 >> SHIFT_CONST, scan_x2 >> SHIFT_CONST, i,
            scan_c1 >> SHIFT_CONST, scan_c2 >> SHIFT_CONST, buffer);

        scan_x1 += dx13;
        scan_x2 += dx23;
        scan_c1 += dc13;
        scan_c2 += dc23;
    }
}

void textured_line(int x1, int x2, int y,
    int tx1, int ty1, int tx2, int ty2,
    unsigned char* texture, unsigned char* buffer)
{
    long delta_x, delta_y, curr_x, curr_y;
    int i, lenght;
    unsigned char* ofs;

    if (y < 0 || y > 199 || x1 == x2) {
        return;
    }

    if (x1 > x2) {
        i = x1;
        x1 = x2;
        x2 = i;

        i = tx1;
        tx1 = tx2;
        tx2 = i;

        i = ty1;
        ty1 = ty2;
        ty2 = i;
    }

    lenght = x2 - x1;

    delta_x = ((tx2 - tx1) << SHIFT_CONST) / lenght;
    curr_x = tx1 << SHIFT_CONST;

    delta_y = ((ty2 - ty1) << SHIFT_CONST) / lenght;
    curr_y = ty1 << SHIFT_CONST;

    ofs = buffer;
    ofs += ((y << 6) + (y << 8) + x1);

    for (i = 0; i < lenght; i++) {
        *ofs = texture[(curr_x >> SHIFT_CONST) + ((curr_y >> SHIFT_CONST) << 7)];
        ofs++;

        curr_x += delta_x;
        curr_y += delta_y;
    }
}

void textured_triangle(int x1, int y1, int x2, int y2, int x3, int y3,
    int tx1, int ty1, int tx2, int ty2, int tx3, int ty3,
    unsigned char* texture, unsigned char* buffer)
{
    long dx12, dx13, dx23, curr_x1, curr_x2;
    long tdx12, tdy12, tdx13, tdy13, tdx23, tdy23, scan_x1, scan_y1, scan_x2, scan_y2;
    int i;

    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;

        i = ty1;
        ty1 = ty2;
        ty2 = i;

        i = tx1;
        tx1 = tx2;
        tx2 = i;
    }
    if (y2 > y3) {
        i = y2;
        y2 = y3;
        y3 = i;

        i = x2;
        x2 = x3;
        x3 = i;

        i = ty2;
        ty2 = ty3;
        ty3 = i;

        i = tx2;
        tx2 = tx3;
        tx3 = i;
    }
    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;

        i = ty1;
        ty1 = ty2;
        ty2 = i;

        i = tx1;
        tx1 = tx2;
        tx2 = i;
    }

    if ((i = y2 - y1) != 0) {
        dx12 = ((x2 - x1) << SHIFT_CONST) / i;
        tdx12 = ((tx2 - tx1) << SHIFT_CONST) / i;
        tdy12 = ((ty2 - ty1) << SHIFT_CONST) / i;
    } else {
        dx12 = 0;
        tdx12 = 0;
        tdy12 = 0;
    }

    if ((i = y3 - y1) != 0) {
        dx13 = ((x3 - x1) << SHIFT_CONST) / i;
        tdx13 = ((tx3 - tx1) << SHIFT_CONST) / i;
        tdy13 = ((ty3 - ty1) << SHIFT_CONST) / i;
    } else {
        dx13 = 0;
        tdx13 = 0;
        tdy13 = 0;
    }

    if ((i = y3 - y2) != 0) {
        dx23 = ((x3 - x2) << SHIFT_CONST) / i;
        tdx23 = ((tx3 - tx2) << SHIFT_CONST) / i;
        tdy23 = ((ty3 - ty2) << SHIFT_CONST) / i;
    } else {
        dx23 = 0;
        tdx23 = 0;
        tdy23 = 0;
    }

    curr_x1 = curr_x2 = x1 << SHIFT_CONST;
    scan_x1 = scan_x2 = tx1 << SHIFT_CONST;
    scan_y1 = scan_y2 = ty1 << SHIFT_CONST;

    for (i = y1; i < y2; i++) {
        textured_line(curr_x1 >> SHIFT_CONST, curr_x2 >> SHIFT_CONST, i,
            scan_x1 >> SHIFT_CONST, scan_y1 >> SHIFT_CONST,
            scan_x2 >> SHIFT_CONST, scan_y2 >> SHIFT_CONST, texture, buffer);

        curr_x1 += dx13;
        curr_x2 += dx12;

        scan_x1 += tdx13;
        scan_y1 += tdy13;
        scan_x2 += tdx12;
        scan_y2 += tdy12;
    }

    curr_x2 = x2 << SHIFT_CONST;
    scan_x2 = tx2 << SHIFT_CONST;
    scan_y2 = ty2 << SHIFT_CONST;

    for (i = y2; i < y3; i++) {
        textured_line(curr_x1 >> SHIFT_CONST, curr_x2 >> SHIFT_CONST, i,
            scan_x1 >> SHIFT_CONST, scan_y1 >> SHIFT_CONST,
            scan_x2 >> SHIFT_CONST, scan_y2 >> SHIFT_CONST, texture, buffer);

        curr_x1 += dx13;
        curr_x2 += dx23;

        scan_x1 += tdx13;
        scan_y1 += tdy13;
        scan_x2 += tdx23;
        scan_y2 += tdy23;
    }
}

void bump_line(int x1, int x2, int y,
    int bx1, int by1, int bx2, int by2,
    int ex1, int ey1, int ex2, int ey2,
    unsigned char* bumpmap, unsigned char* envmap,
    unsigned char* buffer)
{
    unsigned char *ofs, color;
    int i, width;
    long dbx, dby, curr_bx, curr_by;
    long dex, dey, curr_ex, curr_ey;
    int nx, ny, off;

    if (y < 0 || y > 199 || x1 == x2)
        return;

    if (x1 > x2) {
        i = x1;
        x1 = x2;
        x2 = i;

        i = bx1;
        bx1 = bx2;
        bx2 = i;

        i = by1;
        by1 = by2;
        by2 = i;

        i = ex1;
        ex1 = ex2;
        ex2 = i;

        i = ey1;
        ey1 = ey2;
        ey2 = i;
    }

    width = x2 - x1;
    dbx = ((bx2 - bx1) << SHIFT_CONST) / width;
    dby = ((by2 - by1) << SHIFT_CONST) / width;

    dex = ((ex2 - ex1) << SHIFT_CONST) / width;
    dey = ((ey2 - ey1) << SHIFT_CONST) / width;

    curr_bx = bx1 << SHIFT_CONST;
    curr_by = by1 << SHIFT_CONST;

    curr_ex = ex1 << SHIFT_CONST;
    curr_ey = ey1 << SHIFT_CONST;

    ofs = buffer;
    ofs += ((y << 6) + (y << 8) + x1);

    for (i = 0; i < width; i++) {
        off = (curr_bx >> SHIFT_CONST) + ((curr_by >> SHIFT_CONST) << 7);
        nx = bumpmap[(off - 1) & 16383] - bumpmap[(off + 1) & 16383];
        ny = bumpmap[(off - 128) & 16383] - bumpmap[(off + 128) & 16383];

        nx += (curr_ex >> SHIFT_CONST);
        ny += (curr_ey >> SHIFT_CONST);

        if (nx >= 0 && nx <= 127 && ny >= 0 && ny <= 127)
            color = envmap[nx + (ny << 7)];
        else
            color = 0;

        *ofs = color;
        ofs++;

        curr_bx += dbx;
        curr_by += dby;

        curr_ex += dex;
        curr_ey += dey;
    }
}

void bumpmapped_triangle(tri_struct* tri, tri_struct* bump_tri,
    tri_struct* env_tri,
    unsigned char* bumpmap, unsigned char* envmap,
    unsigned char* buffer)
{
    int i;
    long dx12, dx13, dx23, curr_x1, curr_x2;
    long dbx12, dby12, dbx13, dby13, dbx23, dby23, curr_bx1, curr_by1, curr_bx2, curr_by2;
    long dex12, dey12, dex13, dey13, dex23, dey23, curr_ex1, curr_ey1, curr_ex2, curr_ey2;

    if (tri->y1 > tri->y2) {
        i = tri->y1;
        tri->y1 = tri->y2;
        tri->y2 = i;

        i = tri->x1;
        tri->x1 = tri->x2;
        tri->x2 = i;

        i = bump_tri->y1;
        bump_tri->y1 = bump_tri->y2;
        bump_tri->y2 = i;

        i = bump_tri->x1;
        bump_tri->x1 = bump_tri->x2;
        bump_tri->x2 = i;

        i = env_tri->y1;
        env_tri->y1 = env_tri->y2;
        env_tri->y2 = i;

        i = env_tri->x1;
        env_tri->x1 = env_tri->x2;
        env_tri->x2 = i;
    }

    if (tri->y2 > tri->y3) {
        i = tri->y2;
        tri->y2 = tri->y3;
        tri->y3 = i;

        i = tri->x2;
        tri->x2 = tri->x3;
        tri->x3 = i;

        i = bump_tri->y2;
        bump_tri->y2 = bump_tri->y3;
        bump_tri->y3 = i;

        i = bump_tri->x2;
        bump_tri->x2 = bump_tri->x3;
        bump_tri->x3 = i;

        i = env_tri->y2;
        env_tri->y2 = env_tri->y3;
        env_tri->y3 = i;

        i = env_tri->x2;
        env_tri->x2 = env_tri->x3;
        env_tri->x3 = i;
    }

    if (tri->y1 > tri->y2) {
        i = tri->y1;
        tri->y1 = tri->y2;
        tri->y2 = i;

        i = tri->x1;
        tri->x1 = tri->x2;
        tri->x2 = i;

        i = bump_tri->y1;
        bump_tri->y1 = bump_tri->y2;
        bump_tri->y2 = i;

        i = bump_tri->x1;
        bump_tri->x1 = bump_tri->x2;
        bump_tri->x2 = i;

        i = env_tri->y1;
        env_tri->y1 = env_tri->y2;
        env_tri->y2 = i;

        i = env_tri->x1;
        env_tri->x1 = env_tri->x2;
        env_tri->x2 = i;
    }

    i = tri->y2 - tri->y1;
    if (i) {
        dx12 = ((tri->x2 - tri->x1) << SHIFT_CONST) / i;

        dbx12 = ((bump_tri->x2 - bump_tri->x1) << SHIFT_CONST) / i;
        dby12 = ((bump_tri->y2 - bump_tri->y1) << SHIFT_CONST) / i;

        dex12 = ((env_tri->x2 - env_tri->x1) << SHIFT_CONST) / i;
        dey12 = ((env_tri->y2 - env_tri->y1) << SHIFT_CONST) / i;
    } else {
        dx12 = 0;

        dbx12 = 0;
        dby12 = 0;

        dex12 = 0;
        dey12 = 0;
    }

    i = tri->y3 - tri->y1;
    if (i) {
        dx13 = ((tri->x3 - tri->x1) << SHIFT_CONST) / i;

        dbx13 = ((bump_tri->x3 - bump_tri->x1) << SHIFT_CONST) / i;
        dby13 = ((bump_tri->y3 - bump_tri->y1) << SHIFT_CONST) / i;

        dex13 = ((env_tri->x3 - env_tri->x1) << SHIFT_CONST) / i;
        dey13 = ((env_tri->y3 - env_tri->y1) << SHIFT_CONST) / i;
    } else {
        dx13 = 0;

        dbx13 = 0;
        dby13 = 0;

        dex13 = 0;
        dey13 = 0;
    }

    i = tri->y3 - tri->y2;
    if (i) {
        dx23 = ((tri->x3 - tri->x2) << SHIFT_CONST) / i;

        dbx23 = ((bump_tri->x3 - bump_tri->x2) << SHIFT_CONST) / i;
        dby23 = ((bump_tri->y3 - bump_tri->y2) << SHIFT_CONST) / i;

        dex23 = ((env_tri->x3 - env_tri->x2) << SHIFT_CONST) / i;
        dey23 = ((env_tri->y3 - env_tri->y2) << SHIFT_CONST) / i;
    } else {
        dx23 = 0;

        dbx23 = 0;
        dby23 = 0;

        dex23 = 0;
        dey23 = 0;
    }

    curr_x1 = curr_x2 = tri->x1 << SHIFT_CONST;

    curr_bx1 = curr_bx2 = bump_tri->x1 << SHIFT_CONST;
    curr_by1 = curr_by2 = bump_tri->y1 << SHIFT_CONST;

    curr_ex1 = curr_ex2 = env_tri->x1 << SHIFT_CONST;
    curr_ey1 = curr_ey2 = env_tri->y1 << SHIFT_CONST;

    for (i = tri->y1; i < tri->y2; i++) {
        bump_line(curr_x1 >> SHIFT_CONST, curr_x2 >> SHIFT_CONST, i,
            curr_bx1 >> SHIFT_CONST, curr_by1 >> SHIFT_CONST,
            curr_bx2 >> SHIFT_CONST, curr_by2 >> SHIFT_CONST,
            curr_ex1 >> SHIFT_CONST, curr_ey1 >> SHIFT_CONST,
            curr_ex2 >> SHIFT_CONST, curr_ey2 >> SHIFT_CONST,
            bumpmap, envmap, buffer);

        curr_x1 += dx13;
        curr_x2 += dx12;

        curr_bx1 += dbx13;
        curr_by1 += dby13;
        curr_bx2 += dbx12;
        curr_by2 += dby12;

        curr_ex1 += dex13;
        curr_ey1 += dey13;
        curr_ex2 += dex12;
        curr_ey2 += dey12;
    }

    curr_x2 = tri->x2 << SHIFT_CONST;

    curr_bx2 = bump_tri->x2 << SHIFT_CONST;
    curr_by2 = bump_tri->y2 << SHIFT_CONST;

    curr_ex2 = env_tri->x2 << SHIFT_CONST;
    curr_ey2 = env_tri->y2 << SHIFT_CONST;

    for (i = tri->y2; i < tri->y3; i++) {
        bump_line(curr_x1 >> SHIFT_CONST, curr_x2 >> SHIFT_CONST, i,
            curr_bx1 >> SHIFT_CONST, curr_by1 >> SHIFT_CONST,
            curr_bx2 >> SHIFT_CONST, curr_by2 >> SHIFT_CONST,
            curr_ex1 >> SHIFT_CONST, curr_ey1 >> SHIFT_CONST,
            curr_ex2 >> SHIFT_CONST, curr_ey2 >> SHIFT_CONST,
            bumpmap, envmap, buffer);

        curr_x1 += dx13;
        curr_x2 += dx23;

        curr_bx1 += dbx13;
        curr_by1 += dby13;
        curr_bx2 += dbx23;
        curr_by2 += dby23;

        curr_ex1 += dex13;
        curr_ey1 += dey13;
        curr_ex2 += dex23;
        curr_ey2 += dey23;
    }
}


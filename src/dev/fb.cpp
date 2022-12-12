#include <dev/fb.hpp>
#include <init/LimineRequests.hpp>
#include <cxx/cstddef>
#include <cxx/cstdint>

limine_framebuffer **framebuffers;
limine_framebuffer *main_frm;
uint64_t framebuffer_count;

uint32_t cursorbuffer[16 * 19];
uint32_t cursorbuffersecond[16 * 19];

bool mousedrawn;

void putpix(uint64_t x, uint64_t y, uint32_t color, limine_framebuffer *frm) {
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uint64_t>(frm->address) + (x * 4) + (y * (frm->pitch / (frm->bpp / 8)) * 4)) = color;
}

void putpix(uint64_t x, uint64_t y, uint32_t r, uint32_t g, uint64_t b, limine_framebuffer *frm) {
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uint64_t>(frm->address) + (x * 4) + (y * (frm->pitch / (frm->bpp / 8)) * 4)) = (r << 16) | (g << 8) | b;
}

uint32_t getpix(uint64_t x, uint64_t y, limine_framebuffer *frm) {
    return *reinterpret_cast<uint32_t*>(reinterpret_cast<uint64_t>(frm->address) + (x * 4) + (y * (frm->pitch / (frm->bpp / 8)) * 4));
}

static void drawvertline(uint64_t x, uint64_t y, uint64_t dy, uint32_t color, limine_framebuffer *frm = main_frm) {
    for (uint64_t i = 0; i < dy; i++) putpix(x, y + i, color);
}

static void drawhorline(uint64_t x, uint64_t y, uint64_t dx, uint32_t color, limine_framebuffer *frm = main_frm) {
    for (uint64_t i = 0; i < dx; i++) putpix(x + i, y, color);
}

static void drawdiagline(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t color, limine_framebuffer *frm = main_frm) {
    uint64_t sdx = sign(x1);
    uint64_t sdy = sign(y1);
    uint64_t dxabs = abs(x1);
    uint64_t dyabs = abs(y1);
    uint64_t x = dyabs >> 1;
    uint64_t y = dxabs >> 1;
    uint64_t px = x0;
    uint64_t py = y0;
    if (dxabs >= dyabs) {
        for (size_t i = 0; i < dxabs; i++) {
            y += dyabs;
            if (y >= dxabs) {
                y -= dxabs;
                py += sdy;
            }
            px += sdx;
            putpix(px, py, color);
        }
    } else {
        for (size_t i = 0; i < dyabs; i++) {
            x += dxabs;
            if (x >= dyabs) {
                x -= dyabs;
                px += sdx;
            }
            py += sdy;
            putpix(px, py, color);
        }
    }
}

void drawline(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t color, limine_framebuffer *frm) {
    if (x0 > frm->width) x0 = frm->width - 1;
    if (x1 > frm->width) x1 = frm->width - 1;
    if (y0 > frm->height) y0 = frm->height - 1;
    if (y1 > frm->height) y1 = frm->height - 1;
    if (x0 < uint64_t(-1)) x0 = -1;
    if (x1 < uint64_t(-1)) x1 = -1;
    if (y0 < uint64_t(-1)) y0 = -1;
    if (y1 < uint64_t(-1)) y1 = -1;
    uint64_t dx = x1 - x0, dy = y1 - y0;
    if (dy == 0) {
        drawhorline(x0, y0, dx, color);
        return;
    }
    if (dx == 0) drawvertline(x0, y0, dy, color);
    drawdiagline(x0, y0, dx, dy, color);
}

void drawrectangle(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color, limine_framebuffer *frm) {
    drawline(x, y, x + w, y, color); // ¯
    drawline(x, y + h - 1, x + w, y + h - 1, color); // _
    drawline(x, y, x, y + h, color); // |*
    drawline(x + w - 1, y, x + w - 1, y + h, color); // *|
}

void drawfilledrectangle(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color, limine_framebuffer *frm) {
    for (uint64_t i = 0; i < h; i++) drawline(x, y + i, x + w, y + i, color);
}

void drawcircle(uint64_t cx, uint64_t cy, uint64_t radius, uint32_t color, limine_framebuffer *frm) {
    uint64_t x = -radius, y = 0, err = 2 - 2 * radius;
    do {
        putpix(abs(cx - x), abs(cy + y), color);
        putpix(abs(cx - y), abs(cy - x), color);
        putpix(abs(cx + x), abs(cy - y), color);
        putpix(abs(cx + y), abs(cy + x), color);
        radius = err;
        if (radius > x) err += ++x * 2 + 1;
        if (radius <= y) err += ++y * 2 + 1;
    } while (x < 0);
}

void drawfilledcircle(uint64_t cx, uint64_t cy, uint64_t radius, uint32_t color, limine_framebuffer *frm) {
    if ((radius > cx) | (radius > cy)) { cx = radius; cy = radius; };
    uint64_t x = radius;
    uint64_t y = 0;
    uint64_t xC = 1 - (radius << 1);
    uint64_t yC = 0;
    uint64_t err = 0;

    while (x >= y) {
        for (uint64_t i = cx - x; i <= cx + x; i++) {
            putpix(i, cy + y, color);
            putpix(i, cy - y, color);
        }
        for (uint64_t i = cx - y; i <= cx + y; i++) {
            putpix(i, cy + x, color);
            putpix(i, cy - x, color);
        }

        y++;
        err += yC;
        yC += 2;
        if (((err << 1) + xC) > 0) {
            x--;
            err += xC;
            xC += 2;
        }
    }
}

void clearcursor(uint8_t cursor[], point pos, limine_framebuffer *frm) {
    if (!mousedrawn) return;

    uint64_t xmax = 16, ymax = 19, dx = frm->width - pos.X, dy = frm->height - pos.Y;

    if (dx < 16) xmax = dx;
    if (dy < 19) ymax = dy;

    for (uint64_t y = 0; y < ymax; y++) {
        for (uint64_t x = 0; x < xmax; x++) {
            uint64_t bit = y * 16 + x;
            uint64_t byte = bit / 8;
            if ((cursor[byte] & (0b10000000 >> (x % 8)))) {
                putpix(pos.X + x, pos.Y + y, cursorbuffer[y * 16 + x]);
            }
        }
    }
}

void drawovercursor(uint8_t cursor[], point pos, uint32_t color, bool back, limine_framebuffer *frm) {
    uint64_t xmax = 16, ymax = 19, dx = frm->width - pos.X, dy = frm->height - pos.Y;
    if (dx < 16) xmax = dx;
    if (dy < 19) ymax = dy;
    for (uint64_t y = 0; y < ymax; y++) {
        for (uint64_t x = 0; x < xmax; x++) {
            uint64_t bit = y * 16 + x;
            uint64_t byte = bit / 8;
            if ((cursor[byte] & (0b10000000 >> (x % 8)))) {
                if (back) cursorbuffer[y * 16 + x] = getpix(pos.X + x, pos.Y + y);
                putpix(pos.X + x, pos.Y + y, color);
                if (back) cursorbuffersecond[y * 16 + x] = getpix(pos.X + x, pos.Y + y);
            }
        }
    }

    mousedrawn = true;
}

void InitFB() {
    framebuffers = FrameBufferRequest.response->framebuffers;
    main_frm = framebuffers[0];
    framebuffer_count = FrameBufferRequest.response->framebuffer_count;
}

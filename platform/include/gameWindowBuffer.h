#pragma once
#include <windows.h>

struct GameWindowBuffer
{
    unsigned char *memory = 0;
    int w = 0;
    int h = 0;

    void drawASafe(int x, int y, unsigned char r, unsigned char g, unsigned char b)
    {
        // flip the vertical logic (for some reason normal doesnt work)
        y = h - 1 - y;

        if (x >= w || y >= h || x < 0 || y < 0)
        {
            return;
        }
        memory[4 * (x + y * w) + 0] = b; // blue
        memory[4 * (x + y * w) + 1] = g; // green
        memory[4 * (x + y * w) + 2] = r; // red
        memory[4 * (x + y * w) + 3] = 0; // reserved for alignment
    }

    void clear(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0)
    {
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                memory[4 * (x + y * w) + 0] = b; // blue
                memory[4 * (x + y * w) + 1] = g; // green
                memory[4 * (x + y * w) + 2] = r; // red
                memory[4 * (x + y * w) + 3] = 0; // reserved for alignment
            }
        }
    }
};

void resetWindowBuffer(GameWindowBuffer *gameWindowBuffer, BITMAPINFO *bitmapInfo, HWND window);
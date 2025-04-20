#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <input.h>
#include <chrono>
#include <gameWindowBuffer.h>
#include <gameLogic.h>

struct WindowStuff
{
    bool running = true;

    Input input;

    BITMAPINFO bitmapInfo = {};
    GameWindowBuffer gameWindowBuffer = {};
};
WindowStuff windowStuff;

void endGame()
{
    windowStuff.running = false;
}

LRESULT windProc(HWND window, UINT msg, WPARAM wp, LPARAM lp)
{

    LRESULT rez = 0;
    bool pressed = false;
    switch (msg)
    {
    case WM_CLOSE:
        windowStuff.running = false;
        break;
    case WM_SETFOCUS:
        windowStuff.input.focused = true;
        break;
    case WM_KILLFOCUS:
        windowStuff.input.focused = false;
        break;

    // input
    case WM_KEYDOWN:
        pressed = true;
    case WM_KEYUP:
        for (int i = 0; i < Button::BUTTONS_COUNT; i++)
        {
            if (wp == Button::buttonValues[i])
            {
                processEventButton(windowStuff.input.keyBoard[i], pressed);
            }
            rez = DefWindowProc(window, msg, wp, lp);
        }
    case WM_PAINT:
    {
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(window, &Paint);

        HDC hdc = GetDC(window);

        StretchDIBits(hdc,
                      0, 0, windowStuff.gameWindowBuffer.w, windowStuff.gameWindowBuffer.h,
                      0, 0, windowStuff.gameWindowBuffer.w, windowStuff.gameWindowBuffer.h,
                      windowStuff.gameWindowBuffer.memory,
                      &windowStuff.bitmapInfo,
                      DIB_RGB_COLORS,
                      SRCCOPY);

        ReleaseDC(window, hdc);

        EndPaint(window, &Paint);
    }
    break;

    default:
        rez = DefWindowProc(window, msg, wp, lp);
    }
    return rez;
}

int main()
{
#pragma region createWindow
    WNDCLASS wc = {sizeof(WNDCLASS)};
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = GetModuleHandle(0);
    wc.lpszClassName = "MainWindowClass";
    wc.lpfnWndProc = windProc;

    if (!RegisterClassA(&wc))
    {
        std::cout << "Couldn't register window class";
        return -1;
    }

    RECT windowRect = {0, 0, 1006, 729}; // size is hardcoded becouse had some issue with window and client area (had less area to paint than said, so these values match)
    AdjustWindowRect(&windowRect, WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, FALSE);
    HWND window = CreateWindowA("MainWindowClass", "Snake", WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
                                50, 50, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, GetModuleHandle(0), nullptr);
    resetWindowBuffer(&windowStuff.gameWindowBuffer, &windowStuff.bitmapInfo, window);
#pragma endregion createWindow
    static auto previousTime = std::chrono::high_resolution_clock::now();
    RECT rect = {};
    GetWindowRect(window, &rect);
    int WINDOW_WIDTH = rect.right - rect.left;
    int WINDOW_HEIGHT = rect.bottom - rect.top;

    initGameplay();
    while (windowStuff.running)
    {
#pragma region deltaTime

        auto currentTime = std::chrono::high_resolution_clock::now();

        float deltaTime = (std::chrono::duration<float>(currentTime - previousTime).count());

        previousTime = currentTime;

        // we don't want delta time to drop too low, like let's say under 5 fps. you can set this to whatever you want
        // or remove it but I recomand keeping it
        float augmentedDeltaTime = deltaTime;
        if (augmentedDeltaTime > 1.f / 5)
        {
            augmentedDeltaTime = 1.f / 5;
        }

#pragma endregion deltaTime

        if (!windowStuff.input.focused)
        {
            resetInput(windowStuff.input);
        }

        MSG msg = {};
        while (PeekMessage(&msg, window, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
#pragma region gameLoop

        if (!gameplayFrame(deltaTime, WINDOW_WIDTH, WINDOW_HEIGHT, windowStuff.input, windowStuff.gameWindowBuffer))
        {
            windowStuff.running = false;
        }

#pragma endregion gameLoop
        processInputAfter(windowStuff.input);
        SendMessage(window, WM_PAINT, 0, 0);
    }
    closeGameLogic();
    return 0;
}
#include <input.h>
#include <windows.h>

void processEventButton(Button &b, bool newState)
{
    if (newState)
    {
        if (!b.held)
        {
            b.pressed = 1;
            b.held = 1;
            b.released = 0;
        }
    }
    else if (!newState)
    {
        b.pressed = 0;
        b.held = 0;
        b.released = 1;
    }
}
// reset input after one frame usage
void processInputAfter(Input &input)
{
    for (int i = 0; i < Button::BUTTONS_COUNT; i++)
    {
        input.keyBoard[i].pressed = 0;
        input.keyBoard[i].released = 0;
    }
}
void resetInput(Input &input)
{
    ZeroMemory(input.keyBoard, sizeof(input.keyBoard));
}
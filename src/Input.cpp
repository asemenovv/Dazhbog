#include "Input.h"

bool Input::IsKeyPressed(const Key code)
{
    return CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, static_cast<CGKeyCode>(code));
}

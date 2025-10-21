#pragma once

#include <ApplicationServices/ApplicationServices.h>

class Input {
public:
    enum class Key {
        A = 0,
        S = 1,
        D = 2,
        W = 13,
        Q = 12,
        E = 14,

        ArrowLeft  = 123,
        ArrowRight = 124,
        ArrowDown  = 125,
        ArrowUp    = 126
    };

    static bool IsKeyPressed(Key code);
};

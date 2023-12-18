#pragma once
#include <imgui.h>

template<class T>
inline T lerp(T x1, T y1, T x2, T y2, T x) {
    return y1 + (x - x1) * ((y2 - y1) / (x2 - x1));
}

inline ImVec2 lerp(const ImVec2& s, const ImVec2& e, float t) {
    return ImVec2(
        s.x + (e.x - s.x) * t,
        s.y + (e.y - s.y) * t);
}

//Lerp between automation nodes
inline UCHAR lerp(const AutomationNode& n1, const AutomationNode& n2, int currentFrame) {
    return static_cast<unsigned char>(n1.value + (currentFrame - n1.frame) * ((n2.value - n1.value) / ((float)(n2.frame - n1.frame))));
}

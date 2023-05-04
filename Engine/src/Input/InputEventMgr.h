#pragma once
#include "InputMgr.h"

namespace reality {
    class DLL_API InputEventMgr
    {
        SINGLETON(InputEventMgr)
#define INPUT_EVENT InputEventMgr::GetInst()
    private:
        std::unordered_map<int, std::vector<std::function<void()>>> push_subscribers_;
        std::unordered_map<int, std::vector<std::function<void()>>> hold_subscribers_;
        std::unordered_map<int, std::vector<std::function<void()>>> free_subscribers_;
        std::unordered_map<int, std::vector<std::function<void()>>> up_subscribers_;

    private:
        std::unordered_map<MouseButton, std::vector<std::function<void()>>> mouse_push_subscribers_;
        std::unordered_map<MouseButton, std::vector<std::function<void()>>> mouse_hold_subscribers_;
        std::unordered_map<MouseButton, std::vector<std::function<void()>>> mouse_free_subscribers_;
        std::unordered_map<MouseButton, std::vector<std::function<void()>>> mouse_up_subscribers_;
    public:
        void PollEvents();
        void SubscribeKeyEvent(int key, std::function<void()> callback, DWORD key_state);
        void SubscribeMouseEvent(MouseButton key, const std::function<void()>& callback, DWORD key_state);
    };
}

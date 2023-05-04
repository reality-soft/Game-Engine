#include "stdafx.h"
#include "InputEventMgr.h"

void reality::InputEventMgr::PollEvents()
{
    for (auto& [key, callbacks] : push_subscribers_) {
        DWORD key_state = DINPUT->GetKeyState(key);
        if (key_state == reality::KEY_PUSH) {
            for (auto& callback : callbacks) {
                callback();
            }
        }
    }

    for (auto& [key, callbacks] : free_subscribers_) {
        DWORD key_state = DINPUT->GetKeyState(key);
        if (key_state == reality::KEY_FREE) {
            for (auto& callback : callbacks) {
                callback();
            }
        }
    }

    for (auto& [key, callbacks] : hold_subscribers_) {
        DWORD key_state = DINPUT->GetKeyState(key);
        if (key_state == reality::KEY_HOLD) {
            for (auto& callback : callbacks) {
                callback();
            }
        }
    }

    for (auto& [key, callbacks] : up_subscribers_) {
        DWORD key_state = DINPUT->GetKeyState(key);
        if (key_state == reality::KEY_UP) {
            for (auto& callback : callbacks) {
                callback();
            }
        }
    }

    for (auto& [key, callbacks] : mouse_push_subscribers_) {
        DWORD key_state = DINPUT->GetMouseState(key);
        if (key_state == reality::KEY_PUSH) {
            for (auto& callback : callbacks) {
                callback();
            }
        }
    }

    for (auto& [key, callbacks] : mouse_free_subscribers_) {
        DWORD key_state = DINPUT->GetMouseState(key);
        if (key_state == reality::KEY_FREE) {
            for (auto& callback : callbacks) {
                callback();
            }
        }
    }

    for (auto& [key, callbacks] : mouse_hold_subscribers_) {
        DWORD key_state = DINPUT->GetKeyState(key);
        if (key_state == reality::KEY_HOLD) {
            for (auto& callback : callbacks) {
                callback();
            }
        }
    }

    for (auto& [key, callbacks] : mouse_up_subscribers_) {
        DWORD key_state = DINPUT->GetMouseState(static_cast<MouseButton>(key));
        if (key_state == reality::KEY_UP) {
            for (auto& callback : callbacks) {
                callback();
            }
        }
    }
}

void reality::InputEventMgr::SubscribeKeyEvent(int key, std::function<void()> callback, DWORD key_state)
{
    switch (key_state) {
    case reality::KEY_PUSH:
        push_subscribers_[key].push_back(callback);
        break;
    case reality::KEY_FREE:
        free_subscribers_[key].push_back(callback);
        break;
    case reality::KEY_HOLD:
        hold_subscribers_[key].push_back(callback);
        break;
    case reality::KEY_UP:
        up_subscribers_[key].push_back(callback);
        break;
    }
}

void reality::InputEventMgr::SubscribeMouseEvent(MouseButton key, const std::function<void()>& callback, DWORD key_state)
{
    switch (key_state) {
    case reality::KEY_PUSH:
        mouse_push_subscribers_[key].push_back(callback);
        break;
    case reality::KEY_FREE:
        mouse_free_subscribers_[key].push_back(callback);
        break;
    case reality::KEY_HOLD:
        mouse_hold_subscribers_[key].push_back(callback);
        break;
    case reality::KEY_UP:
        mouse_up_subscribers_[key].push_back(callback);
        break;
    }
}

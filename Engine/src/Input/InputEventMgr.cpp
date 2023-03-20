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

    int num_held_keys = 0;
    for (int key = 0;key < 256;key++) {
        DWORD key_state = DINPUT->GetKeyState(key);
        if (key_state == reality::KEY_HOLD) {
            num_held_keys++;
        }
    }
    for (auto& [keys, callbacks] : combination_hold_subscribers_) {
        bool all_pressed = true;
        for (auto key : keys) {
            DWORD key_state = DINPUT->GetKeyState(key);
            if (key_state != reality::KEY_HOLD) {
                all_pressed = false;
                break;
            }
        }

        if (all_pressed && num_held_keys == keys.size()) {
            for (auto callback : callbacks) {
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
}

void reality::InputEventMgr::Subscribe(vector<int> key, std::function<void()> callback, DWORD key_state)
{
    switch (key_state) {
    case reality::KEY_PUSH:
        push_subscribers_[key[0]].push_back(callback);
        break;
    case reality::KEY_FREE:
        free_subscribers_[key[0]].push_back(callback);
        break;
    case reality::KEY_HOLD:
        combination_hold_subscribers_[key].push_back(callback);
        break;
    case reality::KEY_UP:
        up_subscribers_[key[0]].push_back(callback);
        break;
    }
}

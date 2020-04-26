#include <napi.h>
#include <windows.h>
#include <string>

Napi::CallbackInfo *_info;

HHOOK _hook;

LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    auto cb = (*_info)[0].As<Napi::Function>();

    if (nCode >= 0) {
        MSLLHOOKSTRUCT *data = (MSLLHOOKSTRUCT *)lParam;

        auto x = Napi::Number::New((*_info).Env(), data->pt.x);
        auto y = Napi::Number::New((*_info).Env(), data->pt.y);

        auto mouseData = Napi::Number::New((*_info).Env(), data->mouseData);

        auto name = "";
        auto button = -1;

        if (wParam == WM_LBUTTONUP || wParam == WM_LBUTTONDOWN) {
            button = 1;
        } else if (wParam == WM_RBUTTONUP || wParam == WM_RBUTTONDOWN) {
            button = 2;
        } else if (wParam == WM_MBUTTONUP || wParam == WM_MBUTTONDOWN) {
            button = 3;
        }

        if (wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP || wParam == WM_MBUTTONUP) {
            name = "mouseup";
        } else if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN) {
            name = "mousedown";
        } else if (wParam == WM_MOUSEMOVE) {
            name = "mousemove";
        } else if (wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL) {
            name = "mousewheel";
        }

        if (name != "") {
            cb.Call((*_info).Env().Global(),
                    {Napi::String::New((*_info).Env(), name), x, y,
                     Napi::Number::New((*_info).Env(), button), mouseData});
        }
    }

    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

Napi::Boolean createMouseHook(const Napi::CallbackInfo &info) {
    _info = &(const_cast<Napi::CallbackInfo &>(info));

    _hook = SetWindowsHookEx(WH_MOUSE_LL, HookCallback, NULL, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return Napi::Boolean::New(info.Env(), true);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "createMouseHook"),
                Napi::Function::New(env, createMouseHook));

    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
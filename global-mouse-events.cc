#include <napi.h>
#include <windows.h>
#include <string>
#include <iostream>

HHOOK _hook;
Napi::ThreadSafeFunction _tsfn;
HANDLE _hThread;
boolean captureMouseMove = false;

struct MouseEventContext {
    public:
        int nCode;
        WPARAM wParam;
        LONG ptX;
        LONG ptY;
        DWORD mouseData;
};

void onMainThread(Napi::Env env, Napi::Function function, MouseEventContext *pMouseEvent) {
    auto nCode = pMouseEvent->nCode;
    auto wParam = pMouseEvent->wParam;
    auto ptX = pMouseEvent->ptX;
    auto ptY = pMouseEvent->ptY;
    auto nMouseData = pMouseEvent->mouseData;

    delete pMouseEvent;

    if (nCode >= 0) {
        Napi::HandleScope scope(env);

        auto x = Napi::Number::New(env, ptX);
        auto y = Napi::Number::New(env, ptY);

        auto mouseData = Napi::Number::New(env, nMouseData);

        auto name = "";
        auto button = -1;

        if (wParam == WM_LBUTTONUP || wParam == WM_LBUTTONDOWN) {
            button = 1;
        } else if (wParam == WM_RBUTTONUP || wParam == WM_RBUTTONDOWN) {
            button = 2;
        } else if (wParam == WM_MBUTTONUP || wParam == WM_MBUTTONDOWN) {
            button = 3;
        } else if (wParam == WM_MOUSEWHEEL) {
            button = 0;
        } else if (wParam == WM_MOUSEHWHEEL) {
            button = 1;
        }

        if (wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP || wParam == WM_MBUTTONUP) {
            name = "mouseup";
        } else if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN) {
            name = "mousedown";
        } else if (wParam == WM_MOUSEMOVE && captureMouseMove) {
            name = "mousemove";
        } else if (wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL) {
            name = "mousewheel";
        }

        if (name != "") {
            function.Call(env.Global(),
                    {Napi::String::New(env, name), x, y,
                     Napi::Number::New(env, button), mouseData});
        }
    }
}

LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    MSLLHOOKSTRUCT *data = (MSLLHOOKSTRUCT *)lParam;
    auto pMouseEvent = new MouseEventContext();
    pMouseEvent->nCode = nCode;
    pMouseEvent->wParam = wParam;
    pMouseEvent->ptX = data->pt.x;
    pMouseEvent->ptY = data->pt.y;
    pMouseEvent->mouseData = data->mouseData;

    _tsfn.NonBlockingCall(pMouseEvent, onMainThread);

    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

DWORD WINAPI MouseHookThread(LPVOID lpParam) {
    MSG msg;
    _hook = SetWindowsHookEx(WH_MOUSE_LL, HookCallback, NULL, 0);

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    _tsfn.Release();
    return 0;
}

Napi::Boolean createMouseHook(const Napi::CallbackInfo &info) {
    DWORD dwThreadID;

    _hThread = CreateThread(NULL, 0, MouseHookThread, NULL, CREATE_SUSPENDED, &dwThreadID);
    _tsfn = Napi::ThreadSafeFunction::New(
        info.Env(),
        info[0].As<Napi::Function>(),
        "WH_MOUSE_LL Hook Thread",
        512,
        1,
        [] ( Napi::Env ) { CloseHandle(_hThread); }
    );

    ResumeThread(_hThread);
    return Napi::Boolean::New(info.Env(), true);
}

void enableMouseMove(const Napi::CallbackInfo &info) {
    captureMouseMove = true;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "createMouseHook"),
                Napi::Function::New(env, createMouseHook));

    exports.Set(Napi::String::New(env, "enableMouseMove"),
                Napi::Function::New(env, enableMouseMove));            

    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
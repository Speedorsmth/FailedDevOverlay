#pragma once
#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

class Drawer {
private:
    HWND overlay = NULL;
    HDC memDC = NULL;
    HBITMAP memBmp = NULL;
    COLORREF clearColor = RGB(255, 0, 255);
    int width = 0, height = 0;
    bool ready = false;

    static LRESULT CALLBACK SimpleWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
        return DefWindowProcA(hWnd, msg, wParam, lParam);
    }

public:
    void USEFAILEDDEV(COLORREF transparencyKey = RGB(255, 0, 255)) {
        clearColor = transparencyKey;
        WNDCLASSEXA wc = { 0 };
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = SimpleWndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = "FailedDevOverlay";
        RegisterClassExA(&wc);

        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);

        overlay = CreateWindowExA(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            wc.lpszClassName, "Overlay", WS_POPUP | WS_VISIBLE,
            0, 0, width, height, NULL, NULL, wc.hInstance, NULL);

        if (!overlay) return;
        SetLayeredWindowAttributes(overlay, clearColor, 255, LWA_COLORKEY);

        HDC screenDC = GetDC(NULL);
        memDC = CreateCompatibleDC(screenDC);
        memBmp = CreateCompatibleBitmap(screenDC, width, height);
        SelectObject(memDC, memBmp);
        ReleaseDC(NULL, screenDC);
        HBRUSH br = CreateSolidBrush(clearColor);
        RECT r = { 0, 0, width, height };
        FillRect(memDC, &r, br);
        DeleteObject(br);
        ready = true;   
    }

    void AttachToWindow(HWND target) {
        if (!ready || !target) return;
        RECT r;
        if (GetWindowRect(target, &r)) {
            MoveWindow(overlay, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE);
            width = r.right - r.left;
            height = r.bottom - r.top;
        }
    }

    void RefreshPosition(HWND target) { AttachToWindow(target); }

    void ForceTopmost() { if (overlay) SetWindowPos(overlay, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); }

    void DrawBox(int x, int y, int Width, int Depth, int Height, COLORREF RGB, int Thickness) {
        if (!ready) return;
        int left = x - (Width / 2);
        HPEN pen = CreatePen(PS_SOLID, max(1, Thickness), RGB);
        HGDIOBJ oldPen = SelectObject(memDC, pen);
        HGDIOBJ oldBrush = SelectObject(memDC, GetStockObject(NULL_BRUSH));
        Rectangle(memDC, left, y - Height, left + Width, y);
        SelectObject(memDC, oldBrush);
        SelectObject(memDC, oldPen);
        DeleteObject(pen);
    }

    void FatalDrawNotification(const char* text, COLORREF Background){
		int boxWidth = 300;
		int boxHeight = 50;
        HWND noticedWnd = CreateWindowExA(WS_EX_TOPMOST, "BUTTON", NULL, WS_POPUP | WS_VISIBLE, 100, 100, boxWidth, boxHeight, NULL, NULL, NULL, NULL);
		HDC hdc = GetDC(noticedWnd);
        HBRUSH hBrush = CreateSolidBrush(Background);
        RECT rect = { 0, 0, boxWidth, boxHeight };
        FillRect(hdc, &rect, hBrush);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        TextOutA(hdc, 10, 15, text, (int)strlen(text));
        ValidateRect(noticedWnd, &rect);
        UpdateWindow(noticedWnd);
        UpdateWindow(noticedWnd);
		Present(noticedWnd);
		Sleep(200000); 
        ReleaseDC(noticedWnd, hdc);
        DestroyWindow(noticedWnd);
		DeleteObject(hBrush);
        abort();
    }

    void DrawString(int x, int y, int Width, int Depth, int Height, const char* text, COLORREF RGB) {
        if (!ready || !text) return;
        SetTextColor(memDC, RGB);
        SetBkMode(memDC, TRANSPARENT);

        HFONT hFont = CreateFontA(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, "Arial");
        HGDIOBJ oldFont = SelectObject(memDC, hFont);

        TextOutA(memDC, x - (Width / 2), y - Height - 20, text, (int)strlen(text));

        SelectObject(memDC, oldFont);
        DeleteObject(hFont);
    }

    void Present(HWND ProvHWND = NULL) {
        if (!ready) return;

        HWND trgt = (ProvHWND == NULL) ? overlay : ProvHWND;

        HDC hdc = GetDC(trgt);
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
        ReleaseDC(trgt, hdc);
    }

    void Shutdown() {
        if (memBmp) DeleteObject(memBmp);
        if (memDC) DeleteDC(memDC);
        if (overlay) DestroyWindow(overlay);
    }
};
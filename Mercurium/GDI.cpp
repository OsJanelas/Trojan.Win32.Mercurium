#include <windows.h>
#include <math.h>
#include <time.h>
#include <process.h>

// --- Estruturas e Globais ---
int sw, sh;
int stage = 1;

struct HSV { float h, s, v; };

static ULONGLONG n, r;
int randy() { return n = r, n ^= 0x8ebf635bee3c6d25, n ^= n << 5 | n >> 26, n *= 0xf3e05ca5c43e376b, r = n, n & 0x7fffffff; }

// --- Auxiliares de Cor ---
HSV RGBtoHSV(RGBQUAD rgb) {
    float r = rgb.rgbRed / 255.0f, g = rgb.rgbGreen / 255.0f, b = rgb.rgbBlue / 255.0f;
    float max = fmaxf(fmaxf(r, g), b), min = fminf(fminf(r, g), b);
    float h, s, v = max;
    float d = max - min;
    s = max == 0 ? 0 : d / max;
    if (max == min) h = 0;
    else {
        if (max == r) h = (g - b) / d + (g < b ? 6 : 0);
        else if (max == g) h = (b - r) / d + 2;
        else h = (r - g) / d + 4;
        h /= 6;
    }
    return { h * 360.0f, s, v };
}

RGBQUAD HSVtoRGB(HSV hsv) {
    float r, g, b;
    int i = (int)floor(hsv.h / 60.0f);
    float f = hsv.h / 60.0f - i;
    float p = hsv.v * (1 - hsv.s);
    float q = hsv.v * (1 - f * hsv.s);
    float t = hsv.v * (1 - (1 - f) * hsv.s);
    switch (i % 6) {
    case 0: r = hsv.v, g = t, b = p; break;
    case 1: r = q, g = hsv.v, b = p; break;
    case 2: r = p, g = hsv.v, b = t; break;
    case 3: r = p, g = q, b = hsv.v; break;
    case 4: r = t, g = p, b = hsv.v; break;
    case 5: r = hsv.v, g = p, b = q; break;
    }
    return { (BYTE)(b * 255), (BYTE)(g * 255), (BYTE)(r * 255), 0 };
}

// --- PAYLOADS ANTERIORES ---
void EfeitoEscudos(HDC hdc) {
    HICON hIcon = LoadIcon(NULL, IDI_SHIELD);
    DrawIconEx(hdc, rand() % sw, rand() % sh, hIcon, 40, 40, 0, NULL, DI_NORMAL);
}

void __cdecl TextPayload(void* arg) {
    LOGFONTW lFont = { 0 };
    lFont.lfHeight = 80; lFont.lfWeight = FW_BOLD;
    lstrcpyW(lFont.lfFaceName, L"Arial Black");
    while (stage == 4) {
        HDC hdc = GetDC(NULL);
        SetTextColor(hdc, RGB(rand() % 255, rand() % 255, rand() % 255));
        SetBkMode(hdc, TRANSPARENT);
        HFONT hFont = CreateFontIndirectW(&lFont);
        SelectObject(hdc, hFont);
        TextOutA(hdc, rand() % sw, rand() % sh, "PREPARE TO IMPACT", 17);
        DeleteObject(hFont);
        ReleaseDC(NULL, hdc);
        Sleep(100);
    }
}

DWORD WINAPI shader1(LPVOID lpParam) {
    RGBQUAD* data = (RGBQUAD*)VirtualAlloc(0, (sw * sh) * sizeof(RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    while (true) {
        if (stage != 3) { Sleep(100); continue; }
        HDC hdc = GetDC(NULL);
        HDC mdc = CreateCompatibleDC(hdc);
        HBITMAP hbm = CreateCompatibleBitmap(hdc, sw, sh);
        SelectObject(mdc, hbm);
        BitBlt(mdc, 0, 0, sw, sh, hdc, 0, 0, SRCCOPY);
        GetBitmapBits(hbm, 4 * sw * sh, data);
        int v = 0;
        for (int i = 0; sw * sh > i; ++i) {
            if (!(i % sh) && !(randy() % 110)) v = randy() % 24;
            *((BYTE*)data + 4 * i + v) -= 5;
        }
        SetBitmapBits(hbm, sw * sh * 4, data);
        BitBlt(hdc, 0, 0, sw, sh, mdc, 0, 0, SRCCOPY);
        DeleteObject(hbm); DeleteDC(mdc); ReleaseDC(NULL, hdc);
        Sleep(10);
    }
    return 0;
}

// --- NOVAS THREADS PAYLOAD 5 ---
void __cdecl PlasmaThread(void* arg) {
    int ws = sw / 4, hs = sh / 4;
    HDC dc = GetDC(NULL);
    HDC dcCopy = CreateCompatibleDC(dc);
    BITMAPINFO bmpi = { 0 };
    bmpi.bmiHeader.biSize = sizeof(bmpi);
    bmpi.bmiHeader.biWidth = ws; bmpi.bmiHeader.biHeight = -hs;
    bmpi.bmiHeader.biPlanes = 1; bmpi.bmiHeader.biBitCount = 32;
    RGBQUAD* rgbquad;
    HBITMAP bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
    SelectObject(dcCopy, bmp);
    float a = 5.0, b = 3.0;
    for (int i = 0; stage == 5; i++) {
        HDC hdc = GetDC(NULL);
        StretchBlt(dcCopy, 0, 0, ws, hs, hdc, 0, 0, sw, sh, SRCCOPY);
        int rx = rand() % ws, ry = rand() % hs;
        for (int j = 0; j < ws * hs; j++) {
            int x = j % ws, y = j / ws;
            float z = pow(x - rx, 2) / (a * a) + pow(y - ry, 2) / (b * b);
            int fx = 128.0 + (128.0 * sin(sqrt(z) / 6.0));
            HSV hsv = RGBtoHSV(rgbquad[j]);
            hsv.h = fmod(fx + i, 360.0);
            rgbquad[j] = HSVtoRGB(hsv);
        }
        StretchBlt(hdc, 0, 0, sw, sh, dcCopy, 0, 0, ws, hs, SRCCOPY);
        ReleaseDC(NULL, hdc);
        Sleep(30);
    }
    DeleteObject(bmp); DeleteDC(dcCopy); ReleaseDC(NULL, dc);
}

void __cdecl BouncingThread(void* arg) {
    int x = 10, y = 10, sx = 1, sy = 1, inc = 15;
    while (stage == 5) {
        HDC hdc = GetDC(NULL);
        x += inc * sx; y += inc * sy;
        HBRUSH br = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
        SelectObject(hdc, br);
        Ellipse(hdc, x, y, x + 120, y + 120);
        if (y >= sh - 120 || y <= 0) sy *= -1;
        if (x >= sw - 120 || x <= 0) sx *= -1;
        DeleteObject(br); ReleaseDC(NULL, hdc);
        Sleep(15);
    }
}

// --- MAIN ---
int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    srand(time(NULL));
    sw = GetSystemMetrics(0); sh = GetSystemMetrics(1);
    CreateThread(0, 0, shader1, 0, 0, 0);
    DWORD start = GetTickCount();

    while (true) {
        HDC hdc = GetDC(NULL);
        DWORD dt = GetTickCount() - start;

        // CONTROLE DE TEMPOS (ms)
        if (dt < 40000) stage = 1;
        else if (dt < 60000) stage = 2;
        else if (dt < 110000) stage = 3;
        else if (dt < 230000) { // Estágio 4 (Dura 2 minutos: 110s até 230s)
            if (stage != 4) {
                stage = 4;
                _beginthread(TextPayload, 0, NULL);
            }
        }
        else { // Estágio 5 (Inicia 2 min após o início do 4)
            if (stage != 5) {
                stage = 5;
                _beginthread(PlasmaThread, 0, NULL);
                _beginthread(BouncingThread, 0, NULL);
            }
        }

        // EXECUÇÃO
        if (stage == 1 || stage == 2) {
            int y = rand() % sh;
            BitBlt(hdc, rand() % 15, y, sw, 15, hdc, 0, y, SRCCOPY);
            BitBlt(hdc, rand() % 5, rand() % 5, sw, sh, hdc, 0, 0, SRCCOPY);
            if (stage == 2) {
                POINT pt[3] = { {30, 0}, {sw - 30, 30}, {0, sh} };
                PlgBlt(hdc, pt, hdc, 0, 0, sw, sh, 0, 0, 0);
            }
        }
        else if (stage == 3) {
            int rx = rand() % sw;
            BitBlt(hdc, rx, 10, 100, sh, hdc, rx, 0, SRCCOPY);
            DrawIcon(hdc, rand() % sw, rand() % sh, LoadIcon(NULL, (rand() % 2) ? IDI_ASTERISK : IDI_QUESTION));
        }
        else if (stage == 4) {
            HDC mdc = CreateCompatibleDC(hdc);
            HBITMAP bm = CreateCompatibleBitmap(hdc, sw, sh);
            SelectObject(mdc, bm);
            POINT pt[3] = { {rand() % 30, rand() % 30}, {sw - rand() % 30, rand() % 30}, {rand() % 30, sh - rand() % 30} };
            PlgBlt(mdc, pt, hdc, 0, 0, sw, sh, 0, 0, 0);
            HBRUSH br = CreateSolidBrush(RGB(0, 255, 0));
            SelectObject(hdc, br);
            BitBlt(hdc, rand() % 20, rand() % 20, sw, sh, mdc, rand() % 20, rand() % 20, 0x123456);
            DeleteObject(br); DeleteObject(bm); DeleteDC(mdc);
            EfeitoEscudos(hdc);
        }
        else if (stage == 5) {
            // Efeito Strobe direto no loop principal
            PatBlt(hdc, 0, 0, sw, sh, PATINVERT);
            Sleep(150);
        }

        ReleaseDC(NULL, hdc);
        Sleep(10);
    }
    return 0;
}
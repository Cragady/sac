#ifndef UNICODE
#define UNICODE
#endif

#include <cstdio>
#include <d2d1.h>
#include <dwrite.h>
#include <windows.h>
#include <windowsx.h>

#pragma comment(lib, "d2d1")

#include "odometer/basewin.hpp"
#include "odometer/util/Timer.hpp"
#include "odometer/util/WinDeltaTime.hpp"

#define SM_STR_BUFF 50
#define DEFAULT_FONT_SIZE 24

void copy_float_to_wchar(WCHAR *str, float num) {
  swprintf(str, SM_STR_BUFF, L"%f", num);
}

template <class T> void SafeRelease(T **ppT) {
  if (*ppT) {
    (*ppT)->Release();
    *ppT = NULL;
  }
}

class DPIScale {
  static float scaleX;
  static float scaleY;

public:
  static void Initialize(ID2D1Factory *pFactory) {
    FLOAT dpiX, dpiY;
    pFactory->GetDesktopDpi(&dpiX, &dpiY);
    scaleX = dpiX / 96.0f;
    scaleY = dpiY / 96.0f;
  }

  template <typename T> static D2D1_POINT_2F PixelsToDips(T x, T y) {
    return D2D1::Point2F(static_cast<float>(x) / scaleX,
                         static_cast<float>(y) / scaleY);
  }
};

float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

class MainWindow : public BaseWindow<MainWindow> {
  ID2D1Factory *pFactory;
  ID2D1HwndRenderTarget *pRenderTarget;
  ID2D1SolidColorBrush *pBrush;
  IDWriteFactory *pDWriteFactory;
  IDWriteTextFormat *pTextFormat;
  ID2D1SolidColorBrush *pBlackBrush;
  D2D1_ELLIPSE ellipse;
  D2D1_POINT_2F ptMouse;
  WinDeltaTime time;
  Timer clickPollTimer = Timer(1.0);

  int clicksInTimeSpan = 0;
  float clicksPerSecond = 0;

  void CalculateLayout() {}
  HRESULT CreateGraphicsResources();
  void DiscardGraphicsResources();
  void OnPaint();
  void Resize();
  void OnLButtonDown(int pixelX, int pixelY, DWORD flags);
  void OnLButtonUp();
  void OnMouseMove(int pixelX, int pixelY, DWORD flags);

public:
  MainWindow()
      : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL), pDWriteFactory(NULL),
        ellipse(D2D1::Ellipse(D2D1::Point2F(), 0, 0)),
        ptMouse(D2D1::Point2F()) {}

  bool keepOpen = true;

  PCWSTR ClassName() const { return L"Odometer Window Class"; }
  LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

  void BackgroundUpdate();
};

HRESULT MainWindow::CreateGraphicsResources() {
  HRESULT hr = S_OK;
  if (pRenderTarget == NULL) {
    RECT rc;
    GetClientRect(m_hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

    hr = pFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(m_hwnd, size), &pRenderTarget);

    if (SUCCEEDED(hr)) {
      const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
      hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

      if (SUCCEEDED(hr)) {
        CalculateLayout();
      }
    }
  }

  static const WCHAR msc_fontName[] = L"Verdana";
  static const FLOAT msc_fontSize = DEFAULT_FONT_SIZE;

  hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(pDWriteFactory),
                           reinterpret_cast<IUnknown **>(&pDWriteFactory));

  if (SUCCEEDED(hr)) {
    hr = pDWriteFactory->CreateTextFormat(
        msc_fontName, NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, msc_fontSize, L"", &pTextFormat);
  }

  if (SUCCEEDED(hr)) {
    hr = pRenderTarget->CreateSolidColorBrush(
      D2D1::ColorF(D2D1::ColorF::Black),
      &pBlackBrush
    );
  }

  return hr;
}

void MainWindow::DiscardGraphicsResources() {
  SafeRelease(&pRenderTarget);
  SafeRelease(&pBrush);
}

void MainWindow::OnPaint() {
  HRESULT hr = CreateGraphicsResources();
  if (SUCCEEDED(hr)) {
    PAINTSTRUCT ps;
    BeginPaint(m_hwnd, &ps);

    pRenderTarget->BeginDraw();

    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
    pRenderTarget->FillEllipse(ellipse, pBrush);

    D2D1_SIZE_F renderTargetSize = pRenderTarget->GetSize();

    static WCHAR clicksPerSecondString[SM_STR_BUFF] = L"";
    copy_float_to_wchar(clicksPerSecondString, clicksPerSecond);

    pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    pRenderTarget->DrawTextW(
        clicksPerSecondString, ARRAYSIZE(clicksPerSecondString), pTextFormat,
        D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
        pBlackBrush);

    pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
    pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    static WCHAR msg_directions[] = L"Click In Window";
    pRenderTarget->DrawTextW(
        msg_directions, ARRAYSIZE(msg_directions), pTextFormat,
        D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
        pBlackBrush);

    static WCHAR msg_cps[] = L"Clicks Per Second:";
    pRenderTarget->DrawTextW(
        msg_cps, ARRAYSIZE(msg_cps), pTextFormat,
        D2D1::RectF(0, DEFAULT_FONT_SIZE + 8, renderTargetSize.width, renderTargetSize.height),
        pBlackBrush);

    hr = pRenderTarget->EndDraw();
    if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
      DiscardGraphicsResources();
    }
    EndPaint(m_hwnd, &ps);
  }
}

void MainWindow::Resize() {
  if (pRenderTarget != NULL) {
    RECT rc;
    GetClientRect(m_hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

    pRenderTarget->Resize(size);
    CalculateLayout();
    InvalidateRect(m_hwnd, NULL, FALSE);
  }
}

void MainWindow::OnLButtonDown(int pixelX, int pixelY, DWORD flags) {
  SetCapture(m_hwnd);
  ellipse.point = ptMouse = DPIScale::PixelsToDips(pixelX, pixelY);
  ellipse.radiusX = ellipse.radiusY = 1.0f;
  InvalidateRect(m_hwnd, NULL, FALSE);
}

void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags) {
  if (flags & MK_LBUTTON) {
    const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);

    const float width = (dips.x - ptMouse.x) / 2;
    const float height = (dips.y - ptMouse.y) / 2;
    const float x1 = ptMouse.x + width;
    const float y1 = ptMouse.y + height;

    ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);

    InvalidateRect(m_hwnd, NULL, FALSE);
  }
}

void MainWindow::OnLButtonUp() { ReleaseCapture(); }

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
  MainWindow win;

  int win_width = 500;
  int win_height = 500;
  if (!win.Create(L"Odometer", WS_OVERLAPPEDWINDOW, 0, CW_USEDEFAULT,
                  CW_USEDEFAULT, win_width, win_height)) {
    return 0;
  }

  ShowWindow(win.Window(), nCmdShow);

  // Run the message loop.

  MSG msg = {};
  // while (GetMessage(&msg, NULL, 0, 0)) {
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) || win.keepOpen) {
    win.BackgroundUpdate();
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
  case WM_CREATE:
    if (FAILED(
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) {
      return -1; // Fail CreateWindowEx.
    }

    DPIScale::Initialize(pFactory);
    return 0;

  case WM_DESTROY:
    DiscardGraphicsResources();
    SafeRelease(&pFactory);
    keepOpen = false;
    PostQuitMessage(0);
    return 0;

  case WM_PAINT:
    OnPaint();
    return 0;

  case WM_SIZE:
    Resize();
    return 0;

  case WM_LBUTTONDOWN:
    OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
    clicksInTimeSpan++;
    return 0;

  case WM_LBUTTONUP:
    OnLButtonUp();
    return 0;

  case WM_MOUSEMOVE:
    OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
    return 0;
  }
  return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void MainWindow::BackgroundUpdate() {
  time.Update();
  clickPollTimer.Countdown(time.delta_time);

  if (clickPollTimer.CheckTimer()) {
    bool lastClicksNotZero = clicksPerSecond > 0;
    clicksPerSecond = (float)clicksInTimeSpan / clickPollTimer.CalcOvershoot();
    bool currentClicksNotZero = clicksPerSecond > 0;
    if (lastClicksNotZero || currentClicksNotZero) {
      SendMessage(m_hwnd, WM_PAINT, NULL, NULL);
    }
    clicksInTimeSpan = 0;
  }
}

#ifndef UNICODE
#define UNICODE
#endif

#include <cstdio>
#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
ATOM register_class_wrapper(WNDCLASS *wc, HINSTANCE hInstance,
                            const wchar_t *CLASS_NAME);
HWND create_window_ex_wrapper(const wchar_t *CLASS_NAME, HINSTANCE hInstance);

LARGE_INTEGER frequency;
LARGE_INTEGER now;
LARGE_INTEGER last;
double delta_time;

double poll_timer = 0.0;
double poll_timer_amount = 1.0;

int clicks_in_time_span = 0;
float clicks_per_second = 0;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nCmdShow) {
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&now);

  const wchar_t CLASS_NAME[] = L"Win Click Odometer";
  WNDCLASS wc = {};
  register_class_wrapper(&wc, hInstance, CLASS_NAME);

  HWND hwnd = create_window_ex_wrapper(CLASS_NAME, hInstance);

  if (hwnd == NULL) {
    return 0;
  }

  ShowWindow(hwnd, nCmdShow);

  MSG msg = {};

  poll_timer = poll_timer_amount;

  while (GetMessage(&msg, NULL, 0, 0) > 0) {

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  printf("Last calculated clicks per second: %f\n", clicks_per_second);
  return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {

  last = now;
  QueryPerformanceCounter(&now);

  delta_time =
      (now.QuadPart - last.QuadPart) * 1000.0 / frequency.QuadPart * 0.001;

  poll_timer -= delta_time;

  if (poll_timer <= 0.0) {
    clicks_per_second = clicks_in_time_span / (poll_timer_amount - poll_timer);
    poll_timer = poll_timer_amount;
    printf("clicks per rough-second: %f\n", clicks_per_second);
    clicks_in_time_span = 0;
  }
  // stop_timer += delta_time;
  // if (stop_timer >= STOP_TARGET) should_run = false;

  switch (uMsg) {
  case WM_NCLBUTTONDOWN: {
    int xPos = LOWORD(lParam); // Get the horizontal position of the click
    int yPos = HIWORD(lParam); // Get the vertical position of the click

    // Perform actions based on the click
    // printf("Global click");
    break;
  }
  case WM_LBUTTONDOWN: {
    int xPos = LOWORD(lParam); // Get the horizontal position of the click
    int yPos = HIWORD(lParam); // Get the vertical position of the click

    // Perform actions based on the click
    clicks_in_time_span++;
    break;
  }
  // Handle other messages
  case WM_QUIT:
  case WM_CLOSE:
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    // All painting occurs here, between BeginPaint and EndPaint.
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
    EndPaint(hwnd, &ps);
    break;
  }
  default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
  return 0;
}

ATOM register_class_wrapper(WNDCLASS *wc, HINSTANCE hInstance,
                            const wchar_t *CLASS_NAME) {

  wc->lpfnWndProc = WindowProc;
  wc->hInstance = hInstance;
  wc->lpszClassName = CLASS_NAME;

  return RegisterClass(wc);
}

HWND create_window_ex_wrapper(const wchar_t *CLASS_NAME, HINSTANCE hInstance) {
  int width = 500;
  int height = 500;
  return CreateWindowEx(0, CLASS_NAME, L"Clocker", WS_OVERLAPPEDWINDOW,
                        // size and pos
                        CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL,
                        hInstance, NULL);
}

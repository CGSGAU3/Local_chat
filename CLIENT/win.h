/* FILE NAME  : Win.h
 * PROGRAMMER : AU3
 * LAST UPDATE: 17.07.2022
 * PURPOSE    : WinAPI class
 */

#ifndef __win_h_
#define __win_h_

#include <windows.h>

#include <string>

 /* Buttons constants */
#define BUTTON_LEFT   30
#define BUTTON_RIGHT  47
#define BUTTON_MIDDLE 102

/* Initialization timer constant */
#define INIT_TIMER    0x3047


/* Windows class */
class Win
{
public:
  HWND hWnd;               // window handle
  BOOL IsFullScreen;       // Full Screen flag
protected:
  INT w, h;                // window size
  HINSTANCE hInstance;     // application handle
private:
  RECT fullScreenSaveRect; // FullScreen rectangle
  BOOL isInit;             // Initialization flag

static LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{
  HDC hdc;
  HINSTANCE hInst;
  PAINTSTRUCT ps;
  static HWND hBtn; // дескриптор кнопки
  static HWND hEdt1, hEdt2; // дескрипторы полей редактирования
  static HWND hStat; // дескриптор статического текста
  TCHAR StrA[20];
  int a, b, sum, Len;
  Win *win = nullptr;

  //switch (Message)
  //{
  //case WM_GETMINMAXINFO:
  //  win->onGetMinMaxInfo((MINMAXINFO *)lparam);
  //  return 0;
  //case WM_CREATE:
  //  SetWindowLongPtr(hwnd, 0, (UINT_PTR)((CREATESTRUCT *)lparam)->lpCreateParams);
  //default:
  //  win = reinterpret_cast<Win *>(GetWindowLongPtr(hwnd, 0));
  //  if (win != nullptr)
  //  {
    
      switch (Message)
      {
      case WM_CREATE: // сообщение создания окна
           SetWindowLongPtr(hwnd, 0, (UINT_PTR)((CREATESTRUCT *)lparam)->lpCreateParams);
           win = reinterpret_cast<Win *>(GetWindowLongPtr(hwnd, 0));
          hInst = ((LPCREATESTRUCT)lparam)->hInstance; // дескриптор приложения
          // Создаем и показываем первое поле редактирования
          hEdt1 = CreateWindow(("edit"), ("0"),
              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 50, 50, 60, 20,
              hwnd, 0, hInst, NULL);
          ShowWindow(hEdt1, SW_SHOWNORMAL);
          // Создаем и показываем второе поле редактирования
          hEdt2 = CreateWindow(("edit"), ("0"),
              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 150, 50, 60,
              20, hwnd, 0, hInst, NULL);
          ShowWindow(hEdt2, SW_SHOWNORMAL);
          // Создаем и показываем кнопку
          hBtn = CreateWindow(("button"), ("Рассчитать"),
              WS_CHILD | WS_VISIBLE,
              50, 100, 120, 30, hwnd, 0, hInst, NULL);
          ShowWindow(hBtn, SW_SHOWNORMAL);
          // Создаем и показываем поле текста для результата
          hStat = CreateWindow(("static"), ("0"), WS_CHILD | WS_VISIBLE,
              150, 180, 120, 20, hwnd, 0, hInst, NULL);
          ShowWindow(hStat, SW_SHOWNORMAL);
          break;
      case WM_COMMAND:  // сообщение о команде
          if (lparam == (LPARAM)hBtn)    // если нажали на кнопку
          {
              Len = GetWindowText(hEdt1, StrA, 20);
              a = strtol(StrA, nullptr, 10); // считываем число из первого поля
              Len = GetWindowText(hEdt2, StrA, 20);
              b = strtol(StrA, nullptr, 10); // считываем число из второго поля
              sum = a + b;  // находим сумму двух чисел
              SetWindowText(hStat, std::to_string(sum).c_str()); // выводим результат в статическое поле
          }
          break;
          case WM_GETMINMAXINFO:
           win->onGetMinMaxInfo((MINMAXINFO *)lparam);
           break;
      case WM_PAINT: // перерисовка окна
          hdc = BeginPaint(hwnd, &ps); // начало перерисовки
          TextOut(hdc, 50, 20, ("Введите два числа"), 18); // вывод текстовых сообщений
          TextOut(hdc, 50, 180, ("Результат:"), 10);
          EndPaint(hwnd, &ps); // конец перерисовки
          break;
      case WM_DESTROY: // закрытие окна
          PostQuitMessage(0);
          break;
      default: // обработка сообщения по умолчанию
          return DefWindowProc(hwnd, Message, wparam, lparam);
    //  }
    //}
  }
  return 0;
}

  /* Window handle function.
   * ARGUMENTS:
   *   - window handle:
   *       HWND hWnd;
   *   - message type (see WM_***):
   *       UINT Msg;
   *   - message 'word' parameter:
   *       WPARAM wParam;
   *   - message 'long' parameter:
   *       LPARAM lParam;
   * RETURNS:
   *   (LRESULT) message depende return value.
   */
  static LRESULT CALLBACK winFunc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
  {
    Win *win = nullptr;
    HDC hDC;
    PAINTSTRUCT ps;
    UINT Key = 0;

    switch (Msg)
    {
    case WM_GETMINMAXINFO:
      win->onGetMinMaxInfo((MINMAXINFO *)lParam);
      break;
    case WM_CREATE:
      SetWindowLongPtr(hWnd, 0, (UINT_PTR)((CREATESTRUCT *)lParam)->lpCreateParams);
    default:
      win = reinterpret_cast<Win *>(GetWindowLongPtr(hWnd, 0));
      if (win != nullptr)
      {
        switch (Msg)
        {
        case WM_CREATE:
          win->hWnd = hWnd;
          win->onCreate((CREATESTRUCT *)lParam);
          break;
        case WM_ACTIVATE:
          win->onActivate((UINT)LOWORD(wParam), (HWND)lParam, (BOOL)HIWORD(lParam));
          break;
        case WM_SIZE:
          win->onSize((UINT)wParam, LOWORD(lParam), HIWORD(lParam));
          break;
        case WM_ERASEBKGND:
          win->onEraseBkgnd((HDC)wParam);
          break;
        case WM_PAINT:
          hDC = BeginPaint(hWnd, &ps);
          win->onPaint(hDC, &ps);
          EndPaint(hWnd, &ps);
          break;
        case WM_TIMER:
          win->onTimer((UINT)wParam);
          break;
        case WM_MOUSEWHEEL:
          win->onMouseWheel((INT)(SHORT)LOWORD(lParam),
            (INT)(SHORT)HIWORD(lParam),
            (INT)(SHORT)HIWORD(wParam),
            (UINT)(SHORT)LOWORD(wParam));
          break;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
          switch (Msg)
          {
          case WM_LBUTTONDOWN:
            Key = BUTTON_LEFT;
            break;
          case WM_MBUTTONDOWN:
            Key = BUTTON_MIDDLE;
            break;
          case WM_RBUTTONDOWN:
            Key = BUTTON_RIGHT;
            break;
          }
          win->onButtonDown(FALSE,
            (INT)(SHORT)LOWORD(lParam),
            (INT)(SHORT)HIWORD(lParam), Key);
          break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
          switch (Msg)
          {
          case WM_LBUTTONUP:
            Key = BUTTON_LEFT;
            break;
          case WM_MBUTTONUP:
            Key = BUTTON_MIDDLE;
            break;
          case WM_RBUTTONUP:
            Key = BUTTON_RIGHT;
            break;
          }
          win->onButtonUp((INT)(SHORT)LOWORD(lParam),
            (INT)(SHORT)HIWORD(lParam), Key);
          break;
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
          switch (Msg)
          {
          case WM_LBUTTONDBLCLK:
            Key = BUTTON_LEFT;
            break;
          case WM_MBUTTONDBLCLK:
            Key = BUTTON_MIDDLE;
            break;
          case WM_RBUTTONDBLCLK:
            Key = BUTTON_RIGHT;
            break;
          }
          win->onButtonDown(TRUE,
            (INT)(SHORT)LOWORD(lParam),
            (INT)(SHORT)HIWORD(lParam), Key);
          break;
        case WM_COMMAND:
          win->onCommand(wParam, lParam);
          break;
        case WM_KEYDOWN:
          win->onKeyDown((UINT)wParam);
          break;
        case WM_KEYUP:
          win->onKeyUp((UINT)wParam);
          break;
        case WM_CLOSE:
          win->onClose();
          break;
        case WM_DESTROY:
          win->onDestroy();
          break;
        default:
          return DefWindowProc(hWnd, Msg, wParam, lParam);
        }
      }
    }
    return DefWindowProc(hWnd, Msg, wParam, lParam);
  } /* End of 'WinFunc' function */
public:
  BOOL isActive;  // Active flag
  INT mouseWheel; // Mouse wheel value

  /* Default constructor */
  Win( const std::string &name,
       HINSTANCE hInst = GetModuleHandle(nullptr) ) : hInstance(hInst), w(0), h(0), mouseWheel(0),
    IsFullScreen(FALSE), isActive(TRUE), hWnd((HWND)0), fullScreenSaveRect(), isInit(FALSE)
  {
    WNDCLASS wc;

    wc.style = CS_VREDRAW | CS_HREDRAW;          /* Window redraw style */
    wc.cbClsExtra = 0;                           /* Extra bytes for class */
    wc.cbWndExtra = sizeof(Win *);               /* To connect class with Windows */
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;     /* Background color */
    wc.hCursor = LoadCursor(nullptr, IDC_CROSS); /* Window cursor */
    wc.hIcon = LoadIcon(nullptr, IDI_ASTERISK);  /* Window icon */
    wc.lpfnWndProc = winFunc;                    /* Message peeker callback function */
    wc.lpszMenuName = nullptr;                   /* Menu resource name */
    wc.hInstance = hInstance;                    /* Program registering class descriptor */
    wc.lpszClassName = name.c_str();             /* Class name */

    /* Register class in system */
    if (!RegisterClass(&wc))
    {
      MessageBox(nullptr, "Error register window class", ERROR, MB_OK);
      return;
    }

    /* Create window */
    hWnd = CreateWindow(name.c_str(), name.c_str(), WS_OVERLAPPEDWINDOW,
      30, 30, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance,
      reinterpret_cast<VOID *>(this));

    /* Show and draw window */
    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);
  } /* End of 'Win' constructor */

  /* Win virtual destructor */
  virtual ~Win( VOID )
  {
  } /* End of 'Win' destructor */

  /* Start message loop function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID run( VOID )
  {
    MSG Msg;

    /* Message loop */
    while (TRUE)
    {
      if (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
      {
        if (Msg.message == WM_QUIT)
          break;
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
      }
      else
      {
        if (isInit)
          idle();
      }
    }
  } /* End of 'Run' function */

  /* Flip full screen function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID fullScreen( VOID )
  {
    if (!IsFullScreen)
    {
      HMONITOR hMon;
      MONITORINFOEX moninfo;
      RECT rc;

      IsFullScreen = TRUE;

      /* Save old window size and position */
      GetWindowRect(hWnd, &fullScreenSaveRect);

      /* Get closest monitor */
      hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

      /* Get monitor information */
      moninfo.cbSize = sizeof(moninfo);
      GetMonitorInfo(hMon, (MONITORINFO *)&moninfo);

      rc = moninfo.rcMonitor;
      AdjustWindowRect(&rc, GetWindowLong(hWnd, GWL_STYLE), FALSE);

      /* Expand window to full screen */
      SetWindowPos(hWnd, HWND_TOP,
        rc.left, rc.top,
        rc.right - rc.left, rc.bottom - rc.top,
        SWP_NOOWNERZORDER);
    }
    else
    {
      IsFullScreen = FALSE;

      /* Restore window size and position */
      SetWindowPos(hWnd, HWND_NOTOPMOST,
        fullScreenSaveRect.left, fullScreenSaveRect.top,
        fullScreenSaveRect.right - fullScreenSaveRect.left, fullScreenSaveRect.bottom - fullScreenSaveRect.top,
        SWP_NOOWNERZORDER);
    }
  } /* End of 'FullScreen' function */
protected:
  /******** Message crackers ********/
  /* WM_CREATE window message handle function.
   * ARGUMENTS:
   *   - structure with creation data:
   *       CREATESTRUCT *CS;
   * RETURNS:
   *   (BOOL) TRUE to continue creation window, FALSE to terminate.
   */
  virtual BOOL onCreate( CREATESTRUCT *CS );

  /* WM_DESTROY window message handle function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  virtual VOID onDestroy( VOID );

  virtual void command( HWND handle, WORD id, WORD cmd )
  {
  }

  void onCommand( WPARAM wParam, LPARAM lParam )
  {
    if (lParam != 0)
      command((HWND)lParam, LOWORD(lParam), HIWORD(wParam));
  }

  /* WM_SIZE window message handle function.
   * ARGUMENTS:
   *   - sizing flag (see SIZE_***, like SIZE_MAXIMIZED)
   *       UINT State;
   *   - new width and height of client area:
   *       INT W, H;
   * RETURNS: None.
   */
  virtual VOID onSize( UINT State, INT W, INT H );

  /* WM_ERASEBKGND window message handle function.
   * ARGUMENTS:
   *   - device context of client area:
   *       HDC hDC;
   * RETURNS:
   *   (BOOL) TRUE if background is erased, FALSE otherwise.
   */
  virtual BOOL onEraseBkgnd( HDC hDC );

  /* WM_PAINT window message handle function.
   * ARGUMENTS:
   *   - window device context:
   *       HDC hDC;
   *   - paint message structure pointer:
   *       PAINTSTRUCT *PS;
   * RETURNS: None.
   */
  virtual VOID onPaint( HDC hDC, PAINTSTRUCT *PS );

  /* WM_ACTIVATE window message handle function.
   * ARGUMENTS:
   *   - reason (WA_CLICKACTIVE, WA_ACTIVE or WA_INACTIVE):
   *       UINT Reason;
   *   - handle of active window:
   *       HWND hWndActDeact;
   *   - minimized flag:
   *       BOOL IsMinimized;
   * RETURNS: None.
   */
  virtual VOID onActivate( UINT Reason, HWND hWndActDeact, BOOL IsMinimized );

  /* WM_TIMER window message handle function.
   * ARGUMENTS:
   *   - specified the timer identifier.
   *       INT Id;
   * RETURNS: None.
   */
  virtual VOID onTimer( INT Id );

  /* WM_MOUSEWHEEL window message handle function.
   * ARGUMENTS:
   *   - mouse window position:
   *       INT X, Y;
   *   - mouse wheel relative delta value:
   *       INT Z;
   *   - mouse keys bits (see MK_*** bits constants):
   *       UINT Keys;
   * RETURNS: None.
   */
  virtual VOID onMouseWheel( INT X, INT Y, INT Z, UINT Keys );

  /* WM_GETMINMAXINFO window message handle function.
   * ARGUMENTS:
   *   - struct to get info:
   *       LPMINMAXINFO MinMax;
   * RETURNS: None.
   */
  VOID onGetMinMaxInfo( LPMINMAXINFO MinMax );

  /* WM_*BUTTonDOWN window message handle function
   * ARGUMENTS:
   *   - is click double flag:
   *       BOOL IsDouble;
   *   - mouse coordinates:
   *       INT X, Y;
   *   - button flag:
   *       UINT Key;
   * RETURNS: None.
   */
  VOID onButtonDown( BOOL IsDouble, INT X, INT Y, UINT Key );

  /* WM_*BUTTonUP window message handle function
   * ARGUMENTS:
   *   - mouse coordinates:
   *       INT X, Y;
   *   - button flag:
   *       UINT Key;
   * RETURNS: None.
   */
  VOID onButtonUp( INT X, INT Y, UINT Key );

  /* WM_CLOSE window message handle function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID onClose( VOID );

  /* WM_KEYDOWN window message handle function.
   * ARGUMENTS:
   *   - key of pressed button:
   *       UINT Key;
   * RETURNS: None.
   */
  VOID onKeyDown( UINT Key );

  VOID onKeyUp( UINT key );
protected:
  /******** Virtual callbacks functions ********/

  /* Initialization function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  virtual VOID init( VOID )
  {
  } /* End of 'Init' function */

  /* Deinitialization function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  virtual VOID close( VOID )
  {
  } /* End of 'Close' function */

  /* Resize window function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  virtual VOID resize( VOID )
  {
  } /* End of 'Resize' function */

  /* Timer function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  virtual VOID timer( VOID )
  {
  } /* End of 'Timer' function */

  /* Activate window function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  virtual VOID activate( VOID )
  {
  } /* End of 'Activate' function */

  /* Idle function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  virtual VOID idle( VOID )
  {
  } /* End of 'Idle' function */

  /* Erase background window function.
   * ARGUMENTS: 
   *   - window device context:
   *       HDC hDC;
   * RETURNS: None.
   */
  virtual VOID erase( HDC hDC )
  {

  } /* End of 'Erase' function */

  /* Paint function.
   * ARGUMENTS:
   *   - window device context:
   *       HDC hDC;
   * RETURNS: None.
   */
  virtual VOID paint( HDC hDC )
  {
  } /* End of 'Paint' function */

  virtual void keyboard( bool isUp, UINT key )
  {
  }
}; /* End of 'Win' class */

#endif /* __win_h_ */

/* END OF 'Win.h' FILE */
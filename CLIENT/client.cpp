#include "client.h"

#include <stdexcept>
#include <commctrl.h>

#pragma comment(lib, "comctl32")

#define SERVER_IPV4 "127.0.0.1"
#define SERVER_PORT 8080
#define TEXT_BOX_BUF_SIZE 4096
#define LABEL_BUF_SIZE 65536

LRESULT Client::editProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
  static Client *win = nullptr;

  switch (Msg)
  {
  case WM_USER:
    win = reinterpret_cast<Client *>(lParam);
    break;
  case WM_KEYDOWN:
    if (win != nullptr)
    {
      if (wParam == VK_SHIFT)
        win->isShift = true;
      if (wParam == VK_RETURN && !win->isShift)
        win->sendToServer();
    }
    break;
  case WM_KEYUP:
    if (win != nullptr)
    {
      if (wParam == VK_SHIFT)
        win->isShift = false;
      if (wParam == VK_RETURN && !win->isShift)
        SendMessage(win->hTextBox, WM_SETTEXT, 0, (LPARAM)"");
    }
    break;
  default:
    return DefSubclassProc(hWnd, Msg, wParam, lParam);
  }
  return DefSubclassProc(hWnd, Msg, wParam, lParam);
}

void Client::init( void )
{
  try
  {
    connectToServer();
    createUIElements();
    msgThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)messageHandler, this, NULL, NULL);
  }
  catch ( const std::exception &e )
  {
    OutputDebugString(e.what());
    exit(1);
  }
}

void Client::connectToServer( void )
{
  WSAData wsaData;
  WORD DLLVersion = MAKEWORD(2, 2);
  if (WSAStartup(DLLVersion, &wsaData) != 0)
  {
    throw std::runtime_error("WSAStartup failed");
  }

  SOCKADDR_IN addr{};
  addr.sin_addr.s_addr = inet_addr(SERVER_IPV4);
  addr.sin_port = htons(SERVER_PORT);
  addr.sin_family = AF_INET;

  connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (connection == INVALID_SOCKET)
    throw std::runtime_error("Socket creation failed");

  if (connect(connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
    throw std::runtime_error("Failed to connect to server");
}

void Client::createUIElements( void )
{
  hSend = CreateWindow("button", "Send", WS_CHILD | WS_VISIBLE,
    w * 4 / 5, h - h / 10, w / 5, h / 10, hWnd, nullptr, hInstance, 0);
  ShowWindow(hSend, SW_SHOWNORMAL);

  hLabel = CreateWindow("edit", "", WS_BORDER | ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_VSCROLL |
    ES_MULTILINE | ES_WANTRETURN | ES_READONLY,
    w / 30, h / 30, w * 28 / 30, (9 * h / 10 - h / 30), hWnd, nullptr, hInstance, 0);
  ShowWindow(hLabel, SW_SHOWNORMAL);

  hTextBox = CreateWindow("edit", "", WS_BORDER | ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
    w / 5, h - h / 10, w * 3 / 5, h / 10, hWnd, nullptr, hInstance, 0);

  SetWindowSubclass(hTextBox, (SUBCLASSPROC)editProc, 0, 0);
  SendMessage(hTextBox, WM_USER, 0, (LPARAM)this);
  SendMessage(hTextBox, EM_SETLIMITTEXT, 2048, 0);

  HFONT hFont = CreateFont(h / 26, 0, GM_COMPATIBLE, GM_COMPATIBLE,
    FW_SEMIBOLD, false, false, false, RUSSIAN_CHARSET, OUT_CHARACTER_PRECIS,
    CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_SWISS | DEFAULT_PITCH, "Consolas");

  SendMessage(hTextBox, WM_SETFONT, (WPARAM)hFont, 0);
  SendMessage(hLabel, WM_SETFONT, (WPARAM)hFont, 0);
  SendMessage(hSend, WM_SETFONT, (WPARAM)hFont, 0);
}

void Client::keyboard( bool isUp, UINT key )
{
  if (key == VK_RETURN)
    sendToServer();
}

void Client::command( HWND handle, WORD id, WORD cmd )
{
  if (handle == hSend)
  {
    if (cmd == BN_CLICKED)
      sendToServer();
  }
}

void Client::sendToServer( void )
{
  char buf[TEXT_BOX_BUF_SIZE] = {0};

  SendMessage(hTextBox, WM_GETTEXT, (WPARAM)TEXT_BOX_BUF_SIZE, (LPARAM)buf);
  std::string msg = buf;

  while (isspace((unsigned char)msg[0]))
    msg = msg.substr(1);
  while (msg.length() > 0 && isspace((unsigned char)msg[msg.length() - 1]))
    msg = msg.substr(0, msg.length() - 1);
  if (msg != "")
    send(connection, msg.c_str(), TEXT_BOX_BUF_SIZE, 0);
  SendMessage(hTextBox, WM_SETTEXT, 0, (LPARAM)"");
}

void Client::messageHandler( Client *win )
{
  char msg[TEXT_BOX_BUF_SIZE];

  while (true)
  {
    memset(msg, 0, TEXT_BOX_BUF_SIZE);
    int flag = recv(win->connection, msg, sizeof(msg), 0);

    if (flag != -1 && flag != 0)
    {
      char buf[LABEL_BUF_SIZE] = {0};

      SendMessage(win->hLabel, WM_GETTEXT, (WPARAM)LABEL_BUF_SIZE, (LPARAM)buf);
      buf[LABEL_BUF_SIZE-1] = 0;

      std::string allText = std::string(buf) + std::string(msg);

      SetWindowText(win->hLabel, allText.c_str());
      SendMessage(win->hLabel, EM_LINESCROLL, 0, 0xFFFFFF);
    }
  }
}

void Client::resize( void )
{
  HFONT hFont = (HFONT)SendMessage(hTextBox, WM_GETFONT, 0, 0);
  DeleteObject(hFont);

  hFont = CreateFont(h / 26, 0, GM_COMPATIBLE, GM_COMPATIBLE,
                     FW_SEMIBOLD, false, false, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
                     CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_SWISS | DEFAULT_PITCH, "Consolas");

  SendMessage(hTextBox, WM_SETFONT, (WPARAM)hFont, 0);
  MoveWindow(hTextBox, w / 5, h - h / 10, w * 3 / 5, h / 10, false);

  SendMessage(hLabel, WM_SETFONT, (WPARAM)hFont, 0);
  MoveWindow(hLabel, w / 30, h / 30, w * 28 / 30, (9 * h / 10 - h / 30), false);

  SendMessage(hSend, WM_SETFONT, (WPARAM)hFont, 0);
  MoveWindow(hSend, w * 4 / 5, h - h / 10, w / 5, h / 10, false);
}

void Client::paint( HDC hDC )
{
  RECT rect;

  GetClientRect(hWnd, &rect);
  FillRect(hDC, &rect, HBRUSH(COLOR_WINDOW + 1));
}

#include <windows.h>
#include <string>
#include <vector>
#include <stack>
#include <cmath>

using namespace std;

// --- State Variables ---
string expression = "";
string result = "";
double memoryValue = 0;
HWND displayExp, displayRes;
HFONT hFontSmall, hFontLarge;

// --- Custom Colors ---
COLORREF colorNumbers = RGB(80, 80, 80); // Light Gray for Numbers
COLORREF colorOps = RGB(60, 60, 60);     // Darker Gray for Operators
COLORREF colorEqual = RGB(255, 140, 0);  // Orange for Equals button
COLORREF colorText = RGB(255, 255, 255); // White Text

// --- Mathematical Logic (PEMDAS / Order of Operations) ---
int precedence(char op)
{
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/' || op == '%')
        return 2;
    return 0;
}

double applyOp(double a, double b, char op)
{
    switch (op)
    {
    case '+':
        return a + b;
    case '-':
        return a - b;
    case '*':
        return a * b;
    case '/':
        if (b == 0)
            throw 0; // Handle Division by Zero
        return a / b;
    case '%':
        return fmod(a, b);
    }
    return 0;
}

double calculateFull(string str)
{
    if (str.empty())
        return 0;
    stack<double> values;
    stack<char> ops;
    for (int i = 0; i < str.length(); i++)
    {
        if (isdigit(str[i]) || str[i] == '.')
        {
            string val = "";
            while (i < str.length() && (isdigit(str[i]) || str[i] == '.'))
                val += str[i++];
            values.push(stod(val));
            i--;
        }
        else
        {
            while (!ops.empty() && precedence(ops.top()) >= precedence(str[i]))
            {
                double v2 = values.top();
                values.pop();
                double v1 = values.top();
                values.pop();
                values.push(applyOp(v1, v2, ops.top()));
                ops.pop();
            }
            ops.push(str[i]);
        }
    }
    while (!ops.empty())
    {
        double v2 = values.top();
        values.pop();
        double v1 = values.top();
        values.pop();
        values.push(applyOp(v1, v2, ops.top()));
        ops.pop();
    }
    return values.top();
}

// Function to format the number and remove trailing zeros
string formatNum(double v)
{
    string s = to_string(v);
    s.erase(s.find_last_not_of('0') + 1, string::npos);
    if (s.back() == '.')
        s.pop_back();
    return s;
}

// --- Drawing Management (Custom Button UI) ---
void DrawCustomButton(LPDRAWITEMSTRUCT pDIS, COLORREF backColor)
{
    FillRect(pDIS->hDC, &pDIS->rcItem, CreateSolidBrush(backColor));
    SetBkMode(pDIS->hDC, TRANSPARENT);
    SetTextColor(pDIS->hDC, colorText);
    char buf[10];
    GetWindowText(pDIS->hwndItem, buf, 10);
    DrawText(pDIS->hDC, buf, -1, &pDIS->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Press effect: Invert colors when clicked
    if (pDIS->itemState & ODS_SELECTED)
    {
        InvertRect(pDIS->hDC, &pDIS->rcItem);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
        if (pDIS->CtlID == 206) // Equals Button
            DrawCustomButton(pDIS, colorEqual);
        else if (pDIS->CtlID >= 100 && pDIS->CtlID <= 109) // Number Buttons
            DrawCustomButton(pDIS, colorNumbers);
        else // Operator Buttons
            DrawCustomButton(pDIS, colorOps);
        return TRUE;
    }
    case WM_CTLCOLORSTATIC:
    {
        // Styling the display area (Static controls)
        SetTextColor((HDC)wParam, colorText);
        SetBkColor((HDC)wParam, RGB(30, 30, 30));
        return (LRESULT)CreateSolidBrush(RGB(30, 30, 30));
    }
    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        if (id >= 100 && id <= 109) // Numbers 0-9
            expression += to_string(id - 100);
        else if (id == 200)
            expression += "+";
        else if (id == 201)
            expression += "-";
        else if (id == 202)
            expression += "*";
        else if (id == 203)
            expression += "/";
        else if (id == 204)
            expression += "%";
        else if (id == 205)
            expression += ".";
        else if (id == 206) // Calculate (=)
        {
            try
            {
                result = "= " + formatNum(calculateFull(expression));
            }
            catch (...)
            {
                result = "Undefined";
            }
        }
        else if (id == 207) // Clear (C)
        {
            expression = "";
            result = "";
        }
        else if (id == 208) // Backspace
        {
            if (!expression.empty())
                expression.pop_back();
        }
        else if (id == 209) // Square Root (√)
        {
            try
            {
                double v = sqrt(calculateFull(expression));
                expression = formatNum(v);
            }
            catch (...)
            {
                result = "Error";
            }
        }

        // Update both display lines
        SetWindowText(displayExp, expression.c_str());
        SetWindowText(displayRes, result.c_str());
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hI;
    wc.lpszClassName = "CalcFinal";
    wc.hbrBackground = CreateSolidBrush(RGB(45, 45, 45));
    RegisterClass(&wc);

    // Creating the main window
    HWND hwnd = CreateWindow("CalcFinal", "Calculator", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, 100, 100, 335, 550, NULL, NULL, hI, NULL);

    // Creating display areas (History line and Result line)
    displayExp = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | SS_RIGHT, 20, 20, 280, 30, hwnd, NULL, hI, NULL);
    displayRes = CreateWindow("STATIC", "0", WS_CHILD | WS_VISIBLE | SS_RIGHT, 20, 55, 280, 45, hwnd, NULL, hI, NULL);

    // Font initialization
    hFontSmall = CreateFont(20, 0, 0, 0, FW_REGULAR, 0, 0, 0, 0, 0, 0, 0, 0, "Segoe UI");
    hFontLarge = CreateFont(35, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Segoe UI");
    SendMessage(displayExp, WM_SETFONT, (WPARAM)hFontSmall, TRUE);
    SendMessage(displayRes, WM_SETFONT, (WPARAM)hFontLarge, TRUE);

    // Button Layout Construction
    int x = 20, y = 120, w = 65, h = 50;

    // Row 1: 7, 8, 9, /
    CreateWindow("BUTTON", "7", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y, w, h, hwnd, (HMENU)107, hI, NULL);
    CreateWindow("BUTTON", "8", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 75, y, w, h, hwnd, (HMENU)108, hI, NULL);
    CreateWindow("BUTTON", "9", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 150, y, w, h, hwnd, (HMENU)109, hI, NULL);
    CreateWindow("BUTTON", "/", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 225, y, w, h, hwnd, (HMENU)203, hI, NULL);

    // Row 2: 4, 5, 6, *
    y += 60;
    CreateWindow("BUTTON", "4", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y, w, h, hwnd, (HMENU)104, hI, NULL);
    CreateWindow("BUTTON", "5", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 75, y, w, h, hwnd, (HMENU)105, hI, NULL);
    CreateWindow("BUTTON", "6", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 150, y, w, h, hwnd, (HMENU)106, hI, NULL);
    CreateWindow("BUTTON", "*", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 225, y, w, h, hwnd, (HMENU)202, hI, NULL);

    // Row 3: 1, 2, 3, -
    y += 60;
    CreateWindow("BUTTON", "1", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y, w, h, hwnd, (HMENU)101, hI, NULL);
    CreateWindow("BUTTON", "2", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 75, y, w, h, hwnd, (HMENU)102, hI, NULL);
    CreateWindow("BUTTON", "3", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 150, y, w, h, hwnd, (HMENU)103, hI, NULL);
    CreateWindow("BUTTON", "-", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 225, y, w, h, hwnd, (HMENU)201, hI, NULL);

    // Row 4: 0, ., %, +
    y += 60;
    CreateWindow("BUTTON", "0", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y, w, h, hwnd, (HMENU)100, hI, NULL);
    CreateWindow("BUTTON", ".", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 75, y, w, h, hwnd, (HMENU)205, hI, NULL);
    CreateWindow("BUTTON", "%", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 150, y, w, h, hwnd, (HMENU)204, hI, NULL);
    CreateWindow("BUTTON", "+", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 225, y, w, h, hwnd, (HMENU)200, hI, NULL);

    // Row 5: Sqrt, Clear, Backspace
    y += 60;
    CreateWindow("BUTTON", "√", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y, w, h, hwnd, (HMENU)209, hI, NULL);
    CreateWindow("BUTTON", "C", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 75, y, 105, h, hwnd, (HMENU)207, hI, NULL);
    CreateWindow("BUTTON", "⌫", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x + 190, y, 100, h, hwnd, (HMENU)208, hI, NULL);

    // Row 6: Equals (Large Orange Button)
    y += 60;
    CreateWindow("BUTTON", "=", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y, 290, 50, hwnd, (HMENU)206, hI, NULL);

    ShowWindow(hwnd, nS);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
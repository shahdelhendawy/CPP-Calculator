#include <graphics.h>
#include <string>
#include <stack>
#include <cmath>
using namespace std;

string expression = "";
string result = "";

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    return 0;
}

double applyOp(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': if (b == 0) throw 0; return a / b;
        case '%': return fmod(a, b);
    }
    return 0;
}

double calculateFull(string str) {
    stack<double> values;
    stack<char> ops;
    for (int i = 0; i < str.length(); i++) {
        if (isdigit(str[i]) || str[i] == '.') {
            string val = "";
            while (i < str.length() && (isdigit(str[i]) || str[i] == '.'))
                val += str[i++];
            values.push(stod(val));
            i--;
        } else {
            while (!ops.empty() && precedence(ops.top()) >= precedence(str[i])) {
                double v2 = values.top(); values.pop();
                double v1 = values.top(); values.pop();
                values.push(applyOp(v1, v2, ops.top()));
                ops.pop();
            }
            ops.push(str[i]);
        }
    }
    while (!ops.empty()) {
        double v2 = values.top(); values.pop();
        double v1 = values.top(); values.pop();
        values.push(applyOp(v1, v2, ops.top()));
        ops.pop();
    }
    return values.top();
}

string formatNum(double v) {
    string s = to_string(v);
    s.erase(s.find_last_not_of('0') + 1, string::npos);
    if (s.back() == '.') s.pop_back();
    return s;
}

void drawButton(int x, int y, int w, int h, const char* label, int color) {
    setfillstyle(SOLID_FILL, color);
    bar(x, y, x+w, y+h);
    setcolor(WHITE);
    rectangle(x, y, x+w, y+h);
    int tw = textwidth(const_cast<char*>(label));
    int th = textheight(const_cast<char*>(label));
    outtextxy(x + w/2 - tw/2, y + h/2 - th/2, const_cast<char*>(label));
}

bool isInside(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x+w && my >= y && my <= y+h);
}

int main() {
    initwindow(350, 550, "Calculator");

    setcolor(WHITE);
    rectangle(20, 20, 310, 80);

    int x = 20, y = 120, w = 65, h = 50;

    drawButton(x, y, w, h, "7", DARKGRAY);
    drawButton(x+75, y, w, h, "8", DARKGRAY);
    drawButton(x+150, y, w, h, "9", DARKGRAY);
    drawButton(x+225, y, w, h, "/", LIGHTGRAY);

    y += 60;
    drawButton(x, y, w, h, "4", DARKGRAY);
    drawButton(x+75, y, w, h, "5", DARKGRAY);
    drawButton(x+150, y, w, h, "6", DARKGRAY);
    drawButton(x+225, y, w, h, "*", LIGHTGRAY);

    y += 60;
    drawButton(x, y, w, h, "1", DARKGRAY);
    drawButton(x+75, y, w, h, "2", DARKGRAY);
    drawButton(x+150, y, w, h, "3", DARKGRAY);
    drawButton(x+225, y, w, h, "-", LIGHTGRAY);

    y += 60;
    drawButton(x, y, w, h, "0", DARKGRAY);
    drawButton(x+75, y, w, h, ".", DARKGRAY);
    drawButton(x+150, y, w, h, "%", LIGHTGRAY);
    drawButton(x+225, y, w, h, "+", LIGHTGRAY);

    y += 60;
    drawButton(x, y, w, h, "√", LIGHTGRAY);
    drawButton(x+75, y, 105, h, "C", LIGHTGRAY);
    drawButton(x+190, y, 100, h, "<-", LIGHTGRAY);

    y += 60;
    drawButton(x, y, 290, 50, "=", COLOR(255,140,0));

    while (true) {
        if (ismouseclick(WM_LBUTTONDOWN)) {
            int mx, my;
            getmouseclick(WM_LBUTTONDOWN, mx, my);

            if (isInside(mx,my,20,120,65,50)) expression += "7";
            else if (isInside(mx,my,95,120,65,50)) expression += "8";
            else if (isInside(mx,my,170,120,65,50)) expression += "9";
            else if (isInside(mx,my,245,120,65,50)) expression += "/";

            else if (isInside(mx,my,20,180,65,50)) expression += "4";
            else if (isInside(mx,my,95,180,65,50)) expression += "5";
            else if (isInside(mx,my,170,180,65,50)) expression += "6";
            else if (isInside(mx,my,245,180,65,50)) expression += "*";

            else if (isInside(mx,my,20,240,65,50)) expression += "1";
            else if (isInside(mx,my,95,240,65,50)) expression += "2";
            else if (isInside(mx,my,170,240,65,50)) expression += "3";
            else if (isInside(mx,my,245,240,65,50)) expression += "-";

            else if (isInside(mx,my,20,300,65,50)) expression += "0";
            else if (isInside(mx,my,95,300,65,50)) expression += ".";
            else if (isInside(mx,my,170,300,65,50)) expression += "%";
            else if (isInside(mx,my,245,300,65,50)) expression += "+";

            else if (isInside(mx,my,20,360,65,50)) {
                try {
                    double v = sqrt(calculateFull(expression));
                    expression = formatNum(v);
                } catch (...) { result = "Error"; }
            }
            else if (isInside(mx,my,95,360,105,50)) { expression = ""; result = ""; }
            else if (isInside(mx,my,200,360,100,50)) { if(!expression.empty()) expression.pop_back(); }

            else if (isInside(mx,my,20,420,290,50)) {
                try { result = "= " + formatNum(calculateFull(expression)); }
                catch (...) { result = "Undefined"; }
            }

            setfillstyle(SOLID_FILL, BLACK);
            bar(21,21,309,79);
            setcolor(WHITE);
            outtextxy(25,30,const_cast<char*>(expression.c_str()));
            outtextxy(25,55,const_cast<char*>(result.c_str()));
        }
    }

    getch();
    closegraph();
    return 0;
}

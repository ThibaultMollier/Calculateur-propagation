#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <math.h>

# define BUTTON_ID 1000
#define DATA_GRAPH 1
#define DATA_TAB 2

HWND* createEditField(HWND m_hwnd);
int GetTextSize (LPSTR st);
void DrawGraph(HDC hdc);
float getmax(float val1[], float val2[], int size);
float getmin(float val1[], float val2[], int size);
void AddMenus(HWND hwnd);

const LPSTR TextParam[7] = {"e(t=0+):","e(t=0-):","Zg:","Zc:","Zl:","Tc:","L:"};
const LPSTR TextUnits[7] = {"V","V","ohms","ohms","ohms","ps/mm","mm"};
const int EDIT_ID[7] = {1001,1002,1003,1004,1005,1006,1007};

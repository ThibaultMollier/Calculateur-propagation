#include "main.h"

HINSTANCE hInst;
HMENU hMenu;
float Va[50], Vb[50], fin;

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static PAINTSTRUCT ps;
    static HDC hdc;
    
	static HWND hwndButton, *hwndEdit;
	static float tau, Zc, Zg, Zl , Eh, Eb;
	static float Ga, Gb, init, dVa, dVb;
	TCHAR buf[100] = "NULL";	
	
	switch(Message) {
		
		/* Upon destruction, tell the main thread to stop */
		case WM_DESTROY: 
			PostQuitMessage(0);
			break;
		
		case WM_CREATE : 
			AddMenus(hwnd);
			hwndButton = CreateWindow( 
				TEXT("BUTTON"), TEXT("OK"),
			    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
			    40,         // x position 
			    250,         // y position 
			    50,        // Button width
			    25,        // Button height
			    hwnd,     // Parent window
			    (HMENU) BUTTON_ID,       // No menu.
			    hInst, 
			    NULL);      // Pointer not needed.
			    
			    hwndEdit = createEditField(hwnd);
			    for(int i = 0 ; i < 50 ; i++){
					Va[i] = 0;
					Vb[i] = 0;
				}
			    
			    
			break;
		case WM_COMMAND:
			switch(wParam)
			{
				case BUTTON_ID:
					
					for(int i = 0 ; i < 50 ; i++){
						Va[i] = 0;
						Vb[i] = 0;
					}
					
					tau = GetDlgItemInt(hwnd,EDIT_ID[5],NULL,FALSE) * GetDlgItemInt(hwnd,EDIT_ID[6],NULL,FALSE);
					Eh = GetDlgItemInt(hwnd,EDIT_ID[0],NULL,TRUE);
					Eb = GetDlgItemInt(hwnd,EDIT_ID[1],NULL,TRUE);
					Zg = GetDlgItemInt(hwnd,EDIT_ID[2],NULL,FALSE);
					Zc = GetDlgItemInt(hwnd,EDIT_ID[3],NULL,FALSE);
					Zl = GetDlgItemInt(hwnd,EDIT_ID[4],NULL,FALSE);
					
					Ga = (Zg-Zc)/(Zg+Zc);
					Gb = (Zl-Zc)/(Zl+Zc);
					init = (Zl/(Zl+Zg))*Eb;
					fin = (Zl/(Zl+Zg))*Eh;
					dVa = (Zc/(Zc+Zg))*(Eh-Eb);
					dVb = 0;
					
					Va[0] = init;
					Vb[0] = init;
					Vb[1] = init;
					
					
					for(int i = 1 ; i < 49 ; i++){
				        Va[i] = Va[i-1] + dVa + dVb;
				        Va[i+1] = Va[i];
				        i++;
				        dVb = dVa*Gb;
				        Vb[i] = Vb[i-1] + dVb + dVa;
				        Vb[i+1]=Vb[i];
				        dVa = dVb*Ga;
					}
					
				    InvalidateRgn(hwnd,NULL,TRUE);
				    UpdateWindow(hwnd);

					break;
				case DATA_TAB :
					
					
					break;
			}
			break;
		case WM_PAINT:
	        hdc = BeginPaint(hwnd, &ps);
	        for(int i = 0 ; i < 7 ; i++){
	        	TextOut(hdc,10, 50+i*25,TextParam[i], GetTextSize(TextParam[i]));
	        	TextOut(hdc,105, 50+i*25,TextUnits[i], GetTextSize(TextUnits[i]));
			}
			
			_stprintf(buf, _T("tau : %3.1f ps"), tau);
			TextOut(hdc,20, 300,buf, GetTextSize(buf));
			_stprintf(buf, _T("Ga : %2.2f"),Ga);
			TextOut(hdc,20, 320,buf, GetTextSize(buf));
			_stprintf(buf, _T("Gb : %2.2f"), Gb);
			TextOut(hdc,20, 340,buf, GetTextSize(buf));
			_stprintf(buf, _T("V(t=0-) : %2.2f V"), init);
			TextOut(hdc,20, 360,buf, GetTextSize(buf));
			_stprintf(buf, _T("V(t=inf) : %2.2f V"), fin);
			TextOut(hdc,20, 380,buf, GetTextSize(buf));
			
			DrawGraph(hdc);
			
	        EndPaint(hwnd, &ps);
	        break;
		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */
	
	hInst = hInstance;

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","CEM",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		1080, /* width */
		540, /* height */
		NULL,NULL,hInstance,NULL);


	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	/*
		This is the heart of our program where all input is processed and 
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	ShowWindow(hwnd,nCmdShow);
    UpdateWindow(hwnd);
	
	
	while(GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	return msg.wParam;
}


HWND* createEditField(HWND m_hwnd){
	HWND hwndEdit[7];
	
	for(int i = 0 ; i < 7 ; i++){
		hwndEdit[i] = CreateWindow( 
				TEXT("EDIT"), TEXT(""),
			    WS_VISIBLE | WS_CHILD | WS_BORDER ,  // Styles 
			    60,         // x position 
			    50 + (i*25),         // y position 
			    40,        //  width
			    20,        //  height
			    m_hwnd,     // Parent window
			    (HMENU) EDIT_ID[i],
			    hInst, 
			    NULL);      // Pointer not needed.
	}
	
	return hwndEdit;
}

int GetTextSize (LPSTR st)
{
    for (int i = 0; ;i++)
    {
        if (st[i] == '\0')
            return i;
    }
}

float getmax(float val1[], float val2[], int size){
	float max = -10;
	
	for(int i = 0;i < size; i++){
		if(val1[i] > max){
			max = val1[i];
		}
		if(val2[i] > max){
			max = val2[i];
		}
	}
	return max;
}

float getmin(float val1[], float val2[], int size){
	float min = 10;
	
	for(int i = 0;i < size; i++){
		if(val1[i] < min){
			min = val1[i];
		}
		if(val2[i] < min){
			min = val2[i];
		}
	}
	return min;
}

void AddMenus(HWND hwnd) {

    HMENU hMenubar;
    hMenubar = CreateMenu();
    AppendMenuW(hMenubar, MF_STRING, DATA_TAB, L"&Tableau");
    SetMenu(hwnd, hMenubar);
}

void DrawGraph(HDC hdc){
	float max = getmax(Va,Vb,50);
	float min = getmin(Va,Vb,50);
	float res = (max-min)/420.0;
	int zero = 20+(max/res);
	
	TCHAR txt[20] = "NULL";
	HPEN hPen1 = CreatePen(PS_SOLID, 1, RGB(160, 160, 160));
	HPEN hPenA = CreatePen(PS_SOLID, 1, RGB(0, 0, 250));
	HPEN hPenB = CreatePen(PS_SOLID, 1, RGB(0, 250, 0));
	
	Rectangle(hdc, 200, 10, 1000, 450);
	MoveToEx(hdc,200,zero,NULL);
	LineTo(hdc,1000,zero);
	
	HPEN holdPen = SelectObject(hdc, hPen1);
	MoveToEx(hdc,200,20,NULL);
	LineTo(hdc,1000,20);
	
	MoveToEx(hdc,200,440,NULL);
	LineTo(hdc,1000,440);
	
	MoveToEx(hdc,200,zero-fin/res,NULL);
	LineTo(hdc,1000,zero-fin/res);
	
	SelectObject(hdc, hPenA);
	int y = zero - Va[0]/res;
	int x = 200;	
	for(int i = 0 ; i < 20 ; i++){
		
		MoveToEx(hdc,x,y,NULL);
		y = zero - Va[i]/res;
		LineTo(hdc,x,y);	
		MoveToEx(hdc,x,y,NULL);
		x = (i+1)*40 + 200;
		LineTo(hdc,x,y);
		
	}
	
	SelectObject(hdc, hPenB);
	y = zero - Vb[0]/res;
	x = 200;	
	for(int i = 0 ; i < 20 ; i++){
		
		MoveToEx(hdc,x,y,NULL);
		y = zero - Vb[i]/res;
		LineTo(hdc,x,y);	
		MoveToEx(hdc,x,y,NULL);
		x = (i+1)*40 + 200;
		LineTo(hdc,x,y);
		
	}
	
	TextOut(hdc,180, zero,"0", 2);
	_stprintf(txt, _T("%3.1f V"), max);
	TextOut(hdc,220, 12,txt, GetTextSize(txt));
	_stprintf(txt, _T("%3.1f V"), min);
	TextOut(hdc,220, 432,txt, GetTextSize(txt));
	_stprintf(txt, _T("%3.1f V"), fin);
	TextOut(hdc,950, (zero-fin/res)-8,txt, GetTextSize(txt));
	
}

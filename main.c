#include "main.h"

WNDPROC oldEditProc;
HINSTANCE hInst;
int sim = 21;
float *Va, *Vb, *dVa, *dVb, fin,tau;
HWND hwndEdit[7];
HWND hwnd, hwdDureeEdit;

LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam) { 
			case 0x09: 
				for(int i=0;i<7;i++){
		    		if(wnd == hwndEdit[i]){
						SetFocus(hwndEdit[(i+1)%7]);	
					}      	
				}
				break;
		    case 0x0D:  
		    	sim = GetDlgItemInt(hwnd,EDIT_DUREE_ID,NULL,FALSE) + 1;
		    	SendMessage(hwnd, WM_COMMAND, BUTTON_ID, 0);                 
	        	break; 
		}
		break;
	default:
	     return CallWindowProc(oldEditProc, wnd, msg, wParam, lParam);
	}
	return 0;
}

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static PAINTSTRUCT ps;
    static HDC hdc;
    
	static HWND hwndButton, hwdDataEdit;
	static float Zc, Zg, Zl;
	static int Eh,Eb;
	static float Ga, Gb, init, RefA, RefB, tStab = -1;
	TCHAR buf[100] = "NULL";
	BITMAP bitmap;
	static HBITMAP hBitmap;
	HDC hdcMem;
    HGDIOBJ oldBitmap;
	
	switch(Message) {
		
		/* Upon destruction, tell the main thread to stop */
		case WM_DESTROY: 
			free(Va);
			free(Vb);
			free(dVa);
			free(dVb);
			PostQuitMessage(0);
			break;
		
		case WM_CREATE : 
		
			Va = (float*)malloc(sizeof(float)*sim);
		    Vb = (float*)malloc(sizeof(float)*sim);
		    dVb = (float*)malloc(sizeof(float)*sim);
			dVa = (float*)malloc(sizeof(float)*sim);	
					
			for(int i = 0 ; i < sim ; i++){
				Va[i] = 0;
				Vb[i] = 0;
			}
					
			hBitmap = (HBITMAP) LoadImageW(NULL, L".\\sch.bmp", 
                        IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                        
			hwndButton = CreateWindow( 
				TEXT("BUTTON"), TEXT("OK"),
			    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
			    60,         // x position 
			    310,         // y position 
			    70,        // Button width
			    35,        // Button height
			    hwnd,     // Parent window
			    (HMENU) BUTTON_ID,       // No menu.
			    hInst, 
			    NULL);      // Pointer not needed.
			    
		    createEditField(hwnd);								
			hwdDataEdit = CreateWindow( 
				TEXT("EDIT"), TEXT(""),
			    WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER | ES_READONLY, // Styles 
			    1000,         // x position 
			    10 ,         // y position 
			    325,        //  width
			    457,        //  height
			    hwnd,     // Parent window
			    (HMENU) EDIT_DATA_ID,
			    hInst, 
			    NULL);      // Pointer not needed.	
				
			hwdDureeEdit = CreateWindow( 
				TEXT("EDIT"), TEXT("20"),
			    WS_CHILD | WS_VISIBLE |  WS_BORDER | ES_NUMBER, // Styles 
			    500,         // x position 
			    460 ,         // y position 
			    25,        //  width
			    20,        //  height
			    hwnd,     // Parent window
			    (HMENU) EDIT_DUREE_ID,
			    hInst, 
			    NULL);      // Pointer not needed.	
			oldEditProc = (WNDPROC)SetWindowLongPtr(hwdDureeEdit, GWLP_WNDPROC, (LONG_PTR)subEditProc);	    
			break;			
		case WM_COMMAND:

			switch(wParam)
			{	
				case BUTTON_ID:	
					Va = (float*)realloc(Va,sizeof(float)*sim);
				    Vb = (float*)realloc(Vb,sizeof(float)*sim);
				    dVb = (float*)realloc(dVb,sizeof(float)*sim);
					dVa = (float*)realloc(dVa,sizeof(float)*sim);	
					
					for(int i = 0 ; i < sim ; i++){
						Va[i] = 0;
						Vb[i] = 0;
					}
					tStab = -1;
					
					/* Get parameters from edit field */
					tau = GetDlgItemInt(hwnd,EDIT_ID[5],NULL,FALSE) * GetDlgItemInt(hwnd,EDIT_ID[6],NULL,FALSE);
					Eh = GetDlgItemInt(hwnd,EDIT_ID[0],NULL,TRUE);
					Eb = GetDlgItemInt(hwnd,EDIT_ID[1],NULL,TRUE);
					Zg = GetDlgItemInt(hwnd,EDIT_ID[2],NULL,FALSE);
					Zc = GetDlgItemInt(hwnd,EDIT_ID[3],NULL,FALSE);
					Zl = GetDlgItemInt(hwnd,EDIT_ID[4],NULL,FALSE);
					
					Ga = (Zg-Zc)/(Zg+Zc);
					Gb = (Zl-Zc)/(Zl+Zc);
					init = (Zl/(Zl+Zg))*(Eb/1000);
					fin = (Zl/(Zl+Zg))*(Eh/1000);
					RefA = (Zc/(Zc+Zg))*(Eh-Eb)/1000;
					RefB = 0;
					
					dVa[0] = 0;
					dVb[0] = 0;
					
					Va[0] = init;
					Vb[0] = init;
					Vb[1] = init;
					
					/* Compute Va and Vb value */
					for(int i = 1 ; i < sim ; i++){
				        Va[i] = Va[i-1] + RefA + RefB;
				        dVa[i] = RefA + RefB;
				        Va[i+1] = Va[i];
				        dVa[i+1] = 0; 
				        i++;
				        RefB = RefA*Gb;
				        Vb[i] = Vb[i-1] + RefB + RefA;
				        dVb[i] = RefA + RefB;
				        Vb[i+1]=Vb[i];
				        dVb[i+1]=0;
				        
				        RefA = RefB*Ga;
					}
					Va[sim] = Va[sim-1] + RefA + RefB;
					
					/* Compute the stabilization time (5%)*/
					for(int i = 0 ; i < sim ; i++){
						if(((fin<0?-1:1)*Vb[i] < fabs(fin + fin*0.05)) && ((fin<0?-1:1)*Vb[i] > fabs(fin - fin*0.05))){
				        	tStab = (i-1)*tau;
				        	break;
						}
					}
					    
					_stprintf(buf, _T(" t(ns)\tVa(V)\tdVa(V)\tdVb(V)\tVb(V) \n"));
					SendMessageA(hwdDataEdit, WM_SETTEXT,0,(LPARAM) buf);
					
					/* Print raw data to edit*/
					for(int i = 0 ; i < sim ; i++){
						_stprintf(buf, _T("%3.2f\t%3.3f\t%3.3f\t%3.3f\t%3.3f\n"), (tau*(i-1))/1000, Va[i], dVa[i], dVb[i], Vb[i] );
						int index = GetWindowTextLength (hwdDataEdit);
						SetFocus (hwdDataEdit); // set focus
						SendMessageA(hwdDataEdit, EM_SETSEL, (WPARAM)index, (LPARAM)index); // set selection - end of text
						SendMessageA(hwdDataEdit, EM_REPLACESEL, 0, (LPARAM) buf); // append!
					}
					
					/* Redraw all the window */
				    InvalidateRgn(hwnd,NULL,TRUE);
				    UpdateWindow(hwnd);

					break;
			}
			break;
		case WM_PAINT:
	        hdc = BeginPaint(hwnd, &ps);
	        
	        TextOut(hdc,10,530,"Mollier T.", 11);
	        
	        for(int i = 0 ; i < 7 ; i++){
	        	TextOut(hdc,30, 130+i*25,TextParam[i], GetTextSize(TextParam[i]));
	        	TextOut(hdc,130, 130+i*25,TextUnits[i], GetTextSize(TextUnits[i]));
			}
			
			TextOut(hdc,360, 460,"Durée de simulation :", 22);
			TextOut(hdc,530, 460,"tau", 4);
			_stprintf(buf, _T("tau : %3.1f ps"), tau);
			TextOut(hdc,50, 350,buf, GetTextSize(buf));
			_stprintf(buf, _T("Ga : %3.3f"),Ga);
			TextOut(hdc,50, 370,buf, GetTextSize(buf));
			_stprintf(buf, _T("Gb : %3.3f"), Gb);
			TextOut(hdc,50, 390,buf, GetTextSize(buf));
			_stprintf(buf, _T("V(t=0-) : %3.3f V"), init);
			TextOut(hdc,50, 410,buf, GetTextSize(buf));
			_stprintf(buf, _T("V(t=inf) : %3.3f V"), fin);
			TextOut(hdc,50, 430,buf, GetTextSize(buf));
			_stprintf(buf, _T("Stabilisat° : %3.1f ns"), tStab/1000);
			TextOut(hdc,50, 450,buf, GetTextSize(buf));
			DrawGraph(hdc);
			
			hdcMem = CreateCompatibleDC(hdc);
            oldBitmap = SelectObject(hdcMem, hBitmap);

            GetObject(hBitmap, sizeof(bitmap), &bitmap);
            BitBlt(hdc, 5, 25, bitmap.bmWidth, bitmap.bmHeight, 
                 hdcMem, 0, 0, SRCCOPY);

            SelectObject(hdcMem, oldBitmap);
            DeleteDC(hdcMem);

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

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Calculateur propagation",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		1350, /* width */
		600, /* height */
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

/*
	Create seven edit field to allow user to enter parameter	
*/
void createEditField(HWND m_hwnd){
	
	
	for(int i = 0 ; i < 7 ; i++){
		hwndEdit[i] = CreateWindow( 
				TEXT("EDIT"), TEXT(""),
			    WS_VISIBLE | WS_CHILD | WS_BORDER ,  // Styles 
			    80,         // x position 
			    130 + (i*25),         // y position 
			    50,        //  width
			    20,        //  height
			    m_hwnd,     // Parent window
			    (HMENU) EDIT_ID[i],
			    hInst, 
			    NULL);      // Pointer not needed.
		oldEditProc = (WNDPROC)SetWindowLongPtr(hwndEdit[i], GWLP_WNDPROC, (LONG_PTR)subEditProc);
	}
}

/*
	Return the size of a string
*/
int GetTextSize (LPSTR st)
{
    for (int i = 0; ;i++)
    {
        if (st[i] == '\0')
            return i;
    }
}

/*
	Return the max value of val1 or val2
*/
float getmax(float val1[], float val2[], int size){
	float max ;
	
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

/*
	Return the min value of val1 or val2
*/
float getmin(float val1[], float val2[], int size){
	float min ;
	
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

/*
	Draw the graph 
*/
void DrawGraph(HDC hdc){
	float max = getmax(Va,Vb,sim);
	float min = getmin(Va,Vb,sim);
	float res = (max-min)/420.0;
	int xres = 800/sim;
	int zero = 20+(max/res);
	
	TCHAR txt[20] = "NULL";
	HPEN hPenDefault = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HPEN hPen1 = CreatePen(PS_SOLID, 1, RGB(250, 250, 50));
	HPEN hPent = CreatePen(PS_DASHDOT, 1, RGB(200, 200, 200));
	HPEN hPenA = CreatePen(PS_SOLID, 1, RGB(0, 0, 250));
	HPEN hPenB = CreatePen(PS_SOLID, 1, RGB(250, 0, 0));
	
	Rectangle(hdc, 200, 10, 1000, 450);
	
	
	if(tau){
		int y = 0;
		int x = 200;
		
		SelectObject(hdc, hPenDefault);
		MoveToEx(hdc,200,zero,NULL);
		LineTo(hdc,1000,zero);
		
		SelectObject(hdc, hPent);
		for(int i = min ; i < max ; i++){
			if(i!=0){
				y = zero - (i/res);
				MoveToEx(hdc,200,y,NULL);
				LineTo(hdc,1000,y);
				_stprintf(txt, "%dV", i);
				TextOut(hdc,x + 10, y - 8,txt, GetTextSize(txt));
			}
		}
		
		for(int i = 0 ; i < sim ; i++){
			
			x = (i+1)*xres + 200;
			MoveToEx(hdc,x,10,NULL);
			LineTo(hdc,x,440);

			if(!(i%(sim/10))){
				if(i==0){
					_stprintf(txt, "%d", i);
				}else{
					_stprintf(txt, "%dt", i);
				}
				TextOut(hdc,x-5, zero - 8,txt, GetTextSize(txt));
			}	
		}
		
		SelectObject(hdc, hPen1);
		if(max!=0){
			MoveToEx(hdc,200,20,NULL);
			LineTo(hdc,1000,20);
		}
		
		if(min!=0){
			MoveToEx(hdc,200,440,NULL);
			LineTo(hdc,1000,440);
		}
		
		if(fin!=0){
			MoveToEx(hdc,200,zero-fin/res,NULL);
			LineTo(hdc,1000,zero-fin/res);
		}	

		SelectObject(hdc, hPenA);
		y = zero - Va[0]/res;
		x = 200;	
		for(int i = 0 ; i < sim ; i++){
			
			MoveToEx(hdc,x,y,NULL);
			y = zero - Va[i]/res;
			LineTo(hdc,x,y);	
			MoveToEx(hdc,x,y,NULL);
			x = (i+1)*xres + 200;
			LineTo(hdc,x,y);
			
		}
		
		SelectObject(hdc, hPenB);
		y = zero - Vb[0]/res;
		x = 200;	
		for(int i = 0 ; i < sim ; i++){
			
			MoveToEx(hdc,x,y,NULL);
			y = zero - Vb[i]/res;
			LineTo(hdc,x,y);	
			MoveToEx(hdc,x,y,NULL);
			x = (i+1)*xres + 200;
			LineTo(hdc,x,y);
					
		}
	
		if(max!=0){
			_stprintf(txt, _T("%3.1f V"), max);
			TextOut(hdc,950, 12,txt, GetTextSize(txt));
		}
		if(min!=0){
			_stprintf(txt, _T("%3.1f V"), min);
			TextOut(hdc,950, 432,txt, GetTextSize(txt));
		}
		if(fin!=0){
			_stprintf(txt, _T("%3.1f V"), fin);
			TextOut(hdc,950, (zero-fin/res)-8,txt, GetTextSize(txt));
		}
		SetTextColor(hdc,RGB(0, 0, 250));
		TextOut(hdc,140, 0 ,"Va", 3);
		SetTextColor(hdc,RGB(250, 0, 0));
		TextOut(hdc,160, 0 ,"Vb", 3);
		SetTextColor(hdc,RGB(0, 0, 0));
		
	}
	DeleteObject(hPen1);
	DeleteObject(hPent);
	DeleteObject(hPenA);
	DeleteObject(hPenB);
	DeleteObject(hPenDefault);
	
}

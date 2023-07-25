#include <windows.h>

// int unsigned = UINT - (windows)

LRESULT CALLBACK MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
	LRESULT Result = 0;

	switch (Message) {
	case WM_SIZE: {
		OutputDebugStringA("WM_SIZE\n");
	} break;


	case WM_DESTROY: {
		OutputDebugStringA("WM_DESTROY\n");

	} break;


	case WM_CLOSE: {
		OutputDebugStringA("WM_CLOSE\n");

	} break;

	case WM_ACTIVATEAPP: {
		OutputDebugStringA("WM_ACTIVATEAPP\n");

	}break;


	case WM_PAINT: {
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(Window, &Paint);

		int X = Paint.rcPaint.left;
		int Y = Paint.rcPaint.top;
		int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
		int Width = Paint.rcPaint.right - Paint.rcPaint.left;
		static DWORD Operation = WHITENESS;
		PatBlt(DeviceContext, X, Y, Width, Height, Operation);

		if (Operation == WHITENESS) Operation = BLACKNESS;
		else if (Operation == BLACKNESS) Operation = WHITENESS;


		EndPaint(Window, &Paint);
	} break;

	default: {
		//		OutputDebugStringA("DEFAULT\n");
		Result = DefWindowProc(Window, Message, WParam, LParam);
	} break;

	}

	return Result;
};

int CALLBACK WinMain(
	HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR     CommandLine,
	int       ShowCode
) {
	// TODO(casey): Check if HREDRAW|VREDRAW|OWNDC still matter
	WNDCLASS WindowClass = {};
	WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // bit field, properties that we want our window to have
	WindowClass.lpfnWndProc = MainWindowCallback; // pointer to a function that we will define
	WindowClass.hInstance = Instance; // A handle to the instance that contains the window procedure for the class
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";
	//WindowClass.hIcon = hInstance;

	if (RegisterClass(&WindowClass)) {
		HWND WindowHandle = CreateWindowEx(
			0,
			WindowClass.lpszClassName,
			"Handmade Hero",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, // x
			CW_USEDEFAULT, // y
			CW_USEDEFAULT, // nWidth
			CW_USEDEFAULT, // nHeight
			0, // If we want windows inside windows
			0, // If we have a menu
			Instance,
			0 // Pass parameters to the window and get with the WM_CREATE
		);

		if (WindowHandle != NULL) {
			MSG Message;
			for (;;) {
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if (MessageResult > 0) {
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				else break;
			}
		}
		else {
			//TODO(casey): Logging
		};

	}
	else {
		//TODO(casey): Logging
	};


	return 0;
}
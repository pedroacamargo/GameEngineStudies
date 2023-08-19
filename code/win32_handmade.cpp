#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void* BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void RenderWeirdGradient(int XOffset, int YOffset){
	int Width = BitmapWidth;
	int Height = BitmapHeight;

	// As VirtualAlloc return *void, we need to get the Bitmemory pointer
	int Pitch = Width * BytesPerPixel;
	uint8* Row = (uint8*)BitmapMemory;

	for (int Y = 0; Y < BitmapHeight; ++Y) {
		uint32* Pixel = (uint32 *)Row;
		for (int X = 0; X < BitmapWidth; ++X) {
			/*
				Pixel in memory: BB GG RR xx -> Little endian
			*/
			uint8 Blue = (X + XOffset);
			uint8 Green = (Y + YOffset);

			/*
				Memory: BB GG RR xx
				Register: xx RR GG BB

				Pixel (32-bits)
			*/

			*Pixel++ = ((Green << 8) | Blue);
		}

		Row += Pitch;
	}
}

// DIB = Device Independent Bitmap -> It contains a color table.
// This function is creating the memory amount for the Bitmap Frame in our window
internal void Win32ResizeDIBSection(int Width, int Height) {
	// TODO: Bulletproof this.
	// Maybe don't free first, free after, then free if that fails.
	if (BitmapMemory) {
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}
	// It could be MEM_DECOMMIT instead of MEM_RELEASE
	// MEM_RELEASE -> Will decommit the memory allocated, but free aswell
	// MEM_DECOMMIT -> If for some reason we said "look, we don't have to pay attention to this anymore, but we're gonna actually probably want it back later"
	// so don't bother about tracking this memory but don't get rid of it too.
	// Also, we could change the memory protection of BitmapMemory to don't let the program write or read in the memory itself

	BitmapWidth = Width;
	BitmapHeight = Height;

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (BitmapWidth * BitmapHeight) * BytesPerPixel;
	BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void Win32UpdateWindow(HDC DeviceContext, RECT* ClientRect, int X, int Y, int Width, int Height) {

	int WindowWidth = ClientRect->right - ClientRect->left;
	int WindowHeight = ClientRect->bottom - ClientRect->top;

	StretchDIBits(DeviceContext,
		/*
		X, Y, Width, Height,
		X, Y, Width, Height,
		*/
		0, 0, BitmapWidth, BitmapHeight,
		0, 0, WindowWidth, WindowHeight,
		BitmapMemory,
		&BitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);
}


LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
	LRESULT Result = 0;

	switch (Message) {
	case WM_SIZE: {
		RECT ClientRect;
		GetClientRect(Window, &ClientRect);
		int Height = ClientRect.bottom - ClientRect.top;
		int Width = ClientRect.right - ClientRect.left;
		Win32ResizeDIBSection(Width, Height);
	} break;


	case WM_DESTROY: {
		// TODO: Handle this with a message to the user?
		Running = false;
	} break;


	case WM_CLOSE: {
		// TODO: Handle this as an error - recreate window?
		Running = false;
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

		RECT ClientRect;
		GetClientRect(Window, &ClientRect);

		Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
		//local_persist DWORD Operation = BLACKNESS;
		//PatBlt(DeviceContext, X, Y, Width, Height, Operation);

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
	WindowClass.lpfnWndProc = Win32MainWindowCallback; // pointer to a function that we will define
	WindowClass.hInstance = Instance; // A handle to the instance that contains the window procedure for the class
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";
	//WindowClass.hIcon = hInstance;

	if (RegisterClassA(&WindowClass)) {
		HWND Window = CreateWindowExA(
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
		Running = true;
		if (Window != NULL) {
			Running = true;
			MSG Message;
			int XOffset = 0;
			int YOffset = 0;
			while (Running) {
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
					if (Message.message == WM_QUIT) {
						Running = false;
					}

					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}
				RenderWeirdGradient(XOffset, YOffset);

				HDC DeviceContext = GetDC(Window);
				RECT ClientRect;
				GetClientRect(Window, &ClientRect);
				int WindowWidth = ClientRect.right - ClientRect.left;
				int WindowHeight = ClientRect.bottom - ClientRect.top;
				Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
				ReleaseDC(Window, DeviceContext);
				++XOffset;

				//BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
				//if (MessageResult > 0) {
					
				//}
				//else break;
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
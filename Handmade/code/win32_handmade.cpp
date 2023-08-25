#include <windows.h>
#include <stdint.h>
#include <Xinput.h>
//#pragma comment(lib, "Xinput.lib")
//#pragma comment(lib, "Xinput9_1_0.lib")


/*
	Pointer aliasing is when two pointers COULD point to the same memory
	and the compuler doesn't know if a write to one of those pointers might effect
	a read from the other pointer.
*/
/*
	Techs used:
	XInput -> Get input from a gamepad
*/


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


struct win32_offscreen_buffer {
	// NOTE: Pixels are always 32-bits wide, Memory Order BB GG RR XX
	BITMAPINFO Info;
	void* Memory;
	int Width;
	int Height;
	int Pitch;
};

struct win32_window_dimension {
	int Width;
	int Height;
};

global_variable bool Running;
global_variable win32_offscreen_buffer GlobalBackBuffer;

// NOTE: XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);  // typedef DWORD WINAPI x_input_get_state(DWORD dwUserIndex, XINPUT_STATE* pState);
X_INPUT_GET_STATE(XInputGetStateStub) {
	return 0;
}
global_variable x_input_get_state* XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

// NOTE: XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);  
X_INPUT_SET_STATE(XInputSetStateStub) {
	return 0;
}
global_variable x_input_set_state* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void Win32LoadXInput() {
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if (XInputLibrary) {
		XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
	}
}

internal win32_window_dimension Win32GetWindowDimension(HWND Window) {
	win32_window_dimension Result;

	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;

	return Result;
}

internal void RenderWeirdGradient(win32_offscreen_buffer *Buffer, int XOffset, int YOffset){
	// As VirtualAlloc return *void, we need to get the Bitmemory pointer
	uint8* Row = (uint8*)Buffer->Memory;

	for (int Y = 0; Y < Buffer->Height; ++Y) {
		uint32* Pixel = (uint32 *)Row;
		for (int X = 0; X < Buffer->Width; ++X) {
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

		Row += Buffer->Pitch;
	}
}

// DIB = Device Independent Bitmap -> It contains a color table.
// This function is creating the memory amount for the Bitmap Frame in our window
internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height) {
	// TODO: Bulletproof this.
	// Maybe don't free first, free after, then free if that fails.
	if (Buffer->Memory) {
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}
	// It could be MEM_DECOMMIT instead of MEM_RELEASE
	// MEM_RELEASE -> Will decommit the memory allocated, but free aswell
	// MEM_DECOMMIT -> If for some reason we said "look, we don't have to pay attention to this anymore, but we're gonna actually probably want it back later"
	// so don't bother about tracking this memory but don't get rid of it too.
	// Also, we could change the memory protection of BitmapMemory to don't let the program write or read in the memory itself

	Buffer->Width = Width;
	Buffer->Height = Height;
	int BytesPerPixel = 4;

	// NOTE: When the biHeight field is negative, this is the clue to Windows
	// to treat this bitmap as top-down, not bottom-up, meaning that the first
	// three bytes of the image are the color for the top left pixel in the
	// bitmap, not the bottom left!
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (Buffer->Width * Buffer->Height) * BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	Buffer->Pitch = Width * BytesPerPixel;
}

internal void Win32DisplayBufferInWindow(win32_offscreen_buffer* Buffer, HDC DeviceContext, int WindowWidth, int WindowHeight) {
	// TODO: Aspect Ratio Correction
	StretchDIBits(DeviceContext,
		/*
		X, Y, Width, Height,
		X, Y, Width, Height,
		*/
		0, 0, WindowWidth, WindowHeight,
		0, 0, Buffer->Width, Buffer->Height,
		Buffer->Memory,
		&Buffer->Info,
		DIB_RGB_COLORS, SRCCOPY);
}


LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
	LRESULT Result = 0;

	switch (Message) {
	case WM_SIZE: {
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

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP: {
		uint32 VKCode = WParam;
		bool WasDown = ((LParam & (1 << 30)) != 0);
		bool IsDown = ((LParam & (1 << 31)) == 0);
		if (WasDown != IsDown) {
			if (VKCode == 'W') {

			}
			else if (VKCode == 'S') {

			}
			else if (VKCode == 'D') {

			}
			else if (VKCode == 'A') {

			}
			else if (VKCode == 'E') {

			}
			else if (VKCode == VK_UP) {

			}
			else if (VKCode == VK_DOWN) {

			}
			else if (VKCode == VK_LEFT) {

			}
			else if (VKCode == VK_RIGHT) {

			}
			else if (VKCode == VK_ESCAPE) {

			}
			else if (VKCode == VK_SPACE) {

			}
		}
	} break;

	case WM_PAINT: {
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(Window, &Paint);
		int X = Paint.rcPaint.left;
		int Y = Paint.rcPaint.top;
		int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
		int Width = Paint.rcPaint.right - Paint.rcPaint.left;

		win32_window_dimension Dimension = Win32GetWindowDimension(Window);
		Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);
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
	Win32LoadXInput();
	WNDCLASSA WindowClass = {};

	Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // bit field, properties that we want our window to have
	WindowClass.lpfnWndProc = Win32MainWindowCallback; // pointer to a function that we will define
	WindowClass.hInstance = Instance; // A handle to the instance that contains the window procedure for the class
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";
	//WindowClass.hIcon = hInstance;

	// To use the CreateWindowExA function, we have to register a window class for subsequent use.
	if (RegisterClassA(&WindowClass)) {

		// Creates an overlapped, popUp, or normal window.
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
			// NOTE: Since we specified CS_OWNDC, we can just
			// get one device context and use it forever because we
			// are not sharing it with anyone.
			HDC DeviceContext = GetDC(Window);

			Running = true;
			int XOffset = 0;
			int YOffset = 0;
			while (Running) {
				MSG Message;
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
					if (Message.message == WM_QUIT) {
						Running = false;
					}

					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}

				// TODO: Should we poll this more frequently
				for (DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ++ControllerIndex) {
					XINPUT_STATE ControllerState;
					ZeroMemory(&ControllerState, sizeof(XINPUT_STATE));

					// Simply get the state of the controller from XInput.

					DWORD dwResult = XInputGetState(ControllerIndex, &ControllerState);
					if (dwResult == ERROR_SUCCESS) {
						// NOTE: This controller is plugged in
						// TODO: See if ControllerState.dwPacketNumber increments too rapidly
						XINPUT_GAMEPAD* Pad = &ControllerState.Gamepad;

						bool Up = Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP;
						bool Down = Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
						bool Left = Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
						bool Right = Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
						bool Start = Pad->wButtons & XINPUT_GAMEPAD_START;
						bool Back = Pad->wButtons & XINPUT_GAMEPAD_BACK;
						bool LeftShoulder = Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
						bool RightShoulder = Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
						bool AButton = Pad->wButtons & XINPUT_GAMEPAD_A;
						bool BButton = Pad->wButtons & XINPUT_GAMEPAD_B;
						bool XButton = Pad->wButtons & XINPUT_GAMEPAD_X;
						bool YButton = Pad->wButtons & XINPUT_GAMEPAD_Y;

						int16 StickX = Pad->sThumbLX;
						int16 StickY = Pad->sThumbLY;

						if (AButton) {
							++YOffset;
						}
					}
					else {
						// NOTE: The controller is not available
					}
				}

				RenderWeirdGradient(&GlobalBackBuffer ,XOffset, YOffset);

				win32_window_dimension Dimension = Win32GetWindowDimension(Window);
				
				Win32DisplayBufferInWindow(&GlobalBackBuffer ,DeviceContext, Dimension.Width, Dimension.Height);
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
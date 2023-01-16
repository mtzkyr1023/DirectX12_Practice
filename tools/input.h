#ifndef _INPUT_H_
#define _INPUT_H_

#define INITGUID
#define DIRECTINPUT_VERSION     0x0800

#pragma comment(lib, "dinput8.lib")

#include <dinput.h>
#include <wrl/client.h>

class Input {
private:
	enum { INPUT_BUFFER_SIZE = 256 };

	Microsoft::WRL::ComPtr<IDirectInput8A> m_dinput;
	Microsoft::WRL::ComPtr<IDirectInputDevice8A> m_keyDevice;
	Microsoft::WRL::ComPtr<IDirectInputDevice8A> m_mouseDevice;

	BYTE m_keyInput[INPUT_BUFFER_SIZE];
	BYTE m_keyInputPreview[INPUT_BUFFER_SIZE];
	DIMOUSESTATE2 m_mouseState;
	DIMOUSESTATE2 m_mouseStatePreview;

	int m_movLeftX;
	int m_movLeftY;
	int m_movRightX;
	int m_movRightY;

public:
	Input();
	~Input();

	bool Initialize(HWND hwnd);


	float GetMoveX();
	float GetMoveY();

	float GetMoveXLeftPushed();
	float GetMoveYLeftPushed();
	float GetMoveXRightPushed();
	float GetMoveYRightPushed();

	void Updata();

	bool Push(int code);
	bool Trigger(int code);
	bool Release(int code);

	bool LeftPush();
	bool RightPush();

	static Input& Instance() {
		static Input input;
		return input;
	}
};

#endif
#include "input.h"

Input::Input() {
}

Input::~Input() {
	m_keyDevice->Unacquire();
	m_mouseDevice->Unacquire();
}

bool Input::Initialize(HWND hwnd) {
	HRESULT result;
	DIPROPDWORD diprop;

	result = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)m_dinput.ReleaseAndGetAddressOf(), NULL);
	if (FAILED(result)) {
		MessageBox(NULL, "failed creating direct input.", "Input.cpp", MB_OK);
		return false;
	}

	result = m_dinput->CreateDevice(GUID_SysKeyboard, m_keyDevice.ReleaseAndGetAddressOf(), NULL);
	if (FAILED(result)) {
		MessageBox(NULL, "failed creating key device.", "Input.cpp", MB_OK);
		return false;
	}

	result = m_keyDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		MessageBox(NULL, "failed setting data format.", "Input.cpp", MB_OK);
		return false;
	}

	result = m_keyDevice->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	if (FAILED(result)) {
		MessageBox(NULL, "failed setting cooprative level.", "Input.cpp", MB_OK);
		return false;
	}

	result = m_dinput->CreateDevice(GUID_SysMouse, m_mouseDevice.ReleaseAndGetAddressOf(), NULL);
	if (FAILED(result)) {
		MessageBox(NULL, "failed creating mouse device.", "Input.cpp", MB_OK);
		return false;
	}

	result = m_mouseDevice->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(result)) {
		MessageBox(NULL, "failed setting data format.", "Input.cpp", MB_OK);
		return false;
	}

	result = m_mouseDevice->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(result)) {
		MessageBox(NULL, "failed setting cooperative level.", "Input.cpp", MB_OK);
		return false;
	}

	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;

	result = m_mouseDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	if (FAILED(result)) {
		MessageBox(NULL, "failed setting property.", "Input.cpp", MB_OK);
		return false;
	}

	m_keyDevice->Acquire();
	m_mouseDevice->Acquire();

	return true;
}

float Input::GetMoveX() {
	return (float)-m_mouseState.lX;
}

float Input::GetMoveY() {
	return (float)m_mouseState.lY;
}

float Input::GetMoveXLeftPushed() {
	return (float)m_movLeftX;
}

float Input::GetMoveYLeftPushed() {
	return (float)m_movLeftY;
}

float Input::GetMoveXRightPushed() {
	return (float)m_movRightX;
}

float Input::GetMoveYRightPushed() {
	return (float)m_movRightY;
}

void Input::Updata() {
	HRESULT result;

	result = m_keyDevice->Acquire();
	if ((result == DI_OK) || (result == S_FALSE)) {
		memcpy_s(m_keyInputPreview, sizeof(BYTE) * INPUT_BUFFER_SIZE, m_keyInput, sizeof(BYTE) * INPUT_BUFFER_SIZE);

		BYTE tKeyInputWork[INPUT_BUFFER_SIZE];
		m_keyDevice->GetDeviceState(sizeof(m_keyInput), &tKeyInputWork);
		for (int i = 0; i < INPUT_BUFFER_SIZE; i++) {
			if (tKeyInputWork[i]) {
				if (m_keyInput[i] < 255)
					m_keyInput[i]++;
				else
					m_keyInput[i] = 255;
			}
			else
				m_keyInput[i] = 0;
		}
	}

	result = m_mouseDevice->Acquire();
	if ((result == DI_OK) || (result == S_FALSE)) {
		memcpy_s(&m_mouseStatePreview, sizeof(DIMOUSESTATE2), &m_mouseState, sizeof(DIMOUSESTATE2));

		m_mouseDevice->GetDeviceState(sizeof(m_mouseState), &m_mouseState);
	}

	if (m_mouseState.rgbButtons[0] & 0x80) {
		m_movLeftX = m_mouseState.lX;
		m_movLeftY = m_mouseState.lY;
	}
	else {
		m_movLeftX = 0;
		m_movLeftY = 0;
	}

	if (m_mouseState.rgbButtons[1] & 0x80) {
		m_movRightX = -m_mouseState.lX;
		m_movRightY = m_mouseState.lY;
	}
	else {
		m_movRightX = 0;
		m_movRightY = 0;
	}
}

bool Input::Push(int code) {
	if (m_keyInput[code])
		return true;
	return false;
}

bool Input::Trigger(int code) {
	if (!m_keyInputPreview[code] && m_keyInput[code])
		return true;
	return false;
}

bool Input::Release(int code) {
	if (m_keyInputPreview[code] && !m_keyInput[code])
		return true;
	return false;
}

bool Input::LeftPush() {
	if (m_mouseState.rgbButtons[0] & 0x80)
		return true;

	return false;
}

bool Input::RightPush() {
	if (m_mouseState.rgbButtons[1] & 0x80)
		return true;

	return false;
}
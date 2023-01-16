#define     NAME    "WINDOW"  //タイトルバーに表示するテキスト

#pragma comment(lib, "d3dcompiler.lib")
#define NOMINMAX
#include <windows.h>
#include <crtdbg.h>
#include <iostream>
#include <thread>

#include "tools/input.h"

#include "imgui_dx12/imgui.h"
#include "imgui_dx12/imgui_impl_win32.h"
#include "imgui_dx12/imgui_impl_dx12.h"

#include "App.hpp"


#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

using namespace std;


//プロトタイプ宣言
LRESULT  CALLBACK   WndProc(HWND, UINT, WPARAM, LPARAM);
int      WINAPI     WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Windws イベント用関数
LRESULT  CALLBACK  WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	//渡された message から、イベントの種類を解析する
	switch (msg) {
		//----終了処理----
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		//----デフォルトの処理----
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0L;
}

//Windows Main 関数
int  WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {

	HWND        hWnd;
	MSG         msg;

	// Set up and register window class

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, NAME, NULL };
	if (RegisterClassEx(&wc) == 0) return FALSE;

	//ウインドウ生成
	hWnd = CreateWindow(NAME,                  //タイトルバーテキスト
		NAME,                  //クラス名
		WS_OVERLAPPEDWINDOW,   //ウインドウスタイル
		(GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2,         //ウインドウ左上x座標
		(GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2,         //ウインドウ左上y座標
		WINDOW_WIDTH,         //ウインドウ幅
		WINDOW_HEIGHT,         //ウインドウ高さ
		NULL,                  //親ウインドウのハンドル
		NULL,
		hInstance,
		NULL);
	if (!hWnd) return FALSE;

	ShowWindow(hWnd, nCmdShow);     //Window を表示
	UpdateWindow(hWnd);             //表示を初期化
	SetFocus(hWnd);                 //フォーカスを設定

	App app;
	if (!app.initialize(hWnd))
		return false;

	if (!Input::Instance().Initialize(hWnd))
		return false;

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) {
			break;
		}
		else {
			if (GetAsyncKeyState(VK_ESCAPE))
				break;

			app.render();

			Input::Instance().Updata();
		}
	}

	app.shutdown();

	return (int)msg.wParam;
}
#include "RadioactiveFish.h"
#include "stdafx.h"
#include <atlbase.h>
#include <atlwin.h>
#include <wrl.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")
using namespace D2D1;
using namespace Microsoft::WRL;

struct SampleWindow : CWindowImpl<SampleWindow, CWindow, CWinTraits<WS_OVERLAPPEDWINDOW | WS_VISIBLE>>
{
	ComPtr<ID2D1Factory> m_factory;
	ComPtr<ID2D1HwndRenderTarget> m_target;
	ComPtr<ID2D1SolidColorBrush> m_brush;
	D2D1_COLOR_F const COLOR_BLUE = ColorF( 0.26f, 0.56f, 0.87f, 1.0f );
	D2D1_COLOR_F const COLOR_YELLOW = ColorF( 0.99f, 0.85f, 0.0f, 1.0f);
	D2D1_COLOR_F const COLOR_BLACK = ColorF( 0.0f, 0.0f, 0.0f, 1.0f );
	D2D1_COLOR_F const COLOR_WHITE = ColorF( 1.0f, 1.0f, 1.0f, 1.0f );

	DECLARE_WND_CLASS_EX(L"Window", CS_HREDRAW | CS_VREDRAW, -1);

	BEGIN_MSG_MAP(SampleWindow)
		MESSAGE_HANDLER(WM_PAINT, PaintHandler)
		MESSAGE_HANDLER(WM_DESTROY, DestroyHandler)
		MESSAGE_HANDLER(WM_SIZE, SizeHandler)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, DisplayChangeHandler)
	END_MSG_MAP()

	LRESULT DisplayChangeHandler(UINT, WPARAM, LPARAM, BOOL &)
	{
		Invalidate();
		return 0;
	}

	LRESULT PaintHandler(UINT, WPARAM, LPARAM, BOOL &)
	{
		PAINTSTRUCT ps;
		BeginPaint(&ps);
		Render();
		EndPaint(&ps);
		return 0;
	}
	LRESULT DestroyHandler(UINT, WPARAM, LPARAM, BOOL &)
	{
		PostQuitMessage(0);
		return 0;

	}
	LRESULT SizeHandler(UINT, WPARAM, LPARAM lparam, BOOL &)
	{
		if (m_target)
		{
			if (S_OK != m_target->Resize(SizeU(LOWORD(lparam), HIWORD(lparam))))
			{
				m_target.Reset();
			}
		}
		return 0;
	}

	void Invalidate()
	{
		InvalidateRect(nullptr, false);
	}
	void Create()
	{
		D2D1_FACTORY_OPTIONS fo = {};
#ifdef DEBUG
		fo.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
			fo,
			m_factory.GetAddressOf());
		CreateDeviceIndependantResources();

			__super::Create(nullptr, 0, L"title");
	}

	void CreateDeviceIndependantResources()
	{

	}
	void CreateDeviceResources()
	{
		m_target->CreateSolidColorBrush(COLOR_BLUE, m_brush.ReleaseAndGetAddressOf());
	}
	void Render()
	{
		if (!m_target)
		{
			RECT rect;
			GetClientRect(&rect);
			auto size = SizeU(rect.right, rect.bottom);
			

			m_factory->CreateHwndRenderTarget(RenderTargetProperties(), HwndRenderTargetProperties(m_hWnd, size), m_target.GetAddressOf());
		
			CreateDeviceResources();


		}

		if (!(D2D1_WINDOW_STATE_OCCLUDED & m_target->CheckWindowState()))
		{
			m_target->BeginDraw();
			
			Draw();

			if (D2DERR_RECREATE_TARGET == m_target->EndDraw())
			{
				m_target.Reset();
			}
		}
	}



	void Draw()
	{
		m_target->Clear(COLOR_BLUE);

		auto size = m_target->GetSize();

		m_brush->SetColor(COLOR_BLACK);
		m_target->FillRectangle(RectF(100.0f, 100.0f, size.width - 100.0f, 200.0f), m_brush.Get());

		m_brush->SetColor(COLOR_WHITE);
		auto bw = RectF(100.0f, 300.0f, size.width - 100.0f, 400.0f);
		m_target->FillRectangle(bw, m_brush.Get());
	}

};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	SampleWindow window;
	window.Create();


	MSG message;
	BOOL result;

	while (result = GetMessage(&message, 0, 0, 0))
	{
		if (-1 != result)
		{
			DispatchMessage(&message);
		}
	}
}

/*
typedef LRESULT(*message_callback)(HWND, WPARAM, LPARAM);

struct message_handler
{
	UINT message;
	message_callback handler;
};

static message_handler s_handlers[] =
{
	{
		WM_PAINT, [](HWND window, WPARAM, LPARAM) -> LRESULT
		{
			PAINTSTRUCT ps;
			BeginPaint(window, &ps);
			EndPaint(window, &ps);
			return 0;
		}
	},
	{
		WM_DESTROY, [](HWND window, WPARAM, LPARAM)->LRESULT
		{
			PostQuitMessage(0);
			return 0;
		}
	}
};

int _stdcall wWinMain(HINSTANCE module, HINSTANCE, PWSTR, int)
{
	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hInstance = module;
	wc.lpszClassName = L"window";

	wc.lpfnWndProc = [](HWND window, UINT message, WPARAM wparam, LPARAM lparam) -> LRESULT
	{
		for (auto h = s_handlers; h != s_handlers + _countof(s_handlers); ++h)
		{
			if (message == h->message)
			{
				return h->handler(window, wparam, lparam);
			}
		}
		return DefWindowProc(window, message, wparam, lparam);
	};

	RegisterClass(&wc);

	auto hwnd = CreateWindow(wc.lpszClassName, L"Title", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, module, nullptr);

	MSG message;
	BOOL result;

	while (result = GetMessage(&message, 0, 0, 0))
	{
		if (result != -1)
		{
			DispatchMessage(&message);
		}
	}
}
*/
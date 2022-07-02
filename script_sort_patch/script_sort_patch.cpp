#include "pch.h"

// デバッグ用コールバック関数。デバッグメッセージを出力する。
void ___outputLog(LPCTSTR text, LPCTSTR output)
{
	::OutputDebugString(output);
}

AviUtlInternal g_auin;

DECLARE_HOOK_PROC(LRESULT, WINAPI, SettingDialogProc, (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam));
DECLARE_HOOK_PROC(LONG, WINAPI, SetWindowLongA, (HWND hwnd, int index, LONG newLong));
DECLARE_HOOK_PROC(INT_PTR, WINAPI, DialogBoxParamA, (HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dlgProc, LPARAM lParam));

HWND getComboBox(HWND settingDialog)
{
	int i = 0; // デバッグ用。
	HWND child = ::GetWindow(settingDialog, GW_CHILD);
	child = ::GetWindow(child, GW_HWNDLAST);
	while (child)
	{
		UINT id = ::GetDlgCtrlID(child);

		if (id >= 8100 && id <= 8147)
		{
			MY_TRACE(_T("%d : %d\n"), i, id);

			if (::IsWindowVisible(child))
			{
				// コンボボックスかどうかクラス名で調べる。
				TCHAR className[MAX_PATH] = {};
				::GetClassName(child, className, MAX_PATH);
				MY_TRACE_TSTR(className);

				if (::lstrcmpi(className, _T("ComboBox")) == 0)
				{
					// 目的のコンボボックスかどうかウィンドウテキストで調べる。
					TCHAR windowText[MAX_PATH] = {};
					::GetWindowText(child, windowText, MAX_PATH);
					MY_TRACE_TSTR(windowText);

					if (::lstrcmp(windowText, _T("震える")) == 0)
					{
						return child;
					}
				}
			}
		}

		i++;
		child = ::GetWindow(child, GW_HWNDPREV);
	}

	return 0;
}

IMPLEMENT_HOOK_PROC_NULL(LRESULT, WINAPI, SettingDialogProc, (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam))
{
	switch (message)
	{
	case WM_CREATE:
		{
			MY_TRACE(_T("SettingDialogProc(WM_CREATE)\n"));

			{
				// script_sort.auf 内の ::SetWindowLongA() をフックする。

				HMODULE module = ::GetModuleHandle(_T("script_sort.auf"));

				if (module)
				{
					true_SetWindowLongA = hookImportFunc(module, "SetWindowLongA", hook_SetWindowLongA);
				}
			}

			{
				// rikky_module.dll 内の ::DialogBoxParamA() をフックする。

				HMODULE module = ::LoadLibrary(_T("rikky_module.dll"));

				if (module)
				{
					true_DialogBoxParamA = hookImportFunc(module, "DialogBoxParamA", hook_DialogBoxParamA);
				}
			}

			break;
		}
	case WM_COMMAND:
		{
			UINT code = HIWORD(wParam);
			UINT id = LOWORD(wParam);
			HWND sender = (HWND)lParam;

//			MY_TRACE(_T("WM_COMMAND, 0x%04X, 0x%04X, 0x%08X)\n"), code, id, sender);

			if (id == 2079)
			{
				MY_TRACE(_T("アニメーション効果が追加されました\n"));

				// 手動でコンボボックスを探し出してから WM_CTLCOLOREDIT を送信する。
				// ::UpdateWindow() を使う方法はコンボボックスがウィンドウ外に隠れているような状況だと効果がない。

				LRESULT result = true_SettingDialogProc(hwnd, message, wParam, lParam);
				HWND combobox = getComboBox(hwnd);
				MY_TRACE_HEX(combobox);
				::SendMessage(hwnd, WM_CTLCOLOREDIT, 0, (LPARAM)combobox);
				return result;
			}

			break;
		}
	}

	return true_SettingDialogProc(hwnd, message, wParam, lParam);
}

IMPLEMENT_HOOK_PROC_NULL(LONG, WINAPI, SetWindowLongA, (HWND hwnd, int index, LONG newLong))
{
	MY_TRACE(_T("SetWindowLongA(0x%08X, %d, 0x%08X)\n"), hwnd, index, newLong);

	// フック前の関数を保存してフック後に復元する。
	// script_sort.auf が他のプラグインのフックも外してしまうのを防ぐため。

	if (index == GWL_WNDPROC)
	{
		const LPCTSTR propName = _T("script_sort_patch");

		if (newLong)
		{
			MY_TRACE(_T("元の関数を保存します\n"));
			LONG retValue = true_SetWindowLongA(hwnd, index, newLong);
			::SetProp(hwnd, propName, (HANDLE)retValue);
			return retValue;
		}
		else
		{
			MY_TRACE(_T("元の関数を復元します\n"));
			newLong = (LONG)::GetProp(hwnd, propName);
			::RemoveProp(hwnd, propName);
		}
	}

	return true_SetWindowLongA(hwnd, index, newLong);
}

DLGPROC true_dlgProc = 0;
INT_PTR CALLBACK hook_dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NOTIFY:
		{
			MY_TRACE(_T("WM_NOTIFY\n"));

			NMHDR* nm = (NMHDR*)lParam;

			MY_TRACE(_T("%d, %d, 0x%08X\n"), nm->code, nm->idFrom, nm->hwndFrom);
			MY_TRACE_HWND(nm->hwndFrom);

			return 0;
		}
	case WM_PRINTCLIENT:
		{
			MY_TRACE(_T("WM_PRINTCLIENT\n"));

			break;
		}
	}

	return true_dlgProc(hwnd, message, wParam, lParam);
}

IMPLEMENT_HOOK_PROC_NULL(INT_PTR, WINAPI, DialogBoxParamA, (HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dlgProc, LPARAM lParam))
{
	MY_TRACE(_T("DialogBoxParamA(0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X)\n"), instance, templateName, parent, dlgProc, lParam);

	true_dlgProc = dlgProc;
	return ::DialogBoxParamW(instance, (LPCWSTR)templateName, parent, hook_dlgProc, lParam);
}

//---------------------------------------------------------------------
//		フィルタ構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) FILTER_DLL* CALLBACK GetFilterTable()
{
	static TCHAR g_filterName[] = TEXT("スクリプト並び替えパッチ");
	static TCHAR g_filterInformation[] = TEXT("スクリプト並び替えパッチ 2.0.0 by 蛇色");

	static FILTER_DLL g_filter =
	{
		FILTER_FLAG_NO_CONFIG |
		FILTER_FLAG_ALWAYS_ACTIVE |
		FILTER_FLAG_DISP_FILTER |
		FILTER_FLAG_EX_INFORMATION,
		0, 0,
		g_filterName,
		NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL,
		NULL,
		func_init,
		func_exit,
		NULL,
		NULL,
		NULL, NULL,
		NULL,
		NULL,
		g_filterInformation,
		NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL,
	};

	return &g_filter;
}

//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
BOOL func_init(FILTER *fp)
{
	if (!g_auin.initExEditAddress())
		return FALSE;

	true_SettingDialogProc = g_auin.HookSettingDialogProc(hook_SettingDialogProc);

	return TRUE;
}

//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
BOOL func_exit(FILTER *fp)
{
	return FALSE;
}

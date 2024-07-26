#include <windows.h>
#include <iostream>
#include <string>

// 钩子句柄
HHOOK hMouseHook;
HWINEVENTHOOK hEventHook;

int value1 = 0;

int upCount = 0;
int downCount = 0;

std::string readIniFile(const std::string &section, const std::string &key, const std::string &defaultValue,
                        const std::string &filePath) {
    char value[255];
    GetPrivateProfileString(section.c_str(), key.c_str(), defaultValue.c_str(), value, sizeof(value), filePath.c_str());
    return std::string(value);
}

// 鼠标钩子函数
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        switch (wParam) {
            case WM_MOUSEWHEEL: {
                MSLLHOOKSTRUCT *pMouseStruct = (MSLLHOOKSTRUCT *) lParam;
                if (pMouseStruct != NULL) {
                    int zDelta = GET_WHEEL_DELTA_WPARAM(pMouseStruct->mouseData);
                    if (zDelta > 0) {
                        upCount++;
                        //std::cout << "up" << std::endl;
                        if (upCount >= value1){
                            upCount = 0;
                            return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
                        }else{
                            return 1;
                        }
                    } else {
                        downCount++;
                        //std::cout << "down" << std::endl;
                        if (downCount >= value1){
                            downCount = 0;
                            return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
                        }else{
                            return 1;
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 获取当前程序运行目录
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    std::string::size_type pos = std::string(path).find_last_of("\\/");
    std::string directory = std::string(path).substr(0, pos);

    // INI 文件路径
    std::string iniFilePath = directory + "\\config.ini";

    // 读取 INI 文件中的键值
    std::string section1 = "Settings";
    std::string key1 = "MouseWheel";
    std::string defaultValue1 = "3";
    value1 = std::stoi(readIniFile(section1, key1, defaultValue1, iniFilePath));

    // 安装鼠标钩子
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    if (hMouseHook == NULL) {
        std::cerr << "Failed to install mouse hook!" << std::endl;
        return 1;
    }

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 卸载钩子
    UnhookWindowsHookEx(hMouseHook);
    UnhookWinEvent(hEventHook);
    return 0;
}
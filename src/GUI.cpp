#include "GUI.h"

#include <windows.h>
#include <tchar.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dwmapi.h>
#include <iostream>
#include <thread>

#include "audio.h"
#include "globals.h"
#include "imgui.h"
#include "sound.h"
#include "utils.h"
#include "validateFiles.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dwmapi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
        HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static ID3D11Device*           g_pd3dDevice = nullptr;
static ID3D11DeviceContext*    g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*         g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

static void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

static void CleanupRenderTarget()
{
    if (g_mainRenderTargetView)
    {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

static bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_0
    };

    HRESULT res = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            featureLevelArray,
            2,
            D3D11_SDK_VERSION,
            &sd,
            &g_pSwapChain,
            &g_pd3dDevice,
            &featureLevel,
            &g_pd3dDeviceContext
    );

    if (FAILED(res))
        return false;

    CreateRenderTarget();
    return true;
}

static void CleanupDeviceD3D()
{
    CleanupRenderTarget();

    if (g_pSwapChain)         { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext)  { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice)         { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
        case WM_SIZE:
            if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
            {
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(
                        0,
                        (UINT)LOWORD(lParam),
                        (UINT)HIWORD(lParam),
                        DXGI_FORMAT_UNKNOWN,
                        0
                );
                CreateRenderTarget();
            }
            return 0;

        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace GUI
{
    void Run()
    {
        HINSTANCE hInstance = GetModuleHandle(nullptr);

        const TCHAR* kClassName = _T("Soundboard");
        WNDCLASSEX wc{};
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = CS_CLASSDC;
        wc.lpfnWndProc   = WndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = hInstance;
        wc.hIcon         = nullptr;
        wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszMenuName  = nullptr;
        wc.lpszClassName = kClassName;
        wc.hIconSm       = nullptr;

        RegisterClassEx(&wc);

        HWND hwnd = CreateWindow(
                kClassName,
                _T("Soundboard"),
                WS_OVERLAPPEDWINDOW,
                100, 100, 1280, 720,
                nullptr, nullptr,
                hInstance,
                nullptr
        );

        if (!CreateDeviceD3D(hwnd))
        {
            CleanupDeviceD3D();
            DestroyWindow(hwnd);
            UnregisterClass(kClassName, hInstance);
            return;
        }

        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

        bool done = false;
        while (!done)
        {
            MSG msg;
            while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    done = true;
            }
            if (done) break;

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImGuiViewport* vp = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(vp->WorkPos);
            ImGui::SetNextWindowSize(vp->WorkSize);

            ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoBringToFrontOnFocus;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

            ImGui::Begin("##Root", nullptr, flags);

            auto V2 = [](float x, float y) { return ImVec2(x, y); };
            auto Add = [](const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); };
            auto Sub = [](const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); };

            ImDrawList* dl = ImGui::GetWindowDrawList();
            const ImVec2 winPos  = ImGui::GetWindowPos();
            const ImVec2 winSize = ImGui::GetWindowSize();
            const ImVec2 winMax  = Add(winPos, winSize);

            const ImU32 cBg      = IM_COL32(12, 16, 22, 255);
            const ImU32 cCard    = IM_COL32(15, 19, 26, 245);
            const ImU32 cTop1    = IM_COL32(22, 28, 38, 255);
            const ImU32 cTop2    = IM_COL32(14, 18, 24, 255);
            const ImU32 cStroke  = IM_COL32(255, 255, 255, 28);
            const ImU32 cTextDim = IM_COL32(170, 180, 195, 255);
            const ImU32 cTextHi  = IM_COL32(230, 235, 245, 255);
            const ImU32 cAccent  = IM_COL32(110, 170, 255, 255);
            const ImU32 cWarn    = IM_COL32(255, 110, 110, 255);
            const ImU32 cOk      = IM_COL32(110, 255, 180, 255);

            dl->AddRectFilled(winPos, winMax, cBg, 0.0f);

            dl->AddRectFilledMultiColor(
                winPos, Add(winPos, V2(winSize.x, 180.0f)),
                IM_COL32(30, 50, 85, 55), IM_COL32(30, 50, 85, 55),
                IM_COL32(0, 0, 0, 0),     IM_COL32(0, 0, 0, 0)
            );

            const float outerPad = 22.0f;
            const float headerH  = 112.0f;
            const float toolbarH = 54.0f;
            const float leftW    = 300.0f;
            const float rightW   = 360.0f;
            const float rounding = 18.0f;

            const ImVec2 cardPos  = Add(winPos, V2(outerPad, outerPad));
            const ImVec2 cardSize = Sub(winSize, V2(outerPad * 2.0f, outerPad * 2.0f));
            const ImVec2 cardMax  = Add(cardPos, cardSize);

            dl->AddRectFilled(Add(cardPos, V2(2, 6)), Add(cardMax, V2(2, 6)), IM_COL32(0, 0, 0, 90), rounding);
            dl->AddRectFilled(Add(cardPos, V2(0, 3)), Add(cardMax, V2(0, 3)), IM_COL32(0, 0, 0, 70), rounding);
            dl->AddRectFilled(cardPos, cardMax, cCard, rounding);
            dl->AddRect(cardPos, cardMax, cStroke, rounding);

            const ImVec2 headerPos = cardPos;
            const ImVec2 headerMax = Add(cardPos, V2(cardSize.x, headerH));
            dl->AddRectFilledMultiColor(headerPos, headerMax, cTop1, cTop1, cTop2, cTop2);
            dl->AddRect(headerPos, headerMax, cStroke, rounding, ImDrawFlags_RoundCornersTop);

            const ImVec2 toolbarPos = headerMax;
            const ImVec2 toolbarMax = Add(toolbarPos, V2(cardSize.x, toolbarH));
            dl->AddRectFilled(toolbarPos, toolbarMax, IM_COL32(18, 22, 30, 245), 0.0f);
            dl->AddRect(toolbarPos, toolbarMax, cStroke);

            const ImVec2 contentPos = toolbarMax;
            const ImVec2 contentMax = cardMax;

            const float midX   = contentPos.x + leftW;
            const float rightX = contentMax.x - rightW;
            dl->AddLine(V2(midX, contentPos.y),   V2(midX, contentMax.y),   cStroke);
            dl->AddLine(V2(rightX, contentPos.y), V2(rightX, contentMax.y), cStroke);

            ImGui::SetCursorScreenPos(cardPos);
            ImGui::BeginChild("##AppCard", cardSize, false,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18, 16));
            ImGui::BeginChild("##Header", ImVec2(0, headerH), false,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::PushStyleColor(ImGuiCol_Text, cTextHi);
            ImGui::SetWindowFontScale(1.35f);
            ImGui::Text("Header Here");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.58f);

            ImGui::PushStyleColor(ImGuiCol_Text, cTextDim);
            size_t loadedSoundsCount = soundVector.size();
            const char* songCountString = ("Loaded Songs: " + std::to_string(loadedSoundsCount)).c_str();
            ImGui::Text(songCountString);
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, cTextHi);
            ImGui::Text(soundGlobals::hotkeysOn ? "ON" : "OFF");
            ImGui::PopStyleColor(2);

            ImGui::EndChild();
            ImGui::PopStyleVar();

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14, 10));
            ImGui::BeginChild("##Toolbar", ImVec2(0, toolbarH), false,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            static int profileIdx = 0;
            static const char* profiles[] = { "Default.ini", "Stream.ini", "Work.ini" };

            ImGui::PushStyleColor(ImGuiCol_Text, cTextDim);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Profile:");
            ImGui::PopStyleColor();
            ImGui::SameLine();

            ImGui::SetNextItemWidth(180);
            ImGui::Combo("##profile", &profileIdx, profiles, IM_ARRAYSIZE(profiles));

            ImGui::SameLine();
            if (ImGui::Button("Reload", ImVec2(86, 0))) {
                SBaudio::Audio_Shutdown();
                soundVector.clear();
                validate::createConfigIni();
                validate::getAbsoluteSounds();
                validate::getRelativeSounds();
                SBaudio::Audio_Init();
            }
            ImGui::SameLine();
            ImGui::Button("Save", ImVec2(70, 0));
            ImGui::SameLine();
            ImGui::Button("New Profile..", ImVec2(120, 0));

            ImGui::EndChild();
            ImGui::PopStyleVar();

            ImGui::BeginChild("##Content", ImGui::GetContentRegionAvail(), false,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::BeginChild("##Left", ImVec2(leftW, 0), false);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 210, 225, 255));
            ImGui::TextUnformatted("Library / Config");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Text, cTextDim);
            ImGui::TextUnformatted("INI Paths (abs)");
            ImGui::PopStyleColor();

            static int selPath = 0;
            ImGui::BeginChild("##Paths", ImVec2(0, 120), true);
            {
                const char* paths[] = { "./assets/sounds", "./steel/sounds", "D:/clips/sfx" };
                for (int i = 0; i < 3; ++i)
                {
                    ImGui::PushID(i);
                    if (ImGui::Selectable(paths[i], selPath == i))
                        selPath = i;
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();

            ImGui::Spacing();
            ImGui::Button("Add Folder", ImVec2(120, 0));
            ImGui::SameLine();
            ImGui::Button("Remove", ImVec2(90, 0));

            ImGui::Separator();
            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Text, cTextDim);
            ImGui::TextUnformatted("Output Device");
            ImGui::PopStyleColor();

            static int devIdx = 0;
            static const char* devices[] = { "Speakers", "Headset", "VB Cable" };
            ImGui::SetNextItemWidth(-1);
            ImGui::Combo("##device", &devIdx, devices, IM_ARRAYSIZE(devices));

            static bool playSelected = true;
            static bool preload = true;
            ImGui::Checkbox("Play over selected device", &playSelected);
            ImGui::Checkbox("Preload into memory", &preload);

            ImGui::Separator();
            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Text, cTextDim);
            ImGui::TextUnformatted("Hotkeys");
            ImGui::PopStyleColor();
            static bool hotkeys = true;
            ImGui::Checkbox("Enable Global Hotkeys", &hotkeys);

            ImGui::PopStyleVar();
            ImGui::EndChild();
            ImGui::SameLine();

            float centerW = ImGui::GetContentRegionAvail().x - rightW;
            if (centerW < 200.0f) centerW = 200.0f;

            ImGui::BeginChild("##Center", ImVec2(centerW, 0), false);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18, 16));

            static char searchBuf[128] = "";
            ImGui::SetNextItemWidth(-90);
            ImGui::InputTextWithHint("##search", "Search pads...", searchBuf, IM_ARRAYSIZE(searchBuf));
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, cTextDim);
            ImGui::TextUnformatted("Ctrl+F");
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::Spacing();
            ImGui::Spacing();

            float cardW = 220.0f;
            float cardH = 86.0f;
            float gap   = 14.0f;

            int cols = (int)((ImGui::GetContentRegionAvail().x + gap) / (cardW + gap));
            if (cols < 2) cols = 2;
            if (cols > 4) cols = 4;

            ImGuiTableFlags tflags =
                ImGuiTableFlags_SizingFixedFit |
                ImGuiTableFlags_NoPadInnerX |
                ImGuiTableFlags_NoBordersInBody;

            if (ImGui::BeginTable("##PadsGrid", cols, tflags))
            {
                for (size_t i = 0; i < soundVector.size(); ++i)
                {
                    ImGui::TableNextColumn();

                    ImGui::PushID((int)i);
                    if (ImGui::Button(soundVector[i]->soundName.c_str(), ImVec2(cardW, cardH)))
                    {
                        std::thread playOnThread(SBaudio::Audio_Play, soundVector[i]->filePath.c_str());
                        playOnThread.detach();
                    }
                    ImGui::PopID();

                    ImGui::Dummy(ImVec2(0, gap));
                }
                ImGui::EndTable();
            }


            ImGui::PopStyleVar();
            ImGui::EndChild();
            ImGui::SameLine();

            ImGui::BeginChild("##Right", ImVec2(rightW, 0), false);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 210, 225, 255));
            ImGui::TextUnformatted("Pad Inspector");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            dl->AddRectFilled(ImGui::GetCursorScreenPos(),
                          ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x,
                                 ImGui::GetCursorScreenPos().y + 92),
                          IM_COL32(20, 26, 36, 235), 14.0f);

            ImGui::BeginChild("##InspectorCard", ImVec2(0, 92), false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(cAccent), "+  Airhorn");
            ImGui::PushStyleColor(ImGuiCol_Text, cTextDim);
            ImGui::TextUnformatted("  ./assets/sounds/airhorn.wav");
            ImGui::PopStyleColor();

            ImGui::EndChild();

            static float vol = 0.72f;
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(cTextDim), "Volume:");
            ImGui::SliderFloat("##vol", &vol, 0.0f, 1.0f, "");

            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(158, 19, 9, 180));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(168, 25, 15, 220));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(204, 69, 59, 210));
            if (ImGui::Button("Stop Sound", ImVec2(-1, 36))) {
                SBaudio::Audio_Shutdown();
                SBaudio::Audio_Init();
                std::cout << "Audio Force Stopped\n";
            };
            ImGui::PopStyleColor(3);

            ImGui::SameLine();

            ImGui::Separator();
            ImGui::Spacing();

            static int mode = 0;
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(cTextDim), "Mode:");
            ImGui::SameLine();
            if (ImGui::RadioButton("One-shot", &mode, 0)) {
                soundGlobals::shouldLoop = false;
            };
            ImGui::SameLine();
            if (ImGui::RadioButton("Loop", &mode, 1)) {
                soundGlobals::shouldLoop = true;
            };

            ImGui::Spacing();
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(cTextDim), "Log:");
            ImGui::BeginChild("##Log", ImVec2(0, 120), true);
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(cOk),  "• Loaded from Default.ini");
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(cWarn), "• Missing: bruh.wav");
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(cTextDim), "• Device changed to \"Speakers\"");
            ImGui::EndChild();

            ImGui::PopStyleVar();
            ImGui::EndChild();

            ImGui::EndChild();
            ImGui::EndChild();

            ImGui::End();
            ImGui::PopStyleVar(3);


            ImGui::Render();

            const float clearColor[4] = { 0.10f, 0.10f, 0.10f, 1.00f };
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clearColor);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            g_pSwapChain->Present(1, 0);

        } // end of while(!done)

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();
        DestroyWindow(hwnd);
        UnregisterClass(kClassName, hInstance);

    } // end of Run()
}

#include "pch.h"
#pragma once

#include <Windows.h>
#include<string>

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "directx9.h"
#include "mem.h"
#include "drawing.h"
#include "includes.h"
#include "hacks.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"


extern LPDIRECT3DDEVICE9 pDevice = nullptr;
tEndScene oEndScene = nullptr;
BYTE EndSceneBytes[7]{ 0 };
void* d3d9Device[119];

D3DCOLOR yellow = D3DCOLOR_ARGB(255, 255, 255, 1);
D3DCOLOR red = D3DCOLOR_ARGB(255, 255, 1, 1);
D3DCOLOR green = D3DCOLOR_ARGB(255, 1, 255, 1);
D3DCOLOR blue = D3DCOLOR_ARGB(255, 1, 1, 255);
D3DCOLOR white = D3DCOLOR_ARGB(255, 255, 255, 255);
D3DCOLOR black = D3DCOLOR_ARGB(255, 1, 1, 1);

hacks* hack;
int counter_deadlock = 0;
int menuX = 200;
int menuY = 200;

bool initialized;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC oWndProc = nullptr;
LRESULT __stdcall Hooked_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ImGuiIO& io = ImGui::GetIO();
	/*
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
	if (io.WantCaptureMouse && (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEMOVE))
	{
		return TRUE;
	}*/
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return 1;
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

int getMyWeapon()
{
	int weaponEntity = *reinterpret_cast<DWORD*>(hack->client + hack->entity_list_off + ((hack->localent->activeWeapon & 0xFFF) - 1) * 0x10);
	
	if (weaponEntity != NULL){
		int weaponid = *reinterpret_cast<DWORD*>(weaponEntity + hack->itemDefIndex);
		return weaponid;
	}
	return 0;
}

void setTBDelay(float distance)
{
	float delay;
	if (hack->setting.triggerbot_custom) {
		delay = hack->setting.triggerbot_delay;
	}
	else {
		switch (getMyWeapon())
		{
		case 1: delay = 0.1; break;
		case 7: delay = 0.8; break;
		case 9: delay = 0.8; break;
		case 40: delay = 0.1; break;
		default: delay = 0.9;
		}
	}
	if (distance > 150)
		distance = 150;
	hack->tbDelay = distance * delay;
}

bool checkTBot()
{
	int crosshair = hack->localent->crosshairid;
	crosshair = crosshair - 1;
	if (crosshair != 0 && crosshair < 32)
	{
		if (!hack->checkValidityEnt(hack->entlist->ents[crosshair].ent))
			return false;
		int eTeam = hack->entlist->ents[crosshair].ent->iTeamNum;
		int eHealth = hack->entlist->ents[crosshair].ent->iHealth;
		if (eTeam!= hack->localent->iTeamNum && eHealth > 0)
		{
			int weaponEntity = *reinterpret_cast<DWORD*>(hack->client + hack->entity_list_off + ((hack->localent->activeWeapon & 0xFFF) - 1) * 0x10);
			//float Accuracypenalty = 0;
			float Accuracypenalty = *reinterpret_cast<float*>(weaponEntity + offsets::m_fAccuracyPenalty);
			if (Accuracypenalty > 0.1f)
				return false;
			float distance = hack->getDistance(crosshair);
			int myWeaponID = getMyWeapon();
			setTBDelay(distance);
			if (myWeaponID == 40 || myWeaponID == 38 || myWeaponID == 9)
				return hack->checkIfScoped();
			else if (myWeaponID == 49 || myWeaponID == 48 || myWeaponID == 47 || myWeaponID == 46 || myWeaponID == 45 || myWeaponID == 44 || myWeaponID == 43 || myWeaponID == 42 || myWeaponID == 41)
				return false;
			else
				return true;
		}
		else
			return false;
	}
	else
		return false;
}

void TriggerBot()
{
	if (checkTBot())
		hack->shoot();
	
}

void InitImGui(IDirect3DDevice9* pDevice) {

	ImGui::CreateContext();
	D3DDEVICE_CREATION_PARAMETERS CP;
	pDevice->GetCreationParameters(&CP);
	window = CP.hFocusWindow;
	
	oWndProc = (WNDPROC)SetWindowLongA(window, GWL_WNDPROC, (LONG)Hooked_WndProc);
	
	
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	//io.ConfigFlags = ImGuiConfigFlags_NoMouse;
	io.Fonts->AddFontDefault();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(pDevice);
	initialized = true;
	return;
}

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice) {
	if (!pDevice)
		pDevice = o_pDevice;
	// menu
	if (!initialized) InitImGui(pDevice);
	else {
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		if (hack->setting.menu)
		{
			bool bShow = true;
			{
				ImVec4* colors = ImGui::GetStyle().Colors;
				colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 0.00f, 1.00f, 1.00f);
				colors[ImGuiCol_WindowBg] = ImVec4(0.24f, 0.00f, 1.00f, 0.38f);
				colors[ImGuiCol_FrameBg] = ImVec4(0.51f, 0.00f, 0.63f, 0.66f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(0.70f, 0.40f, 1.00f, 0.40f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.68f, 0.30f, 1.00f, 0.67f);
				colors[ImGuiCol_TitleBg] = ImVec4(0.48f, 0.00f, 1.00f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.54f, 0.00f, 1.00f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.38f, 0.00f, 1.00f, 0.28f);
				colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.53f, 0.00f, 1.00f);
				colors[ImGuiCol_SliderGrab] = ImVec4(0.44f, 0.63f, 0.88f, 1.00f);
				colors[ImGuiCol_SliderGrabActive] = ImVec4(0.36f, 0.19f, 0.82f, 1.00f);
				colors[ImGuiCol_Button] = ImVec4(0.77f, 0.00f, 1.00f, 0.40f);
				colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.26f, 0.98f, 1.00f);
				colors[ImGuiCol_Tab] = ImVec4(0.40f, 0.18f, 0.58f, 0.86f);
				colors[ImGuiCol_TabHovered] = ImVec4(0.49f, 0.09f, 1.00f, 0.80f);
				colors[ImGuiCol_TabActive] = ImVec4(0.61f, 0.00f, 1.00f, 1.00f);
				

				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("CSGO hacks");
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);// Create a window called "Hello, world!" and append into it.
				ImGui::Text("");
				ImGui::Text("Please press ESC to use mouse on this menu");
				ImGui::Text("F1 to show/hide this menu");
				ImGui::Text("");
				static bool reset;
				ImGui::Text("Started a new match? reset the hack");
				reset = ImGui::Button("reset");
				if(reset)
					hack->init();
				ImGui::Text("");
				ImGui::Text("BASIC:");
				static bool check = false;
				ImGui::Checkbox("Recommended settings", &check); ImGui::SameLine(); HelpMarker(
					"Glow hack      ON\n"
					"Show teammates ON\n"
					"Bunnyhop	    ON\n"
					"Radar hack     ON\n"
					"No flash       ON\n"
					"Trigger bot    ON"); ImGui::SameLine(); ImGui::Text("for basic/chill users");
				if (check) {
					hack->setting.glow = true;
					hack->setting.show_teammates = true;
					hack->setting.bHop = true;
					hack->setting.radar = true;
					hack->setting.noflash = true;
					hack->setting.triggerbot = true;
				}
				ImGui::Text("For more info/options, visit the advance tab below");
				ImGui::Text("To activate the bunnyhopping, hold SPACEBAR while moving");
				ImGui::Text("To activate the trigger bot ingame, press T");
				ImGui::Text("");
				ImGui::Text("ADVANCE:");
				ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
				if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
				{
					if (ImGui::BeginTabItem("Walls"))
					{
						ImGui::Text("Here you will find all the options to tweak wall hacks");
						ImGui::Text("");
						ImGui::Checkbox("Show teammates", &hack->setting.show_teammates); ImGui::SameLine(); HelpMarker(
							"See teammates as well through walls.");
						//ImGui::Text("");
						ImGui::Checkbox("Glow hacks - personal favourite", &hack->setting.glow); ImGui::SameLine(); HelpMarker(
							"Enemies glow behind walls - color is dependent on their health");
						//ImGui::Text("");
						ImGui::Checkbox("Health and armour status lines", &hack->setting.status2d); ImGui::SameLine(); HelpMarker(
							"Show health and armour as vertical lines.");
						//ImGui::Text("");
						ImGui::Checkbox("Guidance lines", &hack->setting.snaplines); ImGui::SameLine(); HelpMarker(
							"Track the position of foes using lines from your feet to enemy's.");
						//ImGui::Text("");
						ImGui::Checkbox("3D Box (ESP box)", &hack->setting.espbox_3D); ImGui::SameLine(); HelpMarker(
							"See a 3D box around the enemy.");
						//ImGui::Text("");
						ImGui::Checkbox("2D Box (ESP box)", &hack->setting.espbox_2D); ImGui::SameLine(); HelpMarker(
							"See a 2D box around the enemy.");

						//ImGui::Text("");
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Aim"))
					{
						ImGui::Text("Here you will find all the options to tweak aim hacks");
						ImGui::Text("");
						ImGui::Checkbox("Recoil crosshair", &hack->setting.recCrosshair); ImGui::SameLine(); HelpMarker(
							"Show an additional crosshair which shows recoil");
						//ImGui::Text("");
						bool aimbot = false;
						ImGui::Checkbox("Aim bot - DONT RAGE", &aimbot); ImGui::SameLine(); HelpMarker(
							"Aimbot is only activated when you are shooting.");
						if(aimbot)
							ImGui::Text("In maintanence");
						//ImGui::Text("");
						ImGui::Checkbox("Enable TriggerBot", &hack->setting.triggerbot); ImGui::SameLine(); HelpMarker(
							"Automatically fire when enemy moves into crosshair");
						bool status = hack->setting.triggerbot;
						//if (!status)
						//	ImGui::BeginDisabled();
						if (hack->setting.triggerbot){
						ImGui::Text("Adjuct trigger bot shooting delay below"); ImGui::SameLine(); HelpMarker(
							"To make triggerbot look legit, theres a delay added.\n"
							"Distance of the shot is also included in the final calculation.");
						static int e = 0;
						ImGui::RadioButton("Automatic delay", &e, 0); ImGui::SameLine(); HelpMarker(
							"different for each weapon - managed automatically"); ImGui::SameLine();
						ImGui::RadioButton("Manual delay", &e, 1);
						if (e == 1)
							hack->setting.triggerbot_custom = true;
						else
							hack->setting.triggerbot_custom = false;
						ImGui::SliderFloat("delay", &f, 0.0f, 3.0f);
						ImGui::Text("Dont use 0 unless you KennyS. recommended 0.9 for normal players");
						if (e == 1)
							hack->setting.triggerbot_delay = f;
						//if (!status)
						//	ImGui::BeginDisabled();
						}
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Miscellaneous"))
					{
						ImGui::Text("Here you will find all the other cool options");
						ImGui::Text("");
						ImGui::Checkbox("Bunny hop", &hack->setting.bHop); ImGui::SameLine(); HelpMarker(
							"Bunny hop your way around the map wasily with this. Hold SPACEBAR to bunny hop.");
						//ImGui::Text("");
						ImGui::Checkbox("Radar hack", &hack->setting.glow); ImGui::SameLine(); HelpMarker(
							"See all the enemies in your redar whether they are in your sight or not. Enemies need to be somewhat near you, for the server to send their details.");
						//ImGui::Text("");
						ImGui::Checkbox("NoFlash - look through flashes", &hack->setting.noflash); ImGui::SameLine(); HelpMarker(
							"No more flashbangs making your enemy john cena.");
						//ImGui::Text("");
						ImGui::Checkbox("Headline ESP", &hack->setting.headlineesp); ImGui::SameLine(); HelpMarker(
							"View the headline or viewing angles of your enemies.");
						//ImGui::Text("");
//						ImGui::Checkbox("3D Box (ESP box)", &hack->setting.espbox_3D); ImGui::SameLine(); HelpMarker(
//							"See a 3D box around the enemy.");
						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}
				ImGui::Separator();
				ImGui::End();
			}
		}

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	//showmenu();
	hack->checkButtons();


		vector2 l, r, t, b;
		l = r = t = b = hack->crossHair2D;
		l.x -= hack->crossHairSize;
		r.x += hack->crossHairSize;
		t.y -= hack->crossHairSize;
		b.y += hack->crossHairSize;
		// crosshair
		if (hack->setting.recCrosshair) {
			DrawLine(l, r, 2, yellow);
			DrawLine(t, b, 2, yellow);

		}
		//no flash
		if (hack->setting.noflash && hack->checkValidityEnt(hack->localent))
		{
			float duration = hack->localent->fFlash;
			if(duration>0)
				hack->localent->fFlash = 0.0f;
		}
		if(hack->setting.bHop)
			hack->bHop();

		//trigger bot
		if (hack->setting.triggerbot && hack->setting.canTBot)
			TriggerBot();
	

	// 
	// 
	// esp box and lines and bars
	for (int i = 1; i <= 31; i++) {

		entity* curEnt = hack->entlist->ents[i].ent;
		if (!hack->checkValidityEnt(curEnt))
			continue;

		vector2 entpos, entHead2d;
		vector3 entpos3d = curEnt->vecOrigin;
		vector3 entHead3d = hack->GetBonePos(curEnt, 8);


		if (hack->WorldToScreen(entpos3d, entpos)) {

			if (curEnt->iTeamNum == hack->localent->iTeamNum)
			{
				if (hack->setting.show_teammates) {
					
					if (hack->setting.glow)
						hack->setglow(curEnt, true);

					if (hack->setting.snaplines)
						DrawLine(entpos.x, entpos.y, windowWidth / 2, windowHeight, 3, green);

					if (hack->WorldToScreen(entHead3d, entHead2d)) {
						entHead3d.z += 10;//to fit the head inside
						if (hack->setting.espbox_2D)
							DrawEspBox2D(entpos, entHead2d, 1, green);
						if (hack->setting.status2d)
							DrawHealthandArm(curEnt, entpos, entHead2d, 1, yellow, blue);
						if (hack->setting.espbox_3D)
							DrawEspBox3D(entHead3d, entpos3d, curEnt->angEyeAnglesY, 25, 1, green);
						if (hack->setting.statustext)
							printHealthandArm(curEnt, entHead2d, white);
						if (hack->setting.headlineesp)
							headlineESP(curEnt, 10, 1, blue);
					}
				}
			}
			else {
				//hack->setColor(curEnt, 1, 0, 0, 5);
				if(hack->setting.radar)
					curEnt->isSpotted = true;
	
				if(hack->setting.glow)
					hack->setglow(curEnt, false);
				
				if (hack->setting.snaplines)
					DrawLine(entpos.x, entpos.y, windowWidth / 2, windowHeight, 3, red);

				if (hack->WorldToScreen(entHead3d, entHead2d)) {
					entHead3d.z += 10; //to fit the head inside
					if (hack->setting.espbox_2D)
						DrawEspBox2D(entpos, entHead2d, 1, red);
					if (hack->setting.status2d)
						DrawHealthandArm(curEnt, entpos, entHead2d, 1, yellow, blue);
					if (hack->setting.espbox_3D)
						DrawEspBox3D(entHead3d, entpos3d, curEnt->angEyeAnglesY, 25, 1, red);
					if (hack->setting.statustext)
						printHealthandArm(curEnt, entHead2d, white);
					if (hack->setting.headlineesp)
						headlineESP(curEnt, 10, 1, blue);
				}

			}

		}

	}
	// call og function
	oEndScene(pDevice);
}

DWORD WINAPI HackThread(HMODULE hModule) {
	// hook
	hack = new hacks;

	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
		hack->init();
		memcpy(EndSceneBytes, (char*)d3d9Device[42], 7);

		oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);

	}
	// hack loop
	while (!GetAsyncKeyState(VK_END)) {
		hack->update();
		if(hack->setting.recCrosshair){
		vector3 pAng = hack->localent->aimpunchang;
		hack->crossHair2D.y = windowHeight / 2 + (windowHeight / 90 * pAng.x);
		hack->crossHair2D.x = windowWidth / 2 - (windowWidth / 90 * pAng.y);
		}
	}

	// unhook
	Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7);

	//to give time for injection and stuff
	Sleep(1000);

	// uninject
	FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpr) {
	if (reason == DLL_PROCESS_ATTACH)
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0));
	return TRUE;
}


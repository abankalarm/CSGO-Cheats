#pragma once
#include "pch.h"
#include "windows.h"
#include "includes.h"
#include "csgo.h"
#include "iostream"
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>

// all the order has been changed to bypass anti cheats
extern int counter_deadlock;
//auto pad magic
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

struct ClrRender // this for color or tint change
{
	BYTE red, green, blue;
};


struct vector2 {
	float x, y;
};


struct vector3 {
	float x, y, z;
};


struct vector4
{
	float x, y, z, w;
};


struct settings
{
	bool menu = false;

	bool show_teammates = false;

	bool glow = false;
	bool espbox_2D = false;
	bool espbox_3D = false;
	bool snaplines = false;
	bool status2d = false;
	bool statustext = false;

	bool velesp = false;

	bool recCrosshair = false;
	bool triggerbot = false;
	bool triggerbot_custom = false;
	float triggerbot_delay = 0.000f;
	bool canTBot = true;

	bool headlineesp = false;
	bool noflash = false;
	bool radar = false;
	bool bHop = false;
};


class entity {
public:
	//define our variables from offsets
	union {
		DEFINE_MEMBER_N(bool, isDormant, offsets::m_bDormant);
		DEFINE_MEMBER_N(int, iHealth, offsets::m_iHealth);
		DEFINE_MEMBER_N(vector3, vecOrigin, offsets::m_vecOrigin);
		DEFINE_MEMBER_N(int, iTeamNum, offsets::m_iTeamNum);
		DEFINE_MEMBER_N(int, bonematrix, offsets::m_dwBoneMatrix);
		DEFINE_MEMBER_N(int, armourval, offsets::m_ArmorValue);
		DEFINE_MEMBER_N(vector3, aimpunchang, offsets::m_aimPunchAngle);
		DEFINE_MEMBER_N(vector3, vecVelocity, offsets::m_vecVelocity);
		DEFINE_MEMBER_N(float, angEyeAnglesX, offsets::m_angEyeAnglesX);
		DEFINE_MEMBER_N(float, angEyeAnglesY, offsets::m_angEyeAnglesY);
		DEFINE_MEMBER_N(bool, bHasHelmet, offsets::m_bHasHelmet);

		DEFINE_MEMBER_N(bool, scopeStat, offsets::m_bIsScoped);
		DEFINE_MEMBER_N(int, activeWeapon, offsets::m_hActiveWeapon);
		DEFINE_MEMBER_N(int, crosshairid, offsets::m_iCrosshairId);

		DEFINE_MEMBER_N(float, fFlash, offsets::m_flFlashDuration);
		DEFINE_MEMBER_N(bool, isSpotted, offsets::m_bSpotted);

		DEFINE_MEMBER_N(bool, isdefusing, offsets::m_bIsDefusing);
		DEFINE_MEMBER_N(bool, glowIndex, offsets::m_iGlowIndex);
		DEFINE_MEMBER_N(bool, m_clrRender, offsets::m_clrRender);

		//DEFINE_MEMBER_N(DWORD, fJump, offsets::dwForceJump);
		DEFINE_MEMBER_N(BYTE, fFlag, offsets::m_fFlags);
		//DEFINE_MEMBER_N(bool, isSpotted, offsets::veloci);
	};
};

class entitylistObj {
public:
	struct entity* ent;
	char padding[12];
};

class entitylist {
public:
	entitylistObj ents[32];
};

class hacks {
public:
	using Clock = std::chrono::steady_clock;
	std::chrono::time_point<std::chrono::steady_clock> start, now;
	std::chrono::milliseconds duration;
	
	//addresses and offsets
	uintptr_t entity_list_off = offsets::dwEntityList;
	uintptr_t view_mat_off = offsets::dwViewMatrix;
	uintptr_t glowobjManager = offsets::dwGlowObjectManager;
	uintptr_t model_ambient_min = offsets::model_ambient_min;
	uintptr_t fJump = offsets::dwForceJump;

	uintptr_t engine; //engine.dll
	uintptr_t client; //client.dll
	uintptr_t glowObject; // for glow
	entity* localent;
	entitylist* entlist;
	float viewmatrix[16]; //4 times float

	int tbDelay;
	
	//drawing
	ID3DXLine* lineL;
	ID3DXFont* FontF;

	DWORD itemDefIndex = offsets::m_iItemDefinitionIndex;
	uintptr_t forcelmb = offsets::dwForceLeft;

	//crossHair
	vector2 crossHair2D;
	int crossHairSize = 4;




	void init() {
		
		engine = (uintptr_t)GetModuleHandle(L"engine.dll");
		client = (uintptr_t)GetModuleHandle(L"client.dll");
		entlist = (entitylist*)(client + entity_list_off);
		//localent = entlist->ents[0].ent;
		start = Clock::now();
		localent = *(entity**)(client + offsets::dwLocalPlayer);
		std::cout << (localent->iHealth);
	}

	settings setting;

#define toggle(x) x=!x

	void checkButtons() {
		if (GetAsyncKeyState(VK_F1) & 1)
			toggle(setting.menu);
		if (GetAsyncKeyState(0x54) & 1)
			toggle(setting.canTBot);			
	}

	void update() {
		memcpy(&viewmatrix, (BYTE*)client + view_mat_off, sizeof(viewmatrix));
		this->checkButtons();
	}

	int checkValidityEnt(entity* ent) {
		if (ent == nullptr || ent == localent || ent->iHealth <= 0 || ent->isDormant)
			return FALSE;
		return TRUE;
	}

	bool WorldToScreen(vector3 pos, vector2& screen)
	{
		//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
		vector4 clipCoords;
		clipCoords.x = pos.x * viewmatrix[0] + pos.y * viewmatrix[1] + pos.z * viewmatrix[2] + viewmatrix[3];
		clipCoords.y = pos.x * viewmatrix[4] + pos.y * viewmatrix[5] + pos.z * viewmatrix[6] + viewmatrix[7];
		clipCoords.z = pos.x * viewmatrix[8] + pos.y * viewmatrix[9] + pos.z * viewmatrix[10] + viewmatrix[11];
		clipCoords.w = pos.x * viewmatrix[12] + pos.y * viewmatrix[13] + pos.z * viewmatrix[14] + viewmatrix[15];

		if (clipCoords.w < 0.1f)
			return false;

		//perspective division, dividing by clip.W = Normalized Device Coordinates
		vector3 NDC;
		NDC.x = clipCoords.x / clipCoords.w;
		NDC.y = clipCoords.y / clipCoords.w;
		NDC.z = clipCoords.z / clipCoords.w;

		screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
		screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
		return true;
	}

	vector3 GetBonePos(entity* ent, int bone) {
		vector3 bonePos;
		uintptr_t bonePtr = ent->bonematrix;
		uintptr_t b = 0x0C;
		bonePos.x = *(float*)(bonePtr + 0x30 * bone + b);
		b += 0x10;
		bonePos.y = *(float*)(bonePtr + 0x30 * bone + b);
		b += 0x10;
		bonePos.z = *(float*)(bonePtr + 0x30 * bone + b);
		return bonePos;
	}


#define TORAD(x) ((x)*0.01745329252)

	vector3 tranformVector(vector3 src, vector3 ang, float d) {
		vector3 newPos;
		newPos.y = src.y + ((sinf(TORAD(ang.y)) * d));
		newPos.x = src.x + ((cosf(TORAD(ang.y)) * d));
		newPos.z = src.z + ((tanf(TORAD(ang.y)) * d));
		return newPos;
	}

	//triggerbot

	bool checkIfScoped()
	{
		return localent->scopeStat;
	}

	float getDistance(int ent)
	{
		vector3 myLocation = localent->vecOrigin;
		vector3 enemylocation = entlist->ents[ent].ent->vecOrigin;
		return sqrt(pow(myLocation.x - enemylocation.x, 2) + pow(myLocation.z - enemylocation.z + pow(myLocation.y - enemylocation.y, 2) , 2)) * 0.02543;
	}

	void Patch(BYTE* dst, BYTE* src, unsigned int size)
	{
		DWORD oldprotect;

		VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		memcpy(dst, src, size);
		VirtualProtect(dst, size, oldprotect, &oldprotect);
	}

	void shoot()
	{
		INPUT input{ 0 };
		input.type = INPUT_MOUSE;
		/*
		using Clock = std::chrono::steady_clock;
		std::chrono::time_point<std::chrono::steady_clock> start, now;
		std::chrono::milliseconds duration;	
		start = Clock::now();
		*/
		
		now = Clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

		if (duration.count() >= tbDelay)
		{
			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			SendInput(1, &input, sizeof(INPUT));

			input.mi.dwFlags = MOUSEEVENTF_LEFTUP;

			//std::this_thread::sleep_for(std::chrono::milliseconds(30));
			start = std::chrono::steady_clock::now();
			SendInput(1, &input, sizeof(INPUT));
		}	
		
	}


	// glow hacks and color hacks !!!

	void setglow(entity* ent, bool friendly)
	{
		bool defusing;
		int health = 100;
		if(checkValidityEnt(ent)){
			health = ent->iHealth;
			defusing = ent->isdefusing;
		}
		float red = 1 + health * -0.01;
		float green = health * 0.01;
		
		glowObject = *(uintptr_t*)(client + glowobjManager);

		if (checkValidityEnt(ent)) {
			try {
				if (defusing)
				{
					*reinterpret_cast<float*>(glowObject + (ent->glowIndex * 0x38) + 0x8) = 1;
					*reinterpret_cast<float*>(glowObject + (ent->glowIndex * 0x38) + 0xC) = 1;
					*reinterpret_cast<float*>(glowObject + (ent->glowIndex * 0x38) + 0x10) = 1;
					*reinterpret_cast<float*>(glowObject + (ent->glowIndex * 0x38) + 0x14) = 1.7;
					*reinterpret_cast<bool*>(glowObject + (ent->glowIndex * 0x38) + 0x28) = true;
					*reinterpret_cast<bool*>(glowObject + (ent->glowIndex * 0x38) + 0x29) = false;
				}
				else if (friendly == false)
				{
					if (checkValidityEnt(ent)){
						*reinterpret_cast<float*>(glowObject + (ent->glowIndex * 0x38) + 0x8) = red;
						*reinterpret_cast<float*>(glowObject + (ent->glowIndex * 0x38) + 0xC) = green;
					}
					if (checkValidityEnt(ent))
						*reinterpret_cast<float*>(glowObject + (ent->glowIndex * 0x38) + 0x14) = 0.99f;
				}
				else {
					if (checkValidityEnt(ent))
					{
					*reinterpret_cast<float*>(glowObject + (ent->glowIndex * 0x38) + 0x8) = 0;
					*reinterpret_cast<float*>(glowObject + (ent->glowIndex * 0x38) + 0xC) = 0;
					}
					if (checkValidityEnt(ent)){
					*reinterpret_cast<float*>(glowObject + (ent->glowIndex * 0x38) + 0x10) = 2;
					*reinterpret_cast<float*>(glowObject + (ent->glowIndex * 0x38) + 0x14) = 1.7;
					}
				}
				if (checkValidityEnt(ent)){
					*reinterpret_cast<bool*>(glowObject + (ent->glowIndex * 0x38) + 0x28) = true;
					*reinterpret_cast<bool*>(glowObject + (ent->glowIndex * 0x38) + 0x29) = false;
				}
			}
			catch (...) {
				std::cout << "violation";
			}
		}
	}

	void setColor(entity* ent, float red, float green, float blue, float brightness) {
		ClrRender clr = *reinterpret_cast<ClrRender*>(ent->m_clrRender);
		clr.red = red;
		clr.green = green;
		clr.blue = blue;
		int ptr = *reinterpret_cast<int*>(engine + model_ambient_min);
		*reinterpret_cast<int*>(engine + model_ambient_min) = *(int*)&brightness ^ ptr;
	}

//bunny hop
	void bHop() {
			if (localent->vecVelocity.x + localent->vecVelocity.y + localent->vecVelocity.z) {
				if (GetAsyncKeyState(VK_SPACE) && localent->fFlag & (1 << 0))
					*(DWORD*)(client + fJump) = 6;
			}
	}
};
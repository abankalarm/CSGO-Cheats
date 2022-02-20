#pragma once
#include "pch.h"
#include "includes.h"
#include "hacks.h"



void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color);
void _DrawText(const char* text, float x, float y, D3DCOLOR color, D3DCOLOR shadow);
void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR col);
void DrawBorder(int x, int y, int w, int h, int thickness, D3DCOLOR color);

// type 0 = normal with no color for different (pass just on, off as anything maybe)
// type 1 = different color for on and off 
bool CheckBox(int x, int y, bool var, POINT mouse, char* name, D3DCOLOR text, D3DCOLOR frame, D3DCOLOR ONcolor, D3DCOLOR OFFcolor);
void DrawRadio();


// hacks
void DrawEspBox2D(vector2 top, vector2 bottom, int thickness, D3DCOLOR color);
void DrawEspBox3D(vector3 top, vector3 bottom, float a, int width, int thickness, D3DCOLOR color);
void DrawHealthandArm(entity* curEnt, vector2 top, vector2 bottom, int thickness, D3DCOLOR hColor, D3DCOLOR aColor);
void printHealthandArm(entity* curEnt, vector2 top, D3DCOLOR color);
void headlineESP(entity* curEnt, int bone, int thickness, D3DCOLOR color);
void DrawLine(vector2 src, vector2 dest, int thickness, D3DCOLOR color);

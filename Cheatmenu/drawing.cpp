#include "pch.h"
#include "includes.h"
#include <string>

extern hacks* hack;

#define ABS(x) (x<=0)?(-x):(x)


// filled rectangle
void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color) {
	D3DRECT rect = { x,y,x + w,y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawBorder(int x, int y, int w, int h, int thickness, D3DCOLOR color)
{
	// doing all calculatons in a single time
	int el[] = { x, y, w,thickness, h, x + (w - thickness),y + (w - thickness) };
	// use maths for this 4 rectangles now for border , easy
	DrawFilledRect(el[0], el[1], el[2] , el[3], color);
	DrawFilledRect(el[0], el[1], el[3], el[4], color);
	DrawFilledRect(el[5], el[1], el[3], el[4], color);
	DrawFilledRect(el[0], el[6], el[2], el[3], color);
}

bool CheckBox(int x, int y, bool var, POINT mouse ,char* name, D3DCOLOR text, D3DCOLOR frame, D3DCOLOR ONcolor, D3DCOLOR OFFcolor)
{
	int framesize = 10;
	int window = 0;
	DrawBorder(x, y, framesize, framesize, 1, frame);
	
	if (mouse.x >= x && mouse.x <= x + framesize) {
		if (((mouse.y) - (y+window)) >= 0 && (mouse.y) <= y + window + framesize) {
			DrawFilledRect(x + 1, y + 1, 8, 8, OFFcolor);
			if (GetAsyncKeyState(VK_LBUTTON) & 1) {
				var = !var;
			}
		}
	}
	if (var) {
		DrawFilledRect(x + 1, y + 1, 8, 8, ONcolor);
	}
	D3DCOLOR black = D3DCOLOR_ARGB(255, 1, 1, 1);
	_DrawText(name, x + 50, y, text, black);
	return var;
}

void DrawRadio()
{
}



void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color) {
	//ID3DXLine* lineL; moved to hack for efficiency
	if (!hack->lineL)
		D3DXCreateLine(pDevice, &hack->lineL);

	D3DXCreateLine(pDevice, &hack->lineL);
	D3DXVECTOR2 Line[2];
	Line[1] = D3DXVECTOR2(x2, y2);
	Line[0] = D3DXVECTOR2(x1, y1);
	hack->lineL->SetWidth(thickness);
	hack->lineL->Draw(Line, 2, color);
	hack->lineL->Release();
}

void _DrawText(const char* text, float x, float y, D3DCOLOR color, D3DCOLOR shadow)
{
	
	RECT rect;
	if (!hack->FontF)
		D3DXCreateFontA(pDevice, 14, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH || FF_DONTCARE, "arial", &hack->FontF);

	//shadow for text
	SetRect(&rect, x + 1, y + 1, x + 1, y + 1);
	hack->FontF->DrawTextA(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, shadow);
	//actual text
	SetRect(&rect, x, y, x, y);
	hack->FontF->DrawTextA(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, color);
}


void DrawHealthandArm(entity* curEnt, vector2 top, vector2 bottom, int thickness, D3DCOLOR hColor, D3DCOLOR aColor)
{
	// bottom and top are esentially entpos and enthead lol but who cares at the point
	int dX = bottom.x - bottom.x;
	float heathpercentage = curEnt->iHealth / 100.f;
	float armourpercentage = curEnt->iHealth / 100.f;

	vector2 bottom_health, top_health, bottom_armour, top_armour;

	int height = ABS(top.y - bottom.y);
	int a_height = height * armourpercentage;
	int h_height = height * heathpercentage;

	bottom_armour.y = bottom_health.y = top.y;

	bottom_health.x = top.x - (height / 4) - 3;
	bottom_armour.x = top.x + (height / 4) + 3;

	top_health.x = top.x - (height / 4) - 3;
	top_armour.x = top.x + (height / 4) + 3;

	top_health.y = bottom.y + height - h_height;
	top_armour.y = bottom.y + height - a_height;


	DrawLine(bottom_armour, top_armour, 2, aColor);
	DrawLine(bottom_health, top_health, 2, hColor);
}



void DrawEspBox2D(vector2 top, vector2 bottom, int thickness, D3DCOLOR color)
{
	int height = ABS(top.y - bottom.y);
	vector2 tl, tr, bl, br;
	tl.x = top.x - height / 4;
	tr.x = top.x + height / 4;
	tl.y = tr.y = top.y;
	bl.x = bottom.x - height / 4;
	br.x = bottom.x + height / 4;
	bl.y = br.y = bottom.y;

	DrawLine(tl, tr, thickness, color);
	DrawLine(tl, bl, thickness, color);
	DrawLine(tr, br, thickness, color);
	DrawLine(bl, br, thickness, color);
}


void DrawLine(vector2 src, vector2 dest, int thickness, D3DCOLOR color)
{
	DrawLine(src.x, src.y, dest.x, dest.y, thickness, color);
}


void DrawEspBox3D(vector3 top, vector3 bottom, float a, int width, int thickness, D3DCOLOR color)
{
	int Height3d = ABS(top.z - bottom.z);
	vector3 t1, t2, t3, t4, b1, b2, b3, b4; //bottom and top
	b1.z = b2.z = b3.z = b4.z = bottom.z;
	t1.z = t2.z = t3.z = t4.z = bottom.z + Height3d;

	t1.x = b1.x = bottom.x + (cos(TORAD(a + 45)) * width);
	t2.x = b2.x = bottom.x + (cos(TORAD(a + 45 + 90)) * width);
	t3.x = b3.x = bottom.x + (cos(TORAD(a + 45 + 90 + 90)) * width);
	t4.x = b4.x = bottom.x + (cos(TORAD(a + 45 + 90 + 90 + 90)) * width);

	t1.y = b1.y = bottom.y + (sin(TORAD(a + 45)) * width);
	t2.y = b2.y = bottom.y + (sin(TORAD(a + 45 + 90)) * width);
	t3.y = b3.y = bottom.y + (sin(TORAD(a + 45 + 90 + 90)) * width);
	t4.y = b4.y = bottom.y + (sin(TORAD(a + 45 + 90 + 90 + 90)) * width);

	//convert to 2d for draw 
	vector2 b1_2d, b2_2d, b3_2d, b4_2d, t1_2d, t2_2d, t3_2d, t4_2d;
	hack->WorldToScreen(b1, b1_2d);
	hack->WorldToScreen(b2, b2_2d);
	hack->WorldToScreen(b3, b3_2d);
	hack->WorldToScreen(b4, b4_2d);
	hack->WorldToScreen(t1, t1_2d);
	hack->WorldToScreen(t2, t2_2d);
	hack->WorldToScreen(t3, t3_2d);
	hack->WorldToScreen(t4, t4_2d);

	//base
	DrawLine(b1_2d, b2_2d, thickness, color);
	DrawLine(b2_2d, b4_2d, thickness, color);
	DrawLine(b3_2d, b4_2d, thickness, color);
	DrawLine(b4_2d, b1_2d, thickness, color);

	//top
	DrawLine(t1_2d, t2_2d, thickness, color);
	DrawLine(t2_2d, t3_2d, thickness, color);
	DrawLine(t3_2d, t4_2d, thickness, color);
	DrawLine(t4_2d, t1_2d, thickness, color);

	//columns
	DrawLine(t1_2d, b1_2d, thickness, color);
	DrawLine(t2_2d, b2_2d, thickness, color);
	DrawLine(t3_2d, b3_2d, thickness, color);
	DrawLine(t4_2d, b4_2d, thickness, color);

}


void printHealthandArm(entity* curEnt, vector2 top, D3DCOLOR color)
{
	int health = curEnt->iHealth;
	int armour = curEnt->iHealth;

	int height = top.y;
	int width = top.x;
	D3DCOLOR black = D3DCOLOR_ARGB(255, 1, 1, 1);
	std::string s = "Health: " + std::to_string(health);
	const char* healthText = s.c_str();
	_DrawText(healthText, top.x, top.y, color, black);

	std::string s1 = "Armour: " + std::to_string(armour);
	const char* armourText = s1.c_str();
	_DrawText(armourText, top.x, top.y + 15, color, black);

	if (!curEnt->bHasHelmet) {
		std::string s2 = "NO HELMET";
		const char* helmetstat = s2.c_str();
		_DrawText(helmetstat, top.x, top.y + 30, color, black);
	}

}

void headlineESP(entity* curEnt, int bone, int thickness, D3DCOLOR color)
{
	vector3 head3d, entAngles;
	head3d = hack->GetBonePos(curEnt, bone);
	entAngles.z = 0;
	entAngles.x = curEnt->angEyeAnglesX;
	entAngles.y = curEnt->angEyeAnglesY;
	vector2 endpoint_2d, head2d;
	vector3 endpoint = hack->tranformVector(head3d, entAngles, 50);
	hack->WorldToScreen(endpoint, endpoint_2d);
	hack->WorldToScreen(head3d, head2d);
	DrawLine(head2d, endpoint_2d, thickness, color);
}

#include "Menu.h"
#include "TGFCfg.h"
#include "../SDK/Vector.h"
#include "../SDK/ISurface.h"
#include "../Utils/Color.h"
#include "../Utils/GlobalVars.h"
#include "config.h"
Menu g_Menu;
c_config hola_cfg;

//COLORS
Color Blyat(101, 101, 101, 255);
Color Cyka(35, 35, 35, 255);
Color Maricon(0, 0, 0, 255);
//COLORS
bool manually_set_key = false;

enum TABS
{
	Aimbot = 0,
	Visuals = 1,
	Misc = 2
};

#define or ||
#define and &&
int TopLeftGroupBox_X = 5;
int TopLeftGroupBox_Y = 52 + (309 / 1.5) + 5;

int GroupBoxSize_Width = 320 / 1.5;
int GroupBoxSize_Height = 287 / 1.5;


void GradientV(int x, int y, int w, int h, Color c1, Color c2)
{
	g_pSurface->AutoGradientRect(x, y, w, h, c1);
	BYTE first = c2.red;
	BYTE second = c2.green;
	BYTE third = c2.blue;
	for (int i = 0; i < h; i++)
	{
		float fi = i, fh = h;
		float a = fi / fh;
		DWORD ia = a * 255;
		g_pSurface->AutoGradientRect(x, y + i, w, 1, Color(first, second, third, ia));
	}
}

void GradientVEnd(int x, int y, int w, int h, Color c1, Color c2)
{
	//g_pSurface->AutoGradientRect(x, y, w, h, c1);
	BYTE first = c2.red;
	BYTE second = c2.green;
	BYTE third = c2.blue;
	for (float i = 0; i < h / 1.5; i++)
	{
		float fi = i, fh = h / 1.5;
		float a = fi / fh;
		DWORD ia = a * 255;
		g_pSurface->AutoGradientRect(x, y + i, w, 2, Color(first, second, third, ia));
	}
}


#include <tuple>

class vars : public singleton< vars > {
private:

public:
	bool visuals_box = false;
};


std::ptrdiff_t ExtraOffsets::deadflag = NULL;
std::string config_slots[6] = {
	"SLOT 1", "SLOT 2", "SLOT 3", "SLOT 4", "SLOT 5", "SLOT 6"
};

std::string Colors_slots[8] = {
	"Chams", "ChamsXQZ", "Glow", "Name", "Weapon", "Box", "Indicator", "Local Chams"
};

static int save_slot = 0;
#include "../Features/Visuals/EventLogging.h"
void Save_Config()
{
	Config2->Save(config_slots[save_slot]);

	std::string string_to_add;
	string_to_add += "saved '";
	string_to_add += config_slots[save_slot];
	string_to_add += "'.";

	//c_event_logs::get().add(string_to_add, Color(255, 255, 255, 255));
}

void Load_Config()
{
	Config2->Load(config_slots[save_slot]);

	std::string string_to_add;
	string_to_add += "loaded '";
	string_to_add += config_slots[save_slot];
	string_to_add += "'.";

	//c_event_logs::get().add(string_to_add, Color(255, 255, 255, 255));
}

void Export_Config()
{
	Config2->export_to_clipboard("export");
}

void Import_Config()
{
	Config2->import_from_clipboard("import");
}

void Save()
{
	std::string ConfigName;

	switch (2)
	{
	case 0: ConfigName = "1";
		break;
	case 1: ConfigName = "2";
		break;
	case 2: ConfigName = "3";
		break;
	case 3: ConfigName = "4";
		break;
	case 4: ConfigName = "5";
		break;
	}
	Config2->Save(ConfigName);
}

void GradientH(int x, int y, int w, int h, Color c1, Color c2)
{
	g_pSurface->AutoGradientRect(x, y, w, h, c1);
	BYTE first = c2.red;
	BYTE second = c2.green;
	BYTE third = c2.blue;
	for (int i = 0; i < w; i++)
	{
		float fi = i, fw = w;
		float a = fi / fw;
		DWORD ia = a * 255;
		g_pSurface->AutoGradientRect(x + i, y, 1, h, Color(first, second, third, ia));
	}
}

#include <experimental/filesystem>
#include <filesystem>
#include <iostream>

#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;
#include "../SDK/ClientClass.h"
#include "../SDK/IBaseClientDll.h"


DWORD FindSig(DWORD dwAddress, DWORD dwLength, const char* szPattern)
{
	if (!dwAddress || !dwLength || !szPattern)
		return 0;

	const char* pat = szPattern;
	DWORD firstMatch = NULL;

	for (DWORD pCur = dwAddress; pCur < dwLength; pCur++)
	{
		if (!*pat)
			return firstMatch;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == GET_BYTE(pat))
		{
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				return firstMatch;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else pat += 2;
		}
		else
		{
			pat = szPattern;
			firstMatch = 0;
		}
	}

	return 0;
}
DWORD FindSignaturenew(const char* szModuleName, const char* PatternName, char* szPattern)
{
	HMODULE hModule = GetModuleHandleA(szModuleName);
	PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hModule) + pDOSHeader->e_lfanew);

	DWORD ret = FindSig(((DWORD)hModule) + pNTHeaders->OptionalHeader.BaseOfCode, ((DWORD)hModule) + pNTHeaders->OptionalHeader.SizeOfCode, szPattern);;

	return ret;
}

void ForceDaUpdate() {

}

std::string text_uwu;
bool dont_recieve_input = false;


const wchar_t* to_wchar(const char* string) {
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, string);
	_vsnprintf_s(buf, sizeof(buf), string, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	return wbuf;
}

void Menu::spectators() {

	if (!c_config::get().spectators)
		return;

	if (g_pEngine->IsInGame() && g_pEngine->IsConnected()) {
		int index = 0;
		int w, h;
		g_pEngine->GetScreenSize(w, h);

		static unsigned int last_time;

		last_time = GetTickCount();
		Color rainbow;
		rainbow.FromHSV(fmod(last_time * 0.0002f, 1.f), 1.f, 0.5f);

		g_pSurface->DrawT(1835, 390, rainbow, Globals::CourierNew, false, "[SPECTATORS]");

		for (int i = 0; i <= 64; i++) {
			C_BaseEntity* e = (C_BaseEntity*)g_pEntityList->GetClientEntity(i);
			PlayerInfo_t pinfo;

			if (e && e != Globals::LocalPlayer && !e->IsDormant()) {
				g_pEngine->GetPlayerInfo(i, &pinfo);
				uintptr_t obs = e->observer_target();
				if (!obs) continue;

				C_BaseEntity* spec = (C_BaseEntity*)g_pEntityList->GetClientEntityFromHandle(obs);
				if (spec == nullptr) continue;
				PlayerInfo_t spec_info;
				g_pEngine->GetPlayerInfo(i, &spec_info);
				char buf[255]; sprintf_s(buf, "%s", pinfo.szName);
				if (spec->EntIndex() == Globals::LocalPlayer->EntIndex()) {
					g_pSurface->DrawT(1835, 400 + (10 * index), Color(100, 50, 50), Globals::CourierNew, false, buf);

					index++;
				}
			}
		}
	}

}


void Menu::Render()
{
	static unsigned int last_time2;
	last_time2 = GetTickCount();
	Color rainbow2;
	rainbow2.FromHSV(fmod(last_time2 * 0.0002f, 1.f), 1.f, 0.5f);

	spectators();

	static bool _pressed = false;

	if (!_pressed && GetAsyncKeyState(VK_INSERT))
		_pressed = true;
	else if (_pressed && !GetAsyncKeyState(VK_INSERT))
	{
		_pressed = false;
		menuOpened = !menuOpened;
	}
	g_pEngine->GetScreenSize(screen_width, screen_width);
	static Vector2D _mouse_pos;
	static int groupbox_bottom;
	static int _drag_x = 300;
	static int _drag_y = 300;
	int _width = 447;
	int _height = 350;
	static Vector2D oldPos;
	static Vector2D mousePos;
	static bool _dragging = false;
	bool _click = false;
	static bool _resizing = false;

	g_InputSystem->EnableInput(!menuOpened);
	if (menuOpened)
	{

		if (GetAsyncKeyState(VK_LBUTTON))
			_click = true;

		Vector2D _mouse_pos = g_pSurface->GetMousePosition();

		if (_dragging and !_click)
			_dragging = false;

		if (_resizing and !_click)
			_resizing = false;

		if (_dragging and _click)
		{
			_pos.x = _mouse_pos.x - _drag_x;
			_pos.y = _mouse_pos.y - _drag_y;
		}

		if (g_pSurface->MouseInRegion(_pos.x, _pos.y, _width, _height) && !(g_pSurface->MouseInRegion(
			_pos.x + 10, _pos.y + 20, _width - 10, _height - 10)))
		{
			_dragging = true;
			_drag_x = _mouse_pos.x - _pos.x;
			_drag_y = _mouse_pos.y - _pos.y;

		}

		g_pSurface->AutoGradientRect(_pos.x, _pos.y, _width, _height, Cyka);
		g_pSurface->OutlinedRect(_pos.x, _pos.y, _width, _height, Blyat);
		g_pSurface->OutlinedRect(_pos.x, _pos.y + 19, _width, 1, Blyat);
		

		int sum = 0;
		int x;
		ifstream inFile;

		string  path;
		static string username;
		static string rent;

		static bool GrabName = true;

		g_pSurface->AutoGradientRect(_pos.x + 1, _pos.y + 20, _width - 2, _height - 20, Cyka);
		g_pSurface->OutlinedRect(_pos.x + 1, _pos.y + 20, _width - 2, _height - 20, Blyat);

		


		g_pSurface->AutoGradientRect(_pos.x, _pos.y + 40, _width, 3, Cyka);
		g_pSurface->OutlinedRect(_pos.x, _pos.y + 40, _width, 3, Blyat);

		

		g_pSurface->OutlinedRect(_pos.x, _pos.y, _width, _height, Blyat);
		g_pSurface->OutlinedRect(_pos.x, _pos.y + 19, _width, 1, Blyat);


		std::string MenuWatermark;


		//RECT TitleText_Size = g_pSurface->GetTextSizeRect(Globals::CourierNew, "HOLAHOOKS INTERNAL");

		//g_pSurface->DrawT(_pos.x + 3, _pos.y + (20 / 2) - (TitleText_Size.bottom / 2), Color(143, 143, 143, 255), Globals::CourierNew, false, "                                            HOLAHOOKS");
		//g_pSurface->DrawT(_pos.x + 9, _pos.y + (340 - 18), Color(255, 255, 255, 255), Globals::CourierNew, false, __DATE__); // bottom text

		int in_sizew = _width - 1;

		static int tab_selected = 0;
		static const int tab_amount = 6;
		static std::string tab_names[tab_amount] = { "LEGITBOT", "RAGEBOT", "ANTIAIM", "VISUALS", "MISC", "CONFIGS" };

		for (int i = 0; i < tab_amount; i++)
		{
			RECT text_size = g_pSurface->GetTextSizeRect(Globals::TabFont, tab_names[i].c_str());

			int tab_area[4] = { _pos.x + 1 + (i * (in_sizew / tab_amount)), _pos.y + 20, (in_sizew / tab_amount), 20 };

			if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(tab_area[0], tab_area[1], tab_area[2],
				tab_area[3]))
				tab_selected = i;

			if (tab_selected == i)
			{
				g_pSurface->AutoGradientRect(tab_area[0], tab_area[1], tab_area[2], tab_area[3] /*+ 3*/, Color(70, 70, 70, 255));
				g_pSurface->OutlinedRect(tab_area[0], tab_area[1], tab_area[2], tab_area[3]/* + 3*/, Blyat);
				//g_pSurface->FilledRect(tab_area[0], tab_area[1] + tab_area[3], tab_area[2], 3, rainbow2);//
				g_pSurface->DrawT(tab_area[0] + (((in_sizew / tab_amount) / 2) - (text_size.right / 2)),
					tab_area[1] + (tab_area[3] / 2) - (text_size.bottom / 2), Color(255, 255, 255, 255),
					Globals::TabFont, false, tab_names[i].c_str());
				if (g_pSurface->MouseInRegion(tab_area[0], tab_area[1], tab_area[2], tab_area[3]))
				{
					g_pSurface->OutlinedRect(tab_area[0], tab_area[1], tab_area[2], tab_area[3]/* + 3*/, Maricon);
				}
			}
			else
			{
				g_pSurface->AutoGradientRect(tab_area[0], tab_area[1], tab_area[2], tab_area[3], Cyka);
				g_pSurface->OutlinedRect(tab_area[0], tab_area[1], tab_area[2], tab_area[3], Blyat);
				//g_pSurface->FilledRect(tab_area[0], tab_area[1] + tab_area[3], tab_area[2], 3, rainbow2);//

				g_pSurface->DrawT(tab_area[0] + (((in_sizew / tab_amount) / 2) - (text_size.right / 2)),
					tab_area[1] + (tab_area[3] / 2) - (text_size.bottom / 2), Color(143, 143, 143, 255),
					Globals::TabFont, false, tab_names[i].c_str());

				if (g_pSurface->MouseInRegion(tab_area[0], tab_area[1], tab_area[2], tab_area[3]))
				{
					g_pSurface->OutlinedRect(tab_area[0], tab_area[1], tab_area[2], tab_area[3]/* + 3*/, Maricon);
					g_pSurface->DrawT(tab_area[0] + (((in_sizew / tab_amount) / 2) - (text_size.right / 2)),
						tab_area[1] + (tab_area[3] / 2) - (text_size.bottom / 2), Color(255, 255, 255, 255),
						Globals::TabFont, false, tab_names[i].c_str());
					//g_pSurface->AutoGradientRect(tab_area[0], tab_area[1], tab_area[2], tab_area[3] /*+ 3*/, Color(70, 70, 70, 100));

				}
			}
		}
		std::string weapons[5] = { "AUTO", "SCOUT", "AWP", "HEAVY", "OTHER" };
		std::string pitch[8] = { "off", "down", "up", "zero", "minimal", "fake zero", "fake down", "fake up" };
		std::string pitch2[5] = { "Off", "Emotion", "Up", "Zero", "No Spread" };
		std::string EventLogsOptions[5] = { "Weapon Purchase", "Player Hurt", "Player Killed" };
		std::string yaw32131[8] = { "off", "back", "freestanding", "manual", "fake zero", "fake down", "fake up", "switched" };

		std::string yaw[8] = { "sideways", "backwards", "rotate", "jitter", "local view", "lowerbody", "auto direction", "180z" };
		std::string yaw2[3] = { "Off", "Backwards", "Auto-Direction" };
		std::string ANJTNIAM[4] = { "STANDING", "MOVING", "AIR" };
		std::string on_shot[2] = { "STABLE", "EXPERIMENTAL" };
		std::string desync[3] = { "DISABLE", "STABLE", "BETA" };
		std::string freestand_mode[3] = { "THREAT", "CROSSHAIR", "DISTANCE" };
		std::string Health_Type[4] = { "DISABLE", "BAR", "TEXT", "BOTH" };
		std::string Hitmarker_Type[3] = { "DISABLE", "ONE", "TWO" };
		std::string Resolver_Type[4] = { "DISABLE", "MAIN", "FATALITY", "TEST" };
		if (tab_selected == 0)
		{
			groupbox(5, 52, GroupBoxSize_Width, 287, "legitbot");
			checkbox("Enabled", &c_config::get().legit_aimbot_enabled);
			slider(180, "FOV", &c_config::get().legit_aimbot_fov, "", 1);
			slider(100, "SPEED", &c_config::get().linear_progression_threshold, "", 1);
			checkbox("RCS", &c_config::get().rcs);
			slider(100, "RCS X", &c_config::get().rcs_x, "", 1);
			slider(100, "RCS Y", &c_config::get().rcs_y, "", 1);
			checkbox("Backtrack", &c_config::get().legit_aimbot_backtrack);

			groupbox(230, 52, GroupBoxSize_Width, 287, "triggerbot");
			checkbox("triggerbot", &c_config::get().trigger_bot_enable);
			checkbox("RECOIL CONTROL", &c_config::get().trigger_bot_recoil);
			checkbox("friendly fire", &c_config::get().trigger_bot_deadmatch);
			checkbox("smokecheck", &c_config::get().triget_bot_smokecheck);


		}
		else if (tab_selected == 1) {
			groupbox(5, 52, GroupBoxSize_Width, 287, "main");
			checkbox("Enable", &c_config::get().aimbot_enabled);
			checkbox("AUTOSTOP", &c_config::get().autostop);
			checkbox("NO RECOIL", &c_config::get().aimbot_norecoil);
			checkbox("Accuracy Boost", &c_config::get().accuracy_boost);
			checkbox("Auto Scope", &c_config::get().auto_scope);

			combobox(4, "Resolver", Resolver_Type, &c_config::get().aimbot_resolver);


			groupbox(230, 52, GroupBoxSize_Width, 287, "weapons");
			combobox(5, "Weapons", weapons, &c_config::get().WeaponMode);
			if (c_config::get().WeaponMode == 0)
			{
				slider(100, "HITCHANCE", &c_config::get().auto_hitchance, "%", 1);
				slider(100, "MIN DAMAGE", &c_config::get().auto_mindamage, "HP", 1);
			}

			if (c_config::get().WeaponMode == 1)
			{
				slider(100, "HITCHANCE", &c_config::get().scout_hitchance, "%", 1);
				slider(100, "MIN DAMAGE", &c_config::get().scout_mindamage, "HP", 1);
			}

			if (c_config::get().WeaponMode == 2)
			{
				slider(100, "HITCHANCE", &c_config::get().awp_hitchance, "%", 1);
				slider(100, "MIN DAMAGE", &c_config::get().awp_mindamage, "HP", 1);
			}

			if (c_config::get().WeaponMode == 3)
			{
				slider(100, "HITCHANCE", &c_config::get().heavy_pistol_hitchance, "%", 1);
				slider(100, "MIN DAMAGE", &c_config::get().heavy_pistol_mindamage, "HP", 1);
			}

			if (c_config::get().WeaponMode == 4)
			{
				slider(100, "HITCHANCE", &c_config::get().other_hitchance, "%", 1);
				slider(100, "MIN DAMAGE", &c_config::get().other_mindamage, "HP", 1);
			}

			slider(100, "HEAD SCALE", &c_config::get().aimbot_headpointscale, "%", 1);
			slider(100, "BODY SCALE", &c_config::get().aimbot_bodypointscale, "%", 1);
			
		}

		else if (tab_selected == 2) {
			std::string pitch69420[7] = { "off", "emotion", "fake jitter", "fake zero", "spin", "up", "fake up" };
			std::string yaw69420[5] = { "off", "backward", "forward", "t-jitter", "spin" };

			groupbox(5, 52, GroupBoxSize_Width, 287, "antiaim");
			checkbox("ENABLE", &c_config::get().antiaim_enabled);
			checkbox("DESYNC", &c_config::get().DesyncAngle);
			combobox(7, "Pitch", pitch69420, &c_config::get().pitchAntiAim);
			combobox(5, "Yaw", yaw69420, &c_config::get().yawAntiAim);
			checkbox("freestanding", &c_config::get().Freestanding);
			checkbox("at-target", &c_config::get().AtTarget);
			checkbox("random jitter in-range", &c_config::get().RandJitterInRange);
			slider(360, "jitter range", &c_config::get().JitterRange, "t", 1);


			groupbox(230, 52, GroupBoxSize_Width, 287, "fakelag");
			checkbox("Fakelag", &c_config::get().fakelagwhileshooting);
			slider(14, "Fakelag", &c_config::get().fakelag, "t", 1);

			

		}

		else if (tab_selected == 3) {
			groupbox(5, 52, GroupBoxSize_Width, 287, "vis1");
			checkbox("ENABLE", &c_config::get().visuals_enabled);
			checkbox("Flags", &c_config::get().flags_enemy);

			checkbox("NAME", &c_config::get().name_enemies);
			checkbox("BOX", &c_config::get().box_enemies);
			checkbox("WEAPON", &c_config::get().weapon_enemies);
			checkbox("AMMO", &c_config::get().ammo_enemy);
			checkbox("INDICATOR", &c_config::get().fov_arrows_enemy);
			checkbox("GLOW", &c_config::get().glow_enemies);
			checkbox("CHAMS", &c_config::get().chams_enemies);
			checkbox("CHAMS XQZ", &c_config::get().chams_xqz_enemies);
			checkbox("LocalChams", &c_config::get().local_chams);
			combobox(2, "HEALTH", Health_Type, &c_config::get().health_enemies);


			groupbox(230, 52, GroupBoxSize_Width, 287, "vis2");
			checkbox("HIT MARKER", &c_config::get().hitmarker);
			combobox(3, "HIT MARKER SOUND", Hitmarker_Type, &c_config::get().hitmarker_sound);
			checkbox("GRENADE PREDICTION", &c_config::get().grenade_prediction);
			checkbox("REMOVE SCOPE", &c_config::get().remove_scope);
			checkbox("REMOVE SMOKE", &c_config::get().no_smoke);
			checkbox("REMOVE FLASH", &c_config::get().remove_flash);
			checkbox("REMOVE RECOIL", &c_config::get().remove_visual_recoil);
			checkbox("NO ZOOM", &c_config::get().fov_while_zoomed);
			checkbox("NIGHTMODE", &c_config::get().nightmode);
			checkbox("ASUS PROPS", &c_config::get().transparent_props);
			checkbox("BOMB TIMER", &c_config::get().bom_esp_timer);
			checkbox("ESP PREVIEW", &c_config::get().visual_preview);


			if (c_config::get().visual_preview)
			{
				draw_esp_preview(_pos.x + _width + 2, _pos.y);
			}

		}

		else if (tab_selected == 4) {
			groupbox(5, 52, GroupBoxSize_Width, 287, "misc");
			checkbox("BHOP", &c_config::get().misc_bhop);
			checkbox("AUTOSTRAFER", &c_config::get().misc_autostrafe);
			checkbox("CLANTAG", &c_config::get().misc_clantag);
			checkbox("Spectator List", &c_config::get().spectators);

			groupbox(230, 52, GroupBoxSize_Width, 287, "misc2");
			checkbox("ANTI UNTRUSTED", &c_config::get().misc_antiuntrusted);
			checkbox("SLOWWALK", &c_config::get().slowwalk);
			checkbox("KILLCOUNTER", &c_config::get().KillCounter);
			checkbox("watermark", &c_config::get().watermark);
			checkbox("indicators", &c_config::get().indicators);

		}

		else if (tab_selected == 5) {



			groupbox(5, 52, GroupBoxSize_Width, 287, "colors");
			combobox(8, "Colors", Colors_slots, &c_config::get().COLORS);
			if (c_config::get().COLORS == 0)
			{
				slider(255, "chams_r", &c_config::get().chams_esp_color_r, "%", 1);
				slider(255, "chams_g", &c_config::get().chams_esp_color_g, "%", 1);
				slider(255, "chams_b", &c_config::get().chams_esp_color_b, "%", 1);

			}
			else if (c_config::get().COLORS == 1)
			{
				slider(255, "chamsxqz_r", &c_config::get().chams_xqz_esp_color_r, "%", 1);
				slider(255, "chamsxqz_g", &c_config::get().chams_xqz_esp_color_g, "%", 1);
				slider(255, "chamsxqz_b", &c_config::get().chams_xqz_esp_color_b, "%", 1);
			}
			else if (c_config::get().COLORS == 2)
			{
				slider(255, "glow_r", &c_config::get().glow_esp_color_r, "%", 1);
				slider(255, "glow_g", &c_config::get().glow_esp_color_g, "%", 1);
				slider(255, "glow_b", &c_config::get().glow_esp_color_b, "%", 1);
				slider(255, "glow_a", &c_config::get().glow_esp_color_a, "%", 1);
			}
			else if (c_config::get().COLORS == 3)
			{
				slider(255, "name_color_r", &c_config::get().name_esp_color_r, "%", 1);
				slider(255, "name_color_g", &c_config::get().name_esp_color_g, "%", 1);
				slider(255, "name_color_b", &c_config::get().name_esp_color_b, "%", 1);
			}
			else if (c_config::get().COLORS == 4)
			{
				slider(255, "weapon_color_r", &c_config::get().weapon_esp_color_r, "%", 1);
				slider(255, "weapon_color_g", &c_config::get().weapon_esp_color_g, "%", 1);
				slider(255, "weapon_color_b", &c_config::get().weapon_esp_color_b, "%", 1);
			}
			else if (c_config::get().COLORS == 5)
			{
				slider(255, "box_color_r", &c_config::get().box_esp_color_r, "%", 1);
				slider(255, "box_color_g", &c_config::get().box_esp_color_g, "%", 1);
				slider(255, "box_color_b", &c_config::get().box_esp_color_b, "%", 1);
			}
			else if (c_config::get().COLORS == 6)
			{
				slider(255, "oof_color_r", &c_config::get().fov_arrows_esp_color_r, "%", 1);
				slider(255, "oof_color_g", &c_config::get().fov_arrows_esp_color_g, "%", 1);
				slider(255, "oof_color_b", &c_config::get().fov_arrows_esp_color_b, "%", 1);
				//slider(255, "oof_color_a", &c_config::get().fov_arrows_esp_color_a, "%", 1);

			}
			else if (c_config::get().COLORS == 7)
			{
				slider(255, "local_model_r", &c_config::get().local_chams_color_r, "%", 1);
				slider(255, "local_model_g", &c_config::get().local_chams_color_g, "%", 1);
				slider(255, "lcoal_model_b", &c_config::get().local_chams_color_b, "%", 1);
				slider(255, "local_model_a", &c_config::get().local_chams_color_a, "%", 1);

			}
			groupbox(230, 52, GroupBoxSize_Width, 287, "configs");

			combobox(5, "CONFIG", config_slots, &save_slot);
			button(Save_Config, "SAVE");
			button(Load_Config, "LOAD");
		}
		

		else if (tab_selected == 6) {
			groupbox(5, 52, 440, 287, "test");
			color_selector("name_col", &c_config::get().name_esp_color_r, &c_config::get().name_esp_color_g,
				&c_config::get().name_esp_color_b, &c_config::get().name_esp_color_a);
			EmptySpace("NAME COLOR");
			color_selector("box_col", &c_config::get().box_esp_color_r, &c_config::get().box_esp_color_g,
				&c_config::get().box_esp_color_b, &c_config::get().box_esp_color_a);
			EmptySpace("BOX COLOR");
			color_selector("weapon_col", &c_config::get().weapon_esp_color_r, &c_config::get().weapon_esp_color_g,
				&c_config::get().weapon_esp_color_b, &c_config::get().weapon_esp_color_a);
			EmptySpace("WEAPON COLOR");
			color_selector("player_indicator_col", &c_config::get().fov_arrows_esp_color_r,
				&c_config::get().fov_arrows_esp_color_g, &c_config::get().fov_arrows_esp_color_b,
				&c_config::get().fov_arrows_esp_color_a);
			EmptySpace("INDICATOR COLOR");
			color_selector("glow_col", &c_config::get().glow_esp_color_r, &c_config::get().glow_esp_color_g,
				&c_config::get().glow_esp_color_b, &c_config::get().glow_esp_color_a);
			EmptySpace("GLOW COLOR");
			color_selector("chams_col", &c_config::get().chams_esp_color_r, &c_config::get().chams_esp_color_g,
				&c_config::get().chams_esp_color_b, &c_config::get().chams_esp_color_a);
			EmptySpace("CHAMS COLOR");
			color_selector("chams_xqz_col", &c_config::get().chams_xqz_esp_color_r,
				&c_config::get().chams_xqz_esp_color_g, &c_config::get().chams_xqz_esp_color_b,
				&c_config::get().chams_xqz_esp_color_a);
			EmptySpace("CHAMS XQZ COLOR");

		}
	}
}

struct box_stuff {
	int left, top, right, bottom;

	box_stuff(int _l, int _t, int _r, int _b) {
		left = _l; top = _t; right = _r; bottom = _b;
	}
};


void Menu::draw_esp_preview(int x_pos, int y_pos)
{
	static int _width = 280;
	static int _height = 350;

	bool _click = false;

	if (GetAsyncKeyState(VK_LBUTTON))
		_click = true;

	Vector2D _mouse_pos = g_pSurface->GetMousePosition();

	_pos_esp_preview.x = x_pos;
	_pos_esp_preview.y = y_pos;

	g_pSurface->AutoGradientRect(_pos_esp_preview.x, _pos_esp_preview.y, _width, _height, Cyka);
	g_pSurface->OutlinedRect(_pos_esp_preview.x, _pos_esp_preview.y, _width, _height, Color(101, 101, 101, 255 / 2));
	g_pSurface->AutoGradientRect(_pos_esp_preview.x + 10, _pos_esp_preview.y + 20, _width - 20, _height - 30, Cyka);
	g_pSurface->OutlinedRect(_pos_esp_preview.x + 9, _pos_esp_preview.y + 19, _width - 18, _height - 28, Color(101, 101, 101, 255 / 2));

	g_pSurface->DrawT(_pos_esp_preview.x + (_width / 2), _pos_esp_preview.y + 1, Color(255, 255, 255), Globals::CourierNew, true, "PREVIEW");

	box_stuff box(_pos_esp_preview.x + (_width / 2) - 75, _pos_esp_preview.y + (_height / 2) - 120, 150, 250);

	// let's get to drawing shit.
	if (c_config::get().box_enemies) {
		g_pSurface->OutlinedRect(box.left, box.top, box.right, box.bottom, Color(c_config::get().box_esp_color_r, c_config::get().box_esp_color_g, c_config::get().box_esp_color_b));
		g_pSurface->OutlinedRect(box.left + 1, box.top + 1, box.right - 2, box.bottom - 2, Color(0, 0, 0));
		g_pSurface->OutlinedRect(box.left - 1, box.top - 1, box.right + 2, box.bottom + 2, Color(0, 0, 0));
	}

	if (c_config::get().name_enemies) {
			g_pSurface->DrawT(box.left + (box.right / 2), box.top - 15, Color(c_config::get().name_esp_color_r, c_config::get().name_esp_color_g, c_config::get().name_esp_color_b), Globals::WeaponESP, true, "Name");
	}

	if (c_config::get().weapon_enemies) {
		//g_pSurface->DrawT(box.left + (box.right / 2), box.top + box.bottom, Color(c_config::get().name_esp_color_r, c_config::get().name_esp_color_g, c_config::get().name_esp_color_b), Globals::CourierNew, true, "Weapon");
		g_pSurface->DrawT(box.left + (box.right / 2), box.top + box.bottom + 11, Color(c_config::get().name_esp_color_r, c_config::get().name_esp_color_g, c_config::get().name_esp_color_b), Globals::WeaponESP, true, "Weapon");
	}

	if (c_config::get().ammo_enemy)
	{
		static int pixelValue = 150;
		g_pSurface->FilledRect(box.left, box.top + box.bottom + 4, pixelValue, 3, Color(65, 105, 225, 220));
	}

	if (c_config::get().health_enemies)
	{
		static int pixelValue = 100 * box.bottom / 100;
		g_pSurface->FilledRect(box.left - 5 - 4, box.top + box.bottom - pixelValue, 5, pixelValue, Color(0, 255, 0, 220));
	}

	if (c_config::get().flags_enemy)
	{		
		g_pSurface->DrawT(box.left + (box.right + 15), box.top, Color(100, 255, 0), Globals::WeaponESP, true, "   $69420");
		g_pSurface->DrawT(box.left + (box.right + 15), box.top + 10, Color(255, 255, 255), Globals::WeaponESP, true, "   FLAGS");
	}
}

void Menu::groupbox(int x, int y, int w, int h, std::string name)
{
	g_pSurface->OutlinedRect(_pos.x + x, _pos.y + y, w, h, Color(20, 20, 20, 255 / 2));
	g_pSurface->OutlinedRect(_pos.x + (x + 1), _pos.y + (y + 1), w - 2, h - 2, Color(70, 70, 70, 255));

	RECT text_size = g_pSurface->GetTextSizeRect(Globals::SmallText, name.c_str());
	g_pSurface->FilledRect((((_pos.x + x) + (w / 2)) - (text_size.right / 2)) - 3, _pos.y + y, text_size.right + 6, 2, Color(44, 44, 44, 255));

	g_pSurface->DrawT((((_pos.x + x) + (w / 2)) - (text_size.right / 2)), (_pos.y + y - 6), Color(255, 255, 255, 255), Globals::SmallText, false, name.c_str());


	y_offset = (_pos.y + (y + 12));
	x_offset = _pos.x + x;
	groupbox_bottom = (_pos.y + (y + h));
	groupbox_top = (_pos.y + y);
	groupbox_width = (_pos.x + (x + w));
}

void Menu::ScrollableGroupBox(int x, int y, int w, int h)
{
	static bool pressed = false;
	static int add;
	if (GetAsyncKeyState(VK_DOWN) & 1)
	{
		add += 16;
	}

	if (GetAsyncKeyState(VK_UP) & 1)
	{
		add -= 16;
	}


	y_offset = y_offset + add;
}

void Menu::checkbox(std::string name, bool* item)
{
	if (groupbox_bottom <= y_offset + 16)
		return;

	if (groupbox_top >= y_offset + 16)
		return;

	int size = 12;

	static bool pressed = false;

	auto text_size = g_pSurface->GetTextSizeRect(Globals::CourierNew, name.c_str());

	if (g_Menu.menuOpened) {
		if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 7, y_offset, size + text_size.right, size))
		{
			if (pressed)
				* item = !*item;
			pressed = false;
		}

		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 7, y_offset, size + text_size.right, size) && !pressed)
			pressed = true;

	}

	if (*item == true)
		g_pSurface->AutoGradientRect(x_offset + 8, y_offset, size, size, Color(255, 255, 255, 255));
	else
		g_pSurface->AutoGradientRect(x_offset + 8, y_offset, size, size, Color(50, 50, 50, 255));

	g_pSurface->OutlinedRect(x_offset + 8, y_offset, size, size, Blyat);
	if (g_pSurface->MouseInRegion(x_offset + 8, y_offset, size, size))
	{
		g_pSurface->OutlinedRect(x_offset + 8, y_offset, size, size, Maricon);

	}

	g_pSurface->DrawT(x_offset + 29, y_offset - 2, Color(185, 185, 185, 255), Globals::CourierNew, false, name.c_str());

	y_offset += 16;
}

void Menu::combobox(int size, std::string name, std::string* itemname, int* item)
{
	if (groupbox_bottom <= y_offset + 41)
		return;

	bool pressed = false;
	bool open = false;
	static bool selected_opened = false;
	static bool click_rest;
	static bool rest;
	static std::string name_selected;

	int ComboSize = 100; // 163

	if (g_Menu.menuOpened) {
		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 25, y_offset + 14, ComboSize, 17) && !click_rest)
		{
			name_selected = name;
			pressed = true;
			click_rest = true;
		}
		else if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 25, y_offset + 14, ComboSize, 17))
			click_rest = false;

		if (pressed)
		{
			if (!rest)
				selected_opened = !selected_opened;
			rest = true;
		}
		else
			rest = false;

		if (name_selected == name)
			open = selected_opened;

	}

	g_pSurface->DrawT(x_offset + 29, y_offset - 2, Color(255, 255, 255, 255), Globals::CourierNew, false, name.c_str());
	g_pSurface->AutoGradientRect(x_offset + 29, y_offset + 14, ComboSize, 17, Color(50, 50, 50, 255));
	g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14, ComboSize, 17, Blyat);
	if (g_pSurface->MouseInRegion(x_offset + 29, y_offset + 14, ComboSize, 17))
	{
		g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14, ComboSize, 17, Maricon);

	}

	if (open)
	{
		g_pSurface->AutoGradientRect(x_offset + 29, y_offset + 14 + 21, ComboSize, (size * 16), Color(50, 50, 50, 255));
		g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14 + 21, ComboSize, (size * 16), Blyat);
		if (g_pSurface->MouseInRegion(x_offset + 29, y_offset + 14 + 21, ComboSize, (size * 16)))
		{
			g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14 + 21, ComboSize, (size * 16), Maricon);

		}

		for (int i = 0; i < size; i++)
		{
			if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 29, y_offset + 34 + i * 16, ComboSize, 16))
				* item = i;

			if (i == *item)
				g_pSurface->DrawT(x_offset + 29 + 10, y_offset + 35 + i * 16, Color(185, 185, 185, 255), Globals::CourierNew, false, itemname[i].c_str());
			else
				g_pSurface->DrawT(x_offset + 29 + 10, y_offset + 35 + i * 16, Color(155, 155, 155, 255), Globals::CourierNew, false, itemname[i].c_str());
		}
	}

	g_pSurface->Line(x_offset + 29 + ComboSize - 14, y_offset + 13 + 7, x_offset + 29 + ComboSize - 7, y_offset + 13 + 7, Blyat);
	g_pSurface->Line(x_offset + 29 + ComboSize - 14, y_offset + 13 + 9, x_offset + 29 + ComboSize - 7, y_offset + 13 + 9, Blyat);
	g_pSurface->Line(x_offset + 29 + ComboSize - 14, y_offset + 13 + 11, x_offset + 29 + ComboSize - 7, y_offset + 13 + 11, Blyat);

	g_pSurface->DrawT(x_offset + 38, y_offset + 15, Color(185, 185, 185, 255), Globals::CourierNew, false, itemname[*item].c_str());

	if (open) // i added 1 more to 42 bec the space between the main box and the drop down
		y_offset += 42 + (size * 16);
	else
		y_offset += 41;
}

void Menu::slider(int max, std::string name, int* item, std::string suffix, int step)
{
	if (groupbox_bottom <= y_offset + 30)
		return;

	float SliderSize = 100; // 163

	float _pixel_value = max / SliderSize;

	if (g_Menu.menuOpened) {
		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 29, y_offset + 14, SliderSize + 1, 10))
			* item = abs(g_pSurface->GetMousePosition().x - (x_offset + 29)) * _pixel_value;


		static bool pressed_subtract = false;
		static bool pressed_add = false;

		/*
		subtract
		*/

		if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 21, y_offset + 12, 8, 10))
		{
			if (pressed_subtract)
				* item -= step;
			pressed_subtract = false;
		}

		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 21, y_offset + 12, 8, 10) && !pressed_subtract)
			pressed_subtract = true;

		/*
		add
		*/

		if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 22 + SliderSize, y_offset + 12, 15, 10))
		{
			if (pressed_add)
				* item += step;
			pressed_add = false;
		}

		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 22 + SliderSize, y_offset + 12, 15, 10) && !pressed_add)
			pressed_add = true;

		if (*item > max) {
			*item = max;
		}

		if (*item < 0) {
			*item = 0;
		}

	}

	std::string namedisplay;

	namedisplay += name.c_str();

	namedisplay += " (";

	namedisplay += std::to_string(*item);

	namedisplay += ")";

	g_pSurface->DrawT(x_offset + 29, y_offset - 3, Color(185, 185, 185, 255), Globals::CourierNew, false, namedisplay.c_str());

	g_pSurface->AutoGradientRect(x_offset + 29, y_offset + 14, SliderSize, 10, Color(50, 50, 50, 255));

	if (*item)
	{
		if (*item == max) //to fix dumb math stufffff bec pixel size
			g_pSurface->AutoGradientRect(x_offset + 29, y_offset + 14, SliderSize, 10, Color(255, 255, 255, 255));
		else
			g_pSurface->AutoGradientRect(x_offset + 29, y_offset + 14, (*item / _pixel_value), 10, Color(255, 255, 255, 255));
	}

	g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14, SliderSize, 10, Blyat);
	if (g_pSurface->MouseInRegion(x_offset + 29, y_offset + 14, SliderSize, 10))
	{
		g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14, SliderSize, 10, Maricon);

	}

	g_pSurface->DrawT(x_offset + 22, y_offset + 10, Color(185, 185, 185, 255), Globals::CourierNew, false, "-");
	g_pSurface->DrawT(x_offset + 20 + SliderSize + 12, y_offset + 11, Color(185, 185, 185, 255), Globals::CourierNew, false, "+");

	y_offset += 30;
}

void Menu::MultiComboBox(float indexes, std::string name, std::string* itemname, bool* item)
{
	static bool multiPressed = false;
	bool pressed = false;
	bool open = false;
	static bool selectedOpened = false;
	static bool clickRest;
	static bool rest;
	static std::string nameSelected;
	std::string itemsSelected = "";
	int lastItem = 0;
	int ComboSize = 100;

	int offset = 0;
	if (name == "")
	{
		offset = 0;
	}
	else
	{
		offset = 14;
	}

	if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 20, y_offset + offset, ComboSize, 17) && !clickRest
		)
	{
		nameSelected = name;
		pressed = true;
		clickRest = true;
	}
	else if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 20, y_offset + offset, ComboSize, 20))
		clickRest = false;

	if (pressed)
	{
		if (!rest)
			selectedOpened = !selectedOpened;

		rest = true;
	}
	else
		rest = false;

	if (nameSelected == name)
		open = selectedOpened;

	g_pSurface->DrawT(x_offset + 38, y_offset + 15, Color(143, 143, 143, 255), Globals::CourierNew, false,
		name.c_str());

	g_pSurface->AutoGradientRect(x_offset + 29, y_offset + 14, ComboSize, 17, Color(50, 50, 50, 255));
	g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14, ComboSize, 17, Maricon);

	if (open)
	{
		g_pSurface->AutoGradientRect(x_offset + 29, y_offset + 14, ComboSize, 17,
			Color(45, 46, 53, 255));
		g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14, ComboSize, 17,
			Maricon);

		for (int i = 0; i < indexes; i++)
		{
			if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(
				x_offset + 20, y_offset + offset + 4 + 4 + 15 + i * 19, ComboSize, 20))
			{
				if (multiPressed)
					item[i] = !item[i];
				multiPressed = false;
			}

			if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(
				x_offset + 20, y_offset + offset + 16 + (i * 19), ComboSize, 20) && !multiPressed)
				multiPressed = true;

			if (item[i])
				g_pSurface->DrawT(x_offset + 29 + 10, y_offset + 35 + i * 16, Color(255, 255, 255, 255),
					Globals::CourierNew, false, itemname[i].c_str());
			else
				g_pSurface->DrawT(x_offset + 29 + 10, y_offset + 35 + i * 16, Color(185, 185, 185, 255),
					Globals::CourierNew, false, itemname[i].c_str());
		}
	}

	bool items = false;

	for (int i = 0; i < indexes; i++)
	{
		if (item[i])
		{
			if (lastItem < i)
				lastItem = i;
		}
	}

	for (int i = 0; i < indexes; i++)
	{
		if (item[i])
		{
			items = true;
			RECT TextSize = g_pSurface->GetTextSizeRect(Globals::CourierNew, itemsSelected.c_str());
			RECT TextSizeGonaAdd = g_pSurface->GetTextSizeRect(Globals::CourierNew, itemname[i].c_str());
			if (TextSize.right + TextSizeGonaAdd.right < ComboSize)
				itemsSelected += std::to_string(i) + ((lastItem == i) ? "" : ", ");
		}
	}

	if (!items)
		itemsSelected = "Disabled";

	g_pSurface->DrawT(x_offset + 38, y_offset + 15, Color(143, 143, 143, 255), Globals::CourierNew, false,
		itemsSelected.c_str());

	g_pSurface->Line(x_offset + 29 + ComboSize - 14, y_offset + 13 + 7, x_offset + 29 + ComboSize - 7, y_offset + 13 + 7, Blyat);
	g_pSurface->Line(x_offset + 29 + ComboSize - 14, y_offset + 13 + 9, x_offset + 29 + ComboSize - 7, y_offset + 13 + 9, Blyat);
	g_pSurface->Line(x_offset + 29 + ComboSize - 14, y_offset + 13 + 11, x_offset + 29 + ComboSize - 7, y_offset + 13 + 11, Blyat);


	if (open) // i added 1 more to 42 bec the space between the main box and the drop down
		y_offset += 52 + (indexes * 16);
	else
		y_offset += 41;
}

char* KeyStringsStick[254] = {
	"INVLD", "M1", "M2", "BRK", "M3", "M4", "M5",
	"INVLD", "BSPC", "TAB", "INVLD", "INVLD", "INVLD", "ENTER", "INVLD", "INVLD", "SHI",
	"CTRL", "ALT", "PAU", "CAPS", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"ESC", "INVLD", "INVLD", "INVLD", "INVLD", "SPACE", "PGUP", "PGDOWN", "END", "HOME", "LEFT",
	"UP", "RIGHT", "DOWN", "INVLD", "PRNT", "INVLD", "PRTSCR", "INS", "DEL", "INVLD", "0", "1",
	"2", "3", "4", "5", "6", "7", "8", "9", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U",
	"V", "W", "X", "Y", "Z", "LFTWIN", "RGHTWIN", "INVLD", "INVLD", "INVLD", "NUM0", "NUM1",
	"NUM2", "NUM3", "NUM4", "NUM5", "NUM6", "NUM7", "NUM8", "NUM9", "*", "+", "_", "-", ".", "/", "F1", "F2", "F3",
	"F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20",
	"F21",
	"F22", "F23", "F24", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"NUM LOCK", "SCROLL LOCK", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "LSHFT", "RSHFT", "LCTRL",
	"RCTRL", "LMENU", "RMENU", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "NTRK", "PTRK", "STOP", "PLAY", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", ";", "+", ",", "-", ".", "/?", "~", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "{", "\\|", "}", "'\"", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD"
};

bool keys[256];
bool oldKeys[256];

bool GetKeyPress(unsigned int key)
{
	if (keys[key] == true && oldKeys[key] == false)
		return true;
	return false;
}

unsigned int rgb(double hue)
{
	int h = int(hue * 256 * 6);
	int x = h % 0x100;

	int r = 0, g = 0, b = 0;
	switch (h / 256)
	{
	case 0: r = 255;
		g = x;
		break;
	case 1: g = 255;
		r = 255 - x;
		break;
	case 2: g = 255;
		b = x;
		break;
	case 3: b = 255;
		g = 255 - x;
		break;
	case 4: b = 255;
		r = x;
		break;
	case 5: r = 255;
		b = 255 - x;
		break;
	}

	return r + (g << 8) + (b << 16);
}

void color_spectrum(int x, int y)
{
	int Width = 168, Height = 179;

	for (int i = 0; i < Width; i++)
	{
		int div = Width / 6;
		int phase = i / div;
		float t = (i % div) / (float)div;
		int r, g, b;

		switch (phase)
		{
		case(0):
			r = 255;
			g = 255 * t;
			b = 0;
			break;
		case(1):
			r = 255 * (1.f - t);
			g = 255;
			b = 0;
			break;
		case(2):
			r = 0;
			g = 255;
			b = 255 * t;
			break;
		case(3):
			r = 0;
			g = 255 * (1.f - t);
			b = 255;
			break;
		case(4):
			r = 255 * t;
			g = 0;
			b = 255;
			break;
		case(5):
			r = 255;
			g = 0;
			b = 255 * (1.f - t);
			break;
		}

		for (int k = 0; k < Height; k++)
		{
			float sat = k / (float)Height;
			int _r = r + sat * (128 - r);
			int _g = g + sat * (128 - g);
			int _b = b + sat * (128 - b);
			g_pSurface->DrawSetColor(_r, _g, _b, 255);
			g_pSurface->DrawFilledRect(10 + i, 10 + k, 1, 1);
		}
	}
}

#include <conio.h>
#include <iostream>
#include <windows.h>
#include <stdlib.h>

void Menu::EmptySpace(std::string name)
{
	if (groupbox_bottom <= y_offset + 16)
		return;

	g_pSurface->DrawT(x_offset + 20, y_offset - 3, Color(143, 143, 143, 255), Globals::CourierNew, false,
		name.c_str());

	y_offset += 16;
}

void Menu::color_selector(std::string name, int* red, int* green, int* blue, int* alpha)
{
	if (groupbox_bottom <= y_offset + 16)
		return;

	bool pressed = false;
	bool open = false;
	static bool selected_opened = false;
	static bool click_rest;
	static bool rest;
	static std::string name_selected;

	int ComboSize = 127; // 163
	int ComboSizeH = 127; // 163
	static int offset = 0;
	if (name == "sadasd")
	{
		offset = 0;
	}
	else
	{
		offset = 14;
	}

	if (menuOpened)
	{
		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 225, y_offset + 1, 15, 8) && !
			click_rest)
		{
			name_selected = name;
			pressed = true;
			click_rest = true;
		}
		else if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 225, y_offset + 1, 15, 8))
			click_rest = false;

		if (pressed)
		{
			if (!rest)
				selected_opened = !selected_opened;
			rest = true;
		}
		else
			rest = false;

		if (name_selected == name)
			open = selected_opened;
	}


	static unsigned int r = 255;
	static unsigned int g = 0;
	static unsigned int b = 0;


	static unsigned int selected_r = 0;
	static unsigned int selected_g = 0;
	static unsigned int selected_b = 0;

	static Vector2D MousePos;
	COLORREF color;
	COLORREF selected_color;

	if (open)
	{
		dont_recieve_input = true;
		g_pSurface->FilledRect(x_offset + 225 - ComboSize - 5, y_offset, ComboSize, ComboSizeH,
			Color(21, 23, 27, 255));

		g_pSurface->OutlinedRect(x_offset + 225 - ComboSize - 5, y_offset, ComboSize, ComboSizeH,
			Color(42, 46, 46, 255));
		g_pSurface->OutlinedRect(x_offset + 225 - ComboSize - 5 - 1, y_offset - 1, ComboSize + 2, ComboSizeH + 2,
			Color(34, 38, 37, 255));

		float _pixel_value = 255 / (ComboSize - 5 - 17);

		//if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 20 - 3, y_offset + 14, SliderSize + (3 * 2), 8)) {
		//	*item = abs(g_pSurface->GetMousePosition().x - (x_offset + 20)) * _pixel_value;
		//}

		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 225 - ComboSize - 5 + 5,
			y_offset + ComboSizeH - 11, (ComboSize - 5 - 17),
			7) && menuOpened)
		{
			*alpha = abs(g_pSurface->GetMousePosition().x - (x_offset + 225 - ComboSize - 5 + 5)) * _pixel_value;
		}


		if (*alpha < 3)
		{
			*alpha = 0;
		}

		if (*alpha > 204)
		{
			*alpha = 255;
		}

		g_pSurface->FilledRect(x_offset + 225 - ComboSize - 5 + 5, y_offset + ComboSizeH - 11, ComboSize - 5 - 17, 7,
			Color(255, 255, 255, *alpha));
		g_pSurface->OutlinedRect(x_offset + 225 - ComboSize - 5 + 5, y_offset + ComboSizeH - 11, ComboSize - 5 - 17, 7,
			Maricon);


		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 233 - ComboSize + 101 + 1,
			y_offset + 5 + 1, 7 - 2, 107 - 1))
		{
			HDC hdc = GetDC(nullptr);
			color = GetPixel(hdc, g_pSurface->GetMousePosition().x, g_pSurface->GetMousePosition().y);

			r = GetRValue(color);
			g = GetGValue(color);
			b = GetBValue(color);
		}


		GradientH(x_offset + 225 - ComboSize - 5 + 5, y_offset + 5, ComboSize - 5 - 17, ComboSizeH - 20,
			Color(255, 255, 255), Color(r, g, b));
		GradientVEnd(x_offset + 225 - ComboSize - 5 + 5, y_offset + 40, ComboSize - 5 - 17, ComboSizeH - 20,
			Color(0, 0, 0), Color(0, 0, 0));


		//g_pSurface->OutlinedRect(MousePos.x - 4, MousePos.y - 4, 8, 8, Maricon);
		g_pSurface->OutlinedRect(x_offset + 225 - ComboSize - 5 + 5, y_offset + 5, ComboSize - 5 - 17, ComboSizeH - 20,
			Maricon);

		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 225 - ComboSize - 5 + 5 + 1,
			y_offset + 5 + 1, ComboSize - 5 - 17 - 2,
			ComboSizeH - 20 - 2))
		{
			HDC hdc = GetDC(nullptr);
			selected_color = GetPixel(hdc, g_pSurface->GetMousePosition().x, g_pSurface->GetMousePosition().y);

			selected_r = GetRValue(selected_color);
			selected_g = GetGValue(selected_color);
			selected_b = GetBValue(selected_color);

			*red = selected_r;
			*green = selected_g;
			*blue = selected_b;

			MousePos = Vector2D(g_pSurface->GetMousePosition().x, g_pSurface->GetMousePosition().y);
		}

		Color rainbow; int hue;
		for (int i = 0; i < 100; i++)
		{
			float hue = (i * .01f);
			rainbow.FromHSV(hue, 1.f, 1.f);
			g_pSurface->FilledRect(x_offset + 233 - ComboSize + 101, y_offset + 5 + i, 7, 8, rainbow);
			g_pSurface->OutlinedRect(x_offset + 233 - ComboSize + 101, y_offset + 5, 7, 107, Maricon);
		}
	}
	else
	{
		dont_recieve_input = false;
	}


	if (*red > 20 && *green > 20 && *blue > 20)
	{
		g_pSurface->FilledRect(x_offset + 225, y_offset, 15, 7, Color(*red, *green, *blue, 255));
	}
	else
	{
		g_pSurface->FilledRect(x_offset + 225, y_offset, 15, 7, Color(*red, *green, *blue, 255));
	}


	g_pSurface->OutlinedRect(x_offset + 225 - 1, y_offset - 1, 15 + 2, 7 + 2, Maricon);

	if (open)
	{
		y_offset += ComboSizeH;
	}

	/*
	Color rainbow;

	Color Test2;

	static float test;

	static DWORD color;
	static DWORD store_color;
	if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 225 - 93, y_offset + 1, 108, 8)) {


	}
	else {
		unsigned int r = GetRValue(color);
		unsigned int g = GetGValue(color);
		unsigned int b = GetBValue(color);

		*red = r;
		*green = g;
		*blue = b;

		GradientV(x_offset + 225, y_offset, 16, 9, Color(*red, *green, *blue), Color(*red, *green, *blue));
		g_pSurface->OutlinedRect(x_offset + 225, y_offset, 16, 9, Maricon);
	}
	*/
}

char* KeyDigits[254] = {
	nullptr, "Left Mouse", "Right Mouse", "Control+Break", "Middle Mouse", "Mouse 4", "Mouse 5",
	nullptr, "Backspace", "TAB", nullptr, nullptr, nullptr, "ENTER", nullptr, nullptr, "SHIFT", "CTRL", "ALT", "PAUSE",
	"CAPS LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "ESC", nullptr, nullptr, nullptr, nullptr,
	"SPACEBAR",
	"PG UP", "PG DOWN", "END", "HOME", "Left", "Up", "Right", "Down", nullptr, "Print", nullptr, "Print Screen",
	"Insert",
	"Delete", nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr,
	nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U",
	"V", "W", "X",
	"Y", "Z", "Left Windows", "Right Windows", nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
	"7", "8", "9", "*", "+", "_", "-", ".", "/", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11",
	"F12",
	"F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24", nullptr, nullptr, nullptr,
	nullptr, nullptr,
	nullptr, nullptr, nullptr, "NUM LOCK", "SCROLL LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, "LSHIFT", "RSHIFT", "LCONTROL", "RCONTROL", "LMENU", "RMENU", nullptr,
	nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "Next Track", "Previous Track", "Stop", "Play/Pause",
	nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[{", "\\|", "}]", "'\"", nullptr, nullptr, nullptr,
	nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};

void Menu::TextBox(std::string name, std::string* text)
{
	if (groupbox_bottom <= y_offset + 16)
		return;

	bool pressed = false;
	bool open = false;
	static bool selected_opened = false;
	static bool click_rest;
	static bool rest;
	static std::string name_selected;

	int ComboSize = 127; // 163
	int ComboSizeH = 127; // 163
	static int offset = 0;
	if (name == "")
	{
		offset = 0;
	}
	else
	{
		offset = 14;
	}

	std::copy(keys, keys + 255, oldKeys);
	for (int x = 0; x < 255; x++)
	{
		//oldKeys[x] = oldKeys[x] & keys[x];
		keys[x] = (GetAsyncKeyState(x));
	}


	if (menuOpened)
	{
		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 225, y_offset + 1, 15, 8) && !
			click_rest)
		{
			name_selected = name;
			pressed = true;
			click_rest = true;
		}
		else if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 225, y_offset + 1, 15, 8))
			click_rest = false;

		if (pressed)
		{
			if (!rest)
				selected_opened = !selected_opened;
			rest = true;
		}
		else
			rest = false;

		if (name_selected == name)
			open = selected_opened;
	}

	const char* strg = text->c_str();
	if (open)
	{
		for (int i = 0; i < 255; i++)
		{
			if (GetKeyPress(i))
			{
				if (i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT)
				{
					open = false;
					return;
				}

				if (i == VK_BACK && strlen(strg) != 0)
				{
					*text = text->substr(0, strlen(strg) - 1);
				}

				if (strlen(strg) < 6 && (i == 0x30 || i == 0x31 || i == 0x32 || i == 0x33 || i == 0x34 || i == 0x35 || i
					== 0x36 || i == 0x37 || i == 0x38 || i == 0x39 || i == VK_NUMPAD0 || i == VK_NUMPAD1 || i ==
					VK_NUMPAD2 || i == VK_NUMPAD3 || i == VK_NUMPAD4 || i == VK_NUMPAD5 || i == VK_NUMPAD6 || i ==
					VK_NUMPAD7 || i == VK_NUMPAD8 || i == VK_NUMPAD9))
				{
					*text = *text + KeyDigits[i];
					return;
				}
			}
		}
	}

	const char* cstr = text->c_str();
	g_pSurface->AutoGradientRect(x_offset + 29, y_offset + 14, ComboSize, 17, Color(45, 46, 53, 255));
	g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14, ComboSize, 17, Maricon);
	g_pSurface->DrawT(x_offset + 29, y_offset - 2, Color(143, 143, 143, 255), Globals::CourierNew, false,
		cstr);

	y_offset += 26;
}

void Menu::button(ButtonCallback_t callback, std::string title)
{
	if (groupbox_bottom <= y_offset + 41)
		return;

	int ComboSize = 100;

	static bool pressed = false;

	RECT text_size = g_pSurface->GetTextSizeRect(Globals::CourierNew, title.c_str());

	if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 25, y_offset + 14, ComboSize, 17) && menuOpened)
	{
		if (pressed)
		{
			if (callback)
				callback();
		}
		pressed = false;
	}

	if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(x_offset + 25, y_offset + 14, ComboSize, 17) && !pressed)
		pressed = true;

	if (g_pSurface->MouseInRegion(x_offset + 25, y_offset + 14, ComboSize, 17) and GetAsyncKeyState(VK_LBUTTON))
	{
		g_pSurface->AutoGradientRect(x_offset + 29, y_offset + 14, ComboSize, 17, Color(25, 25, 25, 255));
		g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14, ComboSize, 17, Blyat);

	}
	else
	{
		g_pSurface->AutoGradientRect(x_offset + 29, y_offset + 14, ComboSize, 17, Color(50, 50, 50, 255));
		g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14, ComboSize, 17, Blyat);

	}

	if (g_pSurface->MouseInRegion(x_offset + 29, y_offset + 14, ComboSize, 17))
	{
		g_pSurface->OutlinedRect(x_offset + 29, y_offset + 14, ComboSize, 17, Maricon);
	}

	g_pSurface->DrawT(x_offset + 38, y_offset + 15, Color(204, 204, 204, 255),
		Globals::CourierNew, false, title.c_str());
	y_offset += 30;
}


void Menu::ListBox(int x, int y, int w, int h, int size, std::string name, std::string* itemname, int* item) {
	g_pSurface->AutoGradientRect(_pos.x + x, _pos.y + y, w, h, Color(25, 26, 33, 255));
	g_pSurface->OutlinedRect(_pos.x + x, _pos.y + y, w, h, Color(40, 44, 53, 255));

	g_pSurface->AutoGradientRect(_pos.x + x + 1, _pos.y + y + 1, w - 2, 15, Color(31, 31, 38, 255));
	g_pSurface->Line(_pos.x + x + 1, _pos.y + y + 15, _pos.x + x + w - 2, _pos.y + y + 15, Color(40, 44, 53, 255));

	g_pSurface->DrawT(_pos.x + x + (w / 2), _pos.y + y + 1, Color(143, 143, 143, 255), Globals::CourierNew, true, name.c_str());

	for (int i = 0; i < size; i++) {
		RECT Area = { _pos.x + x, _pos.y + y + 17 + (16 * i), w, 15 };

		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(Area.left, Area.top, Area.right, Area.bottom) && menuOpened) {
			*item = i;
		}

		if (*item == i) {
			g_pSurface->AutoGradientRect(Area.left, Area.top, Area.right, Area.bottom, Color(28, 28, 35, 255));
		}
		else {
			g_pSurface->AutoGradientRect(Area.left, Area.top, Area.right, Area.bottom, Color(21, 21, 28, 255));
		}

		g_pSurface->OutlinedRect(Area.left, Area.top, Area.right, Area.bottom, Color(40, 44, 53, 255));

		g_pSurface->DrawT(Area.left + (Area.right / 2), Area.top + 1, Color(143, 143, 143, 255), Globals::CourierNew, true, itemname[i].c_str());
	}
}
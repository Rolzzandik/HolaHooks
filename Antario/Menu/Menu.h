#pragma once
#include <Windows.h>
#include <string>
#include <filesystem>
#include "../SDK/Vector.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
struct ColorV2
{
	float red, green, blue, alpha;
};

class Menu
{
public:
	bool menuOpened;
	void spectators();
	void Render();
	void spectators_list();
	int PlayerList_Player[1024];
	void DoPlayerList(int x, int y, int w, int h);

	//void draw_radar();

	void draw_esp_preview(int x_pos, int y_pos);

	//void groupbox(int x, int y, int w, int h, std::string name, bool dont_draw);

	void groupbox(int x, int y, int w, int h, std::string name);


	//void groupbox(int x, int y, int w, int h, std::string name, bool dont_draw = false, bool has_subtabs = false);

	void Drawgroupboxend(int x, int y, int w, int h);

	void ScrollableGroupBox(int x, int y, int w, int h);
	struct
	{
		bool Aimbot;
		int Hitchance;
		int HitchanceValue;
		int Mindmg;
		bool Resolver;
		int BodyScale;
		int HeadScale;
		bool MultiPoint;
		bool DelayShot;
		bool IgnoreLimbs;
		bool Autostop;
		bool OneTickChoke;
		bool BaimLethal;
		bool BaimPitch;
		bool BaimInAir;


		bool IsThirdperson;


		int pitch[3];
		int yaw[3];
		float yawoffset[3];

		bool Antiaim;
		bool DesyncAngle;
		bool RandJitterInRange;
		int	JitterRange;
		int	Fakelag;
		bool FakeLagOnPeek;
		bool ChokeShotOnPeek;

		bool Esp;
		int Name;
		int HealthVal;
		int Weapon;
		bool Box;
		bool HealthBar;
		bool HitboxPoints;
		bool NoZoom;
		int Fov;
		bool Crosshair;

		bool Bhop;
		bool AutoStrafe;
		bool LegitBacktrack;
		bool Ak47meme;
		bool RemoveScope;
		int	Test;
	}Config;

private:
	Vector _pos = Vector(500, 200, 0);
	Vector _pos_radar = Vector(0, 0, 0);
	Vector2D _pos_esp_preview = Vector2D(960.f, 150.f);
	int ControlsX;
	int GroupTabBottom;
	int OffsetY;
	int screen_width;
	int screen_height;
	int y_offset;
	int x_offset;
	//int MenuAlpha_Main;
	//int MenuAlpha_Text;
	int groupbox_scroll_add;
	int groupbox_width;
	int groupbox_bottom;
	int groupbox_top;
	bool we_are_clipping;
	int how_many_controls;
	typedef void(*ButtonCallback_t)(void);
	void checkbox(std::string name, bool* item);
	void combobox(int size, std::string name, std::string *itemname, int* item);
	void EmptySpace(std::string name);
	void color_selector(std::string name, int* red, int* blue, int* green, int* alpha);
	void TextBox(std::string name, std::string *text);
	void button(ButtonCallback_t callback, std::string title);
	void ListBox(int x, int y, int w, int h, int size, std::string name, std::string *itemname, int* item);
	void slider(int max, std::string name, int * item, std::string suffix, int step);
	void MultiComboBox(float indexes, std::string name, std::string * itemname, bool * item);
	void keybind(int * item, std::string name);
	void ColorPicker(std::string name, ColorV2& item);
};



extern Menu g_Menu;
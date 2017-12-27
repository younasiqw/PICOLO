#include "../Cheat.h"
#include "Menu.h"
#include "../Parser.h"
#include "../SkinChanger.h"
#include "../ImGUI/imgui_internal.h"
#include "../ImGUI/imgui.h"
#include <locale>

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

IDirect3DTexture9 *tImage = nullptr;
void GUI_Init(IDirect3DDevice9* pDevice);
float penis;
int Animatetype;
int sleepanimate;
int sleepanimate2;
int pizda = 0;
int periodicity = 1.f;
int menustyle = 1;

float periodicity1 = 0.01f;
int periodicity2 = 1.f;
float R3animate = 0.0f;
float connva = 0.01f;
int buttonmenu;
int width = 0;
int height = 0;
int width1 = 0;
int height2 = 0;
int width2 = 0;
int height3 = 0;
namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	IMGUI_API bool ComboBoxArray(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	IMGUI_API bool TabLabels(const char **tabLabels, int tabSize, int &tabIndex, int *tabOrder)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		const ImVec2 itemSpacing = style.ItemSpacing;
		const ImVec4 color = style.Colors[ImGuiCol_Button];
		const ImVec4 colorActive = style.Colors[ImGuiCol_ButtonActive];
		const ImVec4 colorHover = style.Colors[ImGuiCol_ButtonHovered];
		const ImVec4 colorText = style.Colors[ImGuiCol_Text];
		style.ItemSpacing.x = 1;
		style.ItemSpacing.y = 1;
		const ImVec4 colorSelectedTab = ImVec4(color.x, color.y, color.z, color.w*0.5f);
		const ImVec4 colorSelectedTabHovered = ImVec4(colorHover.x, colorHover.y, colorHover.z, colorHover.w*0.5f);
		const ImVec4 colorSelectedTabText = ImVec4(colorText.x*0.8f, colorText.y*0.8f, colorText.z*0.8f, colorText.w*0.8f);

		if (tabSize>0 && (tabIndex<0 || tabIndex >= tabSize))
		{
			if (!tabOrder)
			{
				tabIndex = 0;
			}
			else
			{
				tabIndex = -1;
			}
		}

		float windowWidth = 0.f, sumX = 0.f;
		windowWidth = ImGui::GetWindowWidth() - style.WindowPadding.x - (ImGui::GetScrollMaxY()>0 ? style.ScrollbarSize : 0.f);

		static int draggingTabIndex = -1; int draggingTabTargetIndex = -1;
		static ImVec2 draggingtabSize(0, 0);
		static ImVec2 draggingTabOffset(0, 0);

		const bool isMMBreleased = ImGui::IsMouseReleased(2);
		const bool isMouseDragging = ImGui::IsMouseDragging(0, 2.f);
		int justClosedTabIndex = -1, newtabIndex = tabIndex;

		bool selection_changed = false; bool noButtonDrawn = true;

		for (int j = 0, i; j < tabSize; j++)
		{
			i = tabOrder ? tabOrder[j] : j;
			if (i == -1) continue;

			if (sumX > 0.f)
			{
				sumX += style.ItemSpacing.x;
				sumX += ImGui::CalcTextSize(tabLabels[i]).x + 2.f*style.FramePadding.x;

				if (sumX>windowWidth)
				{
					sumX = 0.f;
				}
				else
				{
					ImGui::SameLine();
				}
			}

			if (i != tabIndex)
			{
				// Push the style
				style.Colors[ImGuiCol_Button] = colorSelectedTab;
				style.Colors[ImGuiCol_ButtonActive] = colorSelectedTab;
				style.Colors[ImGuiCol_ButtonHovered] = colorSelectedTabHovered;
				style.Colors[ImGuiCol_Text] = colorSelectedTabText;
			}
			// Draw the button
			ImGui::PushID(i);   // otherwise two tabs with the same name would clash.
			if (ImGui::Button(tabLabels[i], ImVec2(160.f, 25.f))) { selection_changed = (tabIndex != i); newtabIndex = i; }
			ImGui::PopID();
			if (i != tabIndex)
			{
				// Reset the style
				style.Colors[ImGuiCol_Button] = color;
				style.Colors[ImGuiCol_ButtonActive] = colorActive;
				style.Colors[ImGuiCol_ButtonHovered] = colorHover;
				style.Colors[ImGuiCol_Text] = colorText;
			}
			noButtonDrawn = false;

			if (sumX == 0.f) sumX = style.WindowPadding.x + ImGui::GetItemRectSize().x; // First element of a line

			if (ImGui::IsItemHoveredRect())
			{
				if (tabOrder)
				{
					// tab reordering
					if (isMouseDragging)
					{
						if (draggingTabIndex == -1)
						{
							draggingTabIndex = j;
							draggingtabSize = ImGui::GetItemRectSize();
							const ImVec2& mp = ImGui::GetIO().MousePos;
							const ImVec2 draggingTabCursorPos = ImGui::GetCursorPos();
							draggingTabOffset = ImVec2(
								mp.x + draggingtabSize.x*0.5f - sumX + ImGui::GetScrollX(),
								mp.y + draggingtabSize.y*0.5f - draggingTabCursorPos.y + ImGui::GetScrollY()
							);

						}
					}
					else if (draggingTabIndex >= 0 && draggingTabIndex<tabSize && draggingTabIndex != j)
					{
						draggingTabTargetIndex = j; // For some odd reasons this seems to get called only when draggingTabIndex < i ! (Probably during mouse dragging ImGui owns the mouse someway and sometimes ImGui::IsItemHovered() is not getting called)
					}
				}
			}

		}

		tabIndex = newtabIndex;

		// Draw tab label while mouse drags it
		if (draggingTabIndex >= 0 && draggingTabIndex<tabSize)
		{
			const ImVec2& mp = ImGui::GetIO().MousePos;
			const ImVec2 wp = ImGui::GetWindowPos();
			ImVec2 start(wp.x + mp.x - draggingTabOffset.x - draggingtabSize.x*0.5f, wp.y + mp.y - draggingTabOffset.y - draggingtabSize.y*0.5f);
			const ImVec2 end(start.x + draggingtabSize.x, start.y + draggingtabSize.y);
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			const float draggedBtnAlpha = 0.65f;
			const ImVec4& btnColor = style.Colors[ImGuiCol_Button];
			drawList->AddRectFilled(start, end, ImColor(btnColor.x, btnColor.y, btnColor.z, btnColor.w*draggedBtnAlpha), style.FrameRounding);
			start.x += style.FramePadding.x; start.y += style.FramePadding.y;
			const ImVec4& txtColor = style.Colors[ImGuiCol_Text];
			drawList->AddText(start, ImColor(txtColor.x, txtColor.y, txtColor.z, txtColor.w*draggedBtnAlpha), tabLabels[tabOrder[draggingTabIndex]]);

			ImGui::SetMouseCursor(ImGuiMouseCursor_Move);
		}

		// Drop tab label
		if (draggingTabTargetIndex != -1)
		{
			// swap draggingTabIndex and draggingTabTargetIndex in tabOrder
			const int tmp = tabOrder[draggingTabTargetIndex];
			tabOrder[draggingTabTargetIndex] = tabOrder[draggingTabIndex];
			tabOrder[draggingTabIndex] = tmp;
			//fprintf(stderr,"%d %d\n",draggingTabIndex,draggingTabTargetIndex);
			draggingTabTargetIndex = draggingTabIndex = -1;
		}

		// Reset draggingTabIndex if necessary
		if (!isMouseDragging) draggingTabIndex = -1;

		// Change selected tab when user closes the selected tab
		if (tabIndex == justClosedTabIndex && tabIndex >= 0)
		{
			tabIndex = -1;
			for (int j = 0, i; j < tabSize; j++)
			{
				i = tabOrder ? tabOrder[j] : j;
				if (i == -1)
				{
					continue;
				}
				tabIndex = i;
				break;
			}
		}

		// Restore the style
		style.Colors[ImGuiCol_Button] = color;
		style.Colors[ImGuiCol_ButtonActive] = colorActive;
		style.Colors[ImGuiCol_ButtonHovered] = colorHover;
		style.Colors[ImGuiCol_Text] = colorText;
		style.ItemSpacing = itemSpacing;

		return selection_changed;
	}
}
void buttonmenu2(int d) {
	buttonmenu = d;
}

void color()
{
	auto bColor = Vars.g_fBColor;
	auto mColor = Vars.g_fMColor;
	auto tColor = Vars.g_fTColor;
	//int(enemyColor[0] * 255.0f), int(enemyColor[1] * 255.0f), int(enemyColor[2] * 255.0f), 255

	ImColor mainColor = ImColor(int(mColor[0] * 255.0f), int(mColor[1] * 255.0f), int(mColor[2] * 255.0f), 255);
	ImColor bodyColor = ImColor(int(bColor[0] * 255.0f), int(bColor[1] * 255.0f), int(bColor[2] * 255.0f), 255);
	ImColor fontColor = ImColor(int(tColor[0] * 255.0f), int(tColor[1] * 255.0f), int(tColor[2] * 255.0f), 255);

	ImGuiStyle& style = ImGui::GetStyle();

	ImVec4 mainColorHovered = ImVec4(mainColor.Value.x + 0.1f, mainColor.Value.y + 0.1f, mainColor.Value.z + 0.1f, mainColor.Value.w);
	ImVec4 mainColorActive = ImVec4(mainColor.Value.x + 0.2f, mainColor.Value.y + 0.2f, mainColor.Value.z + 0.2f, mainColor.Value.w);
	ImVec4 menubarColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w - 0.8f);
	ImVec4 frameBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .1f);
	ImVec4 tooltipBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .05f);

	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(8, 8);
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ChildWindowRounding = 0.0f;
	style.FramePadding = ImVec2(4, 3);
	style.FrameRounding = 0.0f;
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 3.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	style.AntiAliasedShapes = true;
	style.CurveTessellationTol = 1.25f;

	style.Colors[ImGuiCol_Text] = fontColor;

	style.Colors[ImGuiCol_Tab] = mainColor;
	style.Colors[ImGuiCol_TabHovered] = mainColorHovered;
	style.Colors[ImGuiCol_TabActive] = mainColorActive;
	style.Colors[ImGuiCol_TabText] = ImColor(fontColor.Value.x - .6f, fontColor.Value.y - .6f, fontColor.Value.z - .6f, fontColor.Value.w);
	style.Colors[ImGuiCol_TabTextActive] = fontColor;
	style.Colors[ImGuiCol_TabSelected] = bodyColor;

	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = bodyColor;
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(.0f, .0f, .0f, .0f);
	style.Colors[ImGuiCol_PopupBg] = tooltipBgColor;
	style.Colors[ImGuiCol_Border] = mainColor;
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = frameBgColor;
	style.Colors[ImGuiCol_FrameBgHovered] = mainColorHovered;
	style.Colors[ImGuiCol_FrameBgActive] = mainColorActive;
	style.Colors[ImGuiCol_TitleBg] = mainColor;
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = mainColor;
	style.Colors[ImGuiCol_MenuBarBg] = menubarColor;
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(frameBgColor.x + .05f, frameBgColor.y + .05f, frameBgColor.z + .05f, frameBgColor.w);
	style.Colors[ImGuiCol_ScrollbarGrab] = mainColor;
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ScrollbarGrabActive] = mainColorActive;
	style.Colors[ImGuiCol_ComboBg] = frameBgColor;
	style.Colors[ImGuiCol_CheckMark] = mainColor;
	style.Colors[ImGuiCol_SliderGrab] = mainColorHovered;
	style.Colors[ImGuiCol_SliderGrabActive] = mainColorActive;
	style.Colors[ImGuiCol_Button] = mainColor;
	style.Colors[ImGuiCol_ButtonHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ButtonActive] = mainColorActive;
	style.Colors[ImGuiCol_Header] = mainColor;
	style.Colors[ImGuiCol_HeaderHovered] = mainColorHovered;
	style.Colors[ImGuiCol_HeaderActive] = mainColorActive;

	style.Colors[ImGuiCol_Column] = mainColor;
	style.Colors[ImGuiCol_ColumnHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ColumnActive] = mainColorActive;

	style.Colors[ImGuiCol_ResizeGrip] = mainColor;
	style.Colors[ImGuiCol_ResizeGripHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ResizeGripActive] = mainColorActive;
	style.Colors[ImGuiCol_CloseButton] = mainColor;
	style.Colors[ImGuiCol_CloseButtonHovered] = mainColorHovered;
	style.Colors[ImGuiCol_CloseButtonActive] = mainColorActive;
	style.Colors[ImGuiCol_PlotLines] = mainColor;
	style.Colors[ImGuiCol_PlotLinesHovered] = mainColorHovered;
	style.Colors[ImGuiCol_PlotHistogram] = mainColor;
	style.Colors[ImGuiCol_PlotHistogramHovered] = mainColorHovered;
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

char* KeyStrings[254] = { "No key", "Left Mouse", "Right Mouse", "Control+Break", "Middle Mouse", "Mouse 4", "Mouse 5",
nullptr, "Backspace", "TAB", nullptr, nullptr, nullptr, "ENTER", nullptr, nullptr, "SHIFT", "CTRL", "ALT", "PAUSE",
"CAPS LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "ESC", nullptr, nullptr, nullptr, nullptr, "SPACEBAR",
"PG UP", "PG DOWN", "END", "HOME", "Left", "Up", "Right", "Down", nullptr, "Print", nullptr, "Print Screen", "Insert",
"Delete", nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", "Left Windows", "Right Windows", nullptr, nullptr, nullptr, "NUM 0", "NUM 1", "NUM 2", "NUM 3", "NUM 4", "NUM 5", "NUM 6",
"NUM 7", "NUM 8", "NUM 9", "*", "+", "_", "-", ".", "/", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
"F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, "NUM LOCK", "SCROLL LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, "LSHIFT", "RSHIFT", "LCONTROL", "RCONTROL", "LMENU", "RMENU", nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "Next Track", "Previous Track", "Stop", "Play/Pause", nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[{", "\\|", "}]", "'\"", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

namespace ImGui
{


	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	void KeyBindButton(int Key)
	{
		static bool shouldListen = false;
		static int key = 0;
		if (shouldListen)
		{
			for (int i = 0; i < 255; i++)
			{
				if (GetAsyncKeyState(i))
				{
					if (i == VK_ESCAPE)
					{
						shouldListen = false;
						key = -1;
					}

					key = i;
					shouldListen = false;
				}

			}
		}
		if (ImGui::Button(KeyStrings[key], ImVec2(-1, 0)))
		{
			shouldListen = true;
		}

		Key = key;
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items = -1)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size(), height_in_items);
	}
}



#pragma once

bool get_system_font_path(const std::string& name, std::string& path)
{
	//
	// This code is not as safe as it should be.
	// Assumptions we make:
	//  -> GetWindowsDirectoryA does not fail.
	//  -> The registry key exists.
	//  -> The subkeys are ordered alphabetically
	//  -> The subkeys name and data are no longer than 260 (MAX_PATH) chars.
	//

	char buffer[MAX_PATH];
	HKEY registryKey;

	GetWindowsDirectoryA(buffer, MAX_PATH);
	std::string fontsFolder = buffer + std::string("\\Fonts\\");

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &registryKey)) {
		return false;
	}

	uint32_t valueIndex = 0;
	char valueName[MAX_PATH];
	uint8_t valueData[MAX_PATH];
	std::wstring wsFontFile;

	do {
		uint32_t valueNameSize = MAX_PATH;
		uint32_t valueDataSize = MAX_PATH;
		uint32_t valueType;

		auto error = RegEnumValueA(
			registryKey,
			valueIndex,
			valueName,
			reinterpret_cast<DWORD*>(&valueNameSize),
			0,
			reinterpret_cast<DWORD*>(&valueType),
			valueData,
			reinterpret_cast<DWORD*>(&valueDataSize));

		valueIndex++;

		if (error == ERROR_NO_MORE_ITEMS) {
			RegCloseKey(registryKey);
			return false;
		}

		if (error || valueType != REG_SZ) {
			continue;
		}

		if (_strnicmp(name.data(), valueName, name.size()) == 0) {
			path = fontsFolder + std::string((char*)valueData, valueDataSize);
			RegCloseKey(registryKey);
			return true;
		}
	} while (true);

	return false;
}



int windowWidth = 800;
int windowHeight = 425;
int curWidth = windowWidth;
int curHeight = windowHeight;
int curX = 40;
int curY = 70;
int tabHeight = 30;

LONGLONG LastFrameTime;
float AndeltaTime;

LONGLONG Anmilliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}

float GetAnDeltaTime()
{
	LONGLONG ms = Anmilliseconds_now();
	float ret = ms - LastFrameTime;
	LastFrameTime = ms;
	return ret;
	//return 1000.0f / ImGui::GetIO().Framerate;
}

int CalcTabWidth(int tabs)
{
	ImGuiStyle& style = ImGui::GetStyle();
	//return ((windowWidth - (style.WindowPadding.x * 2)) / tabs) - (style.ItemSpacing.x * tabs);
	return (windowWidth - ((style.WindowPadding.x) + (style.ItemSpacing.x * tabs))) / tabs;
	//return windowWidth / tabs;
}





std::vector<std::string> configs;

void GetConfigMassive()
{
	//get all files on folder

	configs.clear();

	static char path[MAX_PATH];
	std::string szPath1;

	if (!SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path)))
		return;

	szPath1 = std::string(path) + XorStr("\\Picoloware\\*");


	WIN32_FIND_DATA FindFileData;
	HANDLE hf;
	configs.push_back("default.ini");

	hf = FindFirstFile(szPath1.c_str(), &FindFileData);
	if (hf != INVALID_HANDLE_VALUE) {
		do {
			std::string filename = FindFileData.cFileName;

			if (filename == ".")
				continue;

			if (filename == "..")
				continue;

			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if (filename.find(".ini") != std::string::npos)
				{
					configs.push_back(std::string(filename));
				}
			}
		} while (FindNextFile(hf, &FindFileData) != 0);
		FindClose(hf);
	}
}







Vector2D RotatePoint(Vector EntityPos, Vector LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck, bool angleInRadians = false)
{
	float r_1, r_2;
	float x_1, y_1;

	r_1 = -(EntityPos.y - LocalPlayerPos.y);
	r_2 = EntityPos.x - LocalPlayerPos.x;
	float Yaw = angle - 90.0f;

	float yawToRadian = Yaw * (float)(M_PI / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

	*viewCheck = y_1 < 0;

	x_1 *= zoom;
	y_1 *= zoom;

	int sizX = sizeX / 2;
	int sizY = sizeY / 2;

	x_1 += sizX;
	y_1 += sizY;

	if (x_1 < 5)
		x_1 = 5;

	if (x_1 > sizeX - 5)
		x_1 = sizeX - 5;

	if (y_1 < 5)
		y_1 = 5;

	if (y_1 > sizeY - 5)
		y_1 = sizeY - 5;


	x_1 += posX;
	y_1 += posY;


	return Vector2D(x_1, y_1);


	/*if (!angleInRadians)
	angle = (float)(angle * (M_PI / 180));
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);
	Vector2 returnVec = Vector2(
	cosTheta * (pointToRotate.x - centerPoint.x) - sinTheta * (pointToRotate.y - centerPoint.y),
	sinTheta * (pointToRotate.x - centerPoint.x) + cosTheta * (pointToRotate.y - centerPoint.y)
	);
	returnVec += centerPoint;
	return returnVec / zoom;*/
}

bool EntityIsInvalid(CBaseEntity* Entity)
{
	//HANDLE obs = Entity->GetObserverTargetHandle();
	//CBaseEntity *pTarget = I::ClientEntList->GetClientEntityFromHandle(obs);
	if (!Entity)
		return true;
	//if (Entity == pTarget)
	//return true;
	if (Entity->GetHealth() <= 0)
		return true;
	if (Entity->GetDormant())
		return true;

	return false;
}
void EspTab()
{
	const char* tabNames[] =
	{
		"Visuals" , "Radar" ,  "Chams" , "Misc" };

	static int tabOrder[] = { 0 , 1 , 2 , 3};
	static int tabSelected = 0;
	const bool tabChanged = ImGui::TabLabels(tabNames,
		sizeof(tabNames) / sizeof(tabNames[0]),
		tabSelected, tabOrder);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	float SpaceLineOne = 320.f;
	float SpaceLineTwo = 320.f;
	float SpaceLineThr = 320.f;

	if (tabSelected == 0) // Visuals
	{
		ImGui::Columns(2, NULL, true, true);
		{
			ImGui::Checkbox(XorStr("Enable Visuals"), &Vars.Visuals.Enabled);
		
			ImGui::Checkbox(XorStr("Corner Box"), &Vars.Visuals.Box);
			ImGui::Checkbox(XorStr("3D Box"), &Vars.Visuals.esp3d);
			ImGui::Checkbox(XorStr("Skeleton"), &Vars.Visuals.Skeleton);
			ImGui::Checkbox(XorStr("Enemy View Line"), &Vars.Visuals.BulletTrace);
			ImGui::Checkbox(XorStr("Anti-Screenshot"), &Vars.Visuals.AntiSS);
		}


		ImGui::NextColumn();
		{

			ImGui::Selectable(XorStr(" Show Name"), &Vars.Visuals.InfoName);
			ImGui::Selectable(XorStr(" Show Health"), &Vars.Visuals.InfoHealth);
			ImGui::Selectable(XorStr(" Show Weapon"), &Vars.Visuals.InfoWeapon);
			ImGui::Selectable(XorStr(" Show If Flashed"), &Vars.Visuals.InfoFlashed);

			ImGui::Selectable(XorStr(" Show Enemies"), &Vars.Visuals.Filter.Enemies);
			ImGui::Selectable(XorStr(" Show Teammates"), &Vars.Visuals.Filter.Friendlies);
			ImGui::Selectable(XorStr(" Show Weapons"), &Vars.Visuals.Filter.Weapons);
			ImGui::Selectable(XorStr(" Show Decoy"), &Vars.Visuals.Filter.Decoy);
			ImGui::Selectable(XorStr(" Show C4"), &Vars.Visuals.Filter.C4);

		}

	}
	if (tabSelected == 1) // Radar
	{

		ImGui::Checkbox(XorStr("Enable Radar"), &Vars.Visuals.Radar.Enabled);
		ImGui::SliderInt(XorStr("Radar Range"), &Vars.Visuals.Radar.range, 1, 100);
		ImGui::Checkbox(XorStr("Only Enemy"), &Vars.Visuals.Radar.EnemyOnly);
		ImGui::Checkbox(XorStr("Show Nicknames"), &Vars.Visuals.Radar.Nicks);

		if (ImGui::ColorEdit3(XorStr("Body clr"), Vars.g_fBColor) ||
			ImGui::ColorEdit3(XorStr("Main clr"), Vars.g_fMColor) ||
			ImGui::ColorEdit3(XorStr("Text clr"), Vars.g_fTColor)) color();

		ImGui::ColorEdit3(XorStr("Chams CT Visible"), Vars.g_iChamsCTV);
		ImGui::ColorEdit3(XorStr("Chams CT Hidden"), Vars.g_iChamsCTH);
		ImGui::ColorEdit3(XorStr("Chams T Visible"), Vars.g_iChamsTV);
		ImGui::ColorEdit3(XorStr("Chams T Hidden"), Vars.g_iChamsTH);

	}
	if (tabSelected == 2) // Chams
	{

		ImGui::Columns(2, NULL, true, true);
		{
			ImGui::Checkbox(XorStr("Chams Enable"), &Vars.Visuals.Chams.Enabled);
			ImGui::SameLine();
			ImGui::Combo(XorStr("Mode"), &Vars.Visuals.Chams.Mode, XorStr("Flat\0\rTextured\0\0"), -1, ImVec2(70, 0));
			ImGui::Checkbox(XorStr("XQZ"), &Vars.Visuals.Chams.XQZ);
			ImGui::Checkbox(XorStr("Hands"), &Vars.Visuals.Chams.hands);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("Weapon"), &Vars.Visuals.ChamsWeapon);
			ImGui::Checkbox(XorStr("Rainbow Hands"), &Vars.Visuals.Chams.RainbowHands);
			ImGui::Text(XorStr("Speed"));
			ImGui::SameLine();
			ImGui::SliderFloat(XorStr("##CHMSSpeed"), &Vars.Visuals.Chams.RainbowTime, 1.f, 150.f, "%.0f");

			ImGui::Text(XorStr("Removals"));
			ImGui::Checkbox(XorStr("No Visual Recoil"), &Vars.Visuals.RemovalsVisualRecoil);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("No Flash"), &Vars.Visuals.RemovalsFlash);
			ImGui::Checkbox(XorStr("No Scope"), &Vars.Visuals.NoScope);
		}


		ImGui::NextColumn();
		{
			ImGui::Text(XorStr("Chams Colors"));
			ImGui::ColorEdit3(XorStr("Chams CT Visible"), Vars.g_iChamsCTV);
			ImGui::ColorEdit3(XorStr("Chams CT Hidden"), Vars.g_iChamsCTH);
			ImGui::ColorEdit3(XorStr("Chams T Visible"), Vars.g_iChamsTV);
			ImGui::ColorEdit3(XorStr("Chams T Hidden"), Vars.g_iChamsTH);


		}

	}
	if (tabSelected == 3) // Misc
	{

		ImGui::Columns(2, NULL, true, true);
		{
			ImGui::Text(XorStr("Override Fov"));
			ImGui::SameLine();
			ImGui::SliderInt(XorStr("##OverrideFov"), &Vars.Misc.fov, -70, 70);
			ImGui::Text(XorStr("ViewModel Fov"));
			ImGui::SameLine();
			ImGui::SliderInt(XorStr("##ViewFov"), &Vars.Visuals.ViewmodelFov, -70, 70);
			ImGui::Checkbox(XorStr("ThirdPerson"), &Vars.Visuals.thirdperson.enabled);
			ImGui::Text(XorStr("Distance"));
			ImGui::SameLine();
			ImGui::SliderFloat(XorStr("##THRDPERSDIST"), &Vars.Visuals.thirdperson.distance, 30.f, 200.f, "%.0f");
			ImGui::Checkbox(XorStr("SpectatorList"), &Vars.Visuals.SpectatorList);
			ImGui::Combo(XorStr("Vector to Enemies"), &Vars.Visuals.Line, XorStr("Off\0\rCentered\0\rDown\0\0"), -1, ImVec2(70, 0));
		}


		ImGui::NextColumn();
		{
			ImGui::Checkbox(XorStr("Enable Hitmarker"), &Vars.Visuals.HitMarker);
			ImGui::SliderFloat(XorStr("##HitmarkerSize"), &Vars.Visuals.HitMarkerSize, 10.f, 50.f, "%.0f");
			ImGui::Combo(XorStr("Hitsound"), &Vars.Visuals.HitSound, XorStr("Off\0\rNormal\0\rSkeet\0\rCustom\0\0"), -1, ImVec2(100, 0));
			ImGui::Combo(XorStr("Sky"), &Vars.Visuals.SkyChanger, skyList, ARRAYSIZE(skyList));
			ImGui::Checkbox(XorStr("LBYSTATS Enabled"), &Vars.Visuals.LBYSTATS);
			ImGui::Spacing();
			ImGui::Checkbox(XorStr("Crosshair Enabled"), &Vars.Visuals.CrosshairOn);
			ImGui::Checkbox(XorStr("Show Spread"), &Vars.Visuals.CrosshairSpread);
			ImGui::Checkbox(XorStr("Show FOV"), &Vars.Visuals.CrosshairFOV);
			ImGui::Checkbox(XorStr("Show Recoil"), &Vars.Visuals.CrosshairType);
			ImGui::Checkbox(XorStr("Rainbow Color"), &Vars.Visuals.CrosshairRainbow);
			ImGui::Text(XorStr("Crosshair Style"));
			ImGui::SameLine();
			ImGui::Combo(XorStr("##CRSHRStyle"), &Vars.Visuals.CrosshairStyle, XorStr("Plus\0\rCircle\0\rBox\0\0"), -1, ImVec2(70, 0));

		}
	}
	
}
void DrawRadar()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 oldPadding = style.WindowPadding;
	float oldAlpha = style.Colors[ImGuiCol_WindowBg].w;
	style.WindowPadding = ImVec2(0, 0);
	style.Colors[ImGuiCol_WindowBg].w = 1.f;
	if (ImGui::Begin(XorStr("Radar"), &Vars.Visuals.Radar.Enabled, ImVec2(200, 200), 0.4F, ImGuiWindowFlags_NoTitleBar |/*ImGuiWindowFlags_NoResize | */ImGuiWindowFlags_NoCollapse))
	{
		ImVec2 siz = ImGui::GetWindowSize();
		ImVec2 pos = ImGui::GetWindowPos();

		ImDrawList* DrawList = ImGui::GetWindowDrawList();

		DrawList->AddRect(ImVec2(pos.x - 6, pos.y - 6), ImVec2(pos.x + siz.x + 6, pos.y + siz.y + 6), Color::Black().GetU32(), 0.0F, -1, 1.5f);
		//_drawList->AddRect(ImVec2(pos.x - 2, pos.y - 2), ImVec2(pos.x + siz.x + 2, pos.y + siz.y + 2), Color::Black().GetU32(), 0.0F, -1, 1);

		//_drawList->AddRect(ImVec2(pos.x - 2, pos.y - 2), ImVec2(pos.x + siz.x + 2, pos.y + siz.y + 2), Color::Black().GetU32(), 0.0F, -1, 2);
		//_drawList->AddRect(ImVec2(pos.x - 2, pos.y - 2), ImVec2(pos.x + siz.x + 2, pos.y + siz.y + 2), Color::Silver().GetU32(), 0.0F, -1, 1.1f);

		ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
		windowDrawList->AddLine(ImVec2(pos.x + (siz.x / 2), pos.y + 0), ImVec2(pos.x + (siz.x / 2), pos.y + siz.y), Color::Black().GetU32(), 1.5f);
		windowDrawList->AddLine(ImVec2(pos.x + 0, pos.y + (siz.y / 2)), ImVec2(pos.x + siz.x, pos.y + (siz.y / 2)), Color::Black().GetU32(), 1.5f);

		// Rendering players

		if (I::Engine->IsInGame() && I::Engine->IsConnected())
		{
			//CPlayer* pLocalEntity = Interface.EntityList->GetClientEntity<CPlayer>(Interface.Engine->GetLocalPlayer());
			if (G::LocalPlayer)
			{
				Vector LocalPos = G::LocalPlayer->GetEyePosition();
				QAngle ang;
				I::Engine->GetViewAngles(ang);
				for (int i = 0; i < I::Engine->GetMaxClients(); i++) {
					CBaseEntity* pBaseEntity = I::ClientEntList->GetClientEntity(i);

					player_info_t pInfo;
					I::Engine->GetPlayerInfo(i, &pInfo);

					if (EntityIsInvalid(pBaseEntity))
						continue;

					CBaseEntity* observerTarget = I::ClientEntList->GetClientEntityFromHandle(G::LocalPlayer->GetObserverTargetHandle());

					bool bIsEnemy = (G::LocalPlayer->GetTeam() != pBaseEntity->GetTeam() || pBaseEntity == observerTarget || pBaseEntity == G::LocalPlayer) ? true : false;

					if (Vars.Visuals.Radar.EnemyOnly && !bIsEnemy)
						continue;

					bool viewCheck = false;
					Vector2D EntityPos = RotatePoint(pBaseEntity->GetOrigin(), LocalPos, pos.x, pos.y, siz.x, siz.y, ang.y, Vars.Visuals.Radar.range, &viewCheck);

					//ImU32 clr = (bIsEnemy ? (isVisibled ? Color::LightGreen() : Color::Blue()) : Color::White()).GetU32();
					ImU32 clr = (bIsEnemy ? Color::Red() : Color::LightBlue()).GetU32();
					static bool drawname = true;

					if (pBaseEntity == observerTarget || pBaseEntity == G::LocalPlayer)
					{
						clr = Color::White().GetU32();
						drawname = false;
					}
					else
						drawname = true;

					int s = 2;

					windowDrawList->AddRect(ImVec2(EntityPos.x - s, EntityPos.y - s),
						ImVec2(EntityPos.x + s, EntityPos.y + s),
						clr);

					RECT TextSize = D::GetTextSize(F::ESP, pInfo.name);

					if (drawname && Vars.Visuals.Radar.Nicks)
						windowDrawList->AddText(ImVec2(EntityPos.x - (TextSize.left / 2), EntityPos.y - s), Color::White().GetU32(), pInfo.name);
				}
			}
		}
	}
	ImGui::End();
	style.WindowPadding = oldPadding;
	style.Colors[ImGuiCol_WindowBg].w = oldAlpha;
}
void RadarTab()
{
	ImGui::Checkbox(XorStr("Enable Radar"), &Vars.Visuals.Radar.Enabled);
	ImGui::SliderInt(XorStr("Radar Range"), &Vars.Visuals.Radar.range, 1, 100);
	ImGui::Checkbox(XorStr("Only Enemy"), &Vars.Visuals.Radar.EnemyOnly);
	ImGui::Checkbox(XorStr("Show Nicknames"), &Vars.Visuals.Radar.Nicks);

	if (ImGui::ColorEdit3(XorStr("Body clr"), Vars.g_fBColor) ||
		ImGui::ColorEdit3(XorStr("Main clr"), Vars.g_fMColor) ||
		ImGui::ColorEdit3(XorStr("Text clr"), Vars.g_fTColor)) color();

	ImGui::ColorEdit3(XorStr("Chams CT Visible"), Vars.g_iChamsCTV);
	ImGui::ColorEdit3(XorStr("Chams CT Hidden"), Vars.g_iChamsCTH);
	ImGui::ColorEdit3(XorStr("Chams T Visible"), Vars.g_iChamsTV);
	ImGui::ColorEdit3(XorStr("Chams T Hidden"), Vars.g_iChamsTH);

	//SkinsTab();
}




void MiscTab()
{
	const char* tabNames[] =
	{
		"NameSteal & SpamChat" , "Movement & ClanTag" , "Config" };

	static int tabOrder[] = { 0 , 1 , 2 };
	static int tabSelected = 0;
	const bool tabChanged = ImGui::TabLabels(tabNames,
		sizeof(tabNames) / sizeof(tabNames[0]),
		tabSelected, tabOrder);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	float SpaceLineOne = 320.f;
	float SpaceLineTwo = 320.f;
	float SpaceLineThr = 320.f;

	if (tabSelected == 0) // NameSteal and SpamChat
	{
		ImGui::Columns(2, NULL, true, true);
		{
			ImGui::Text(XorStr("Message"));
			ImGui::SameLine();
			ImGui::InputText(XorStr("##SpamMessage"), Vars.Misc.ChatSpamMode, 128);
			ImGui::Text(XorStr("Delay"));
			ImGui::SameLine();
			ImGui::SliderFloat(XorStr("##Spam Delay"), &Vars.Misc.ChatSpamDelay, 0.1f, 10.f);
			ImGui::Checkbox(XorStr("ChatSpam"), &Vars.Misc.ChatSpam);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("Location Spam"), &Vars.Misc.LocSpam);
			ImGui::Checkbox(XorStr("LocSpam Enemies only"), &Vars.Misc.LocSpamEnemies);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("LocSpam teamchat only"), &Vars.Misc.LocSpamChat);
			ImGui::Text(XorStr("Event Spam"));
			ImGui::Checkbox(XorStr("On Headshot"), &Vars.Misc.TapSay);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("On Kill"), &Vars.Misc.SoundShit);
			ImGui::Checkbox(XorStr("Bomb Info"), &Vars.Misc.bombinfo);
			
	
		}


		ImGui::NextColumn();
		{

			ImGui::Checkbox(XorStr("Name Steal"), &Vars.Misc.NameSteal);
			ImGui::SameLine();
			ImGui::SliderFloat(XorStr(""), &Vars.Misc.NameStealDelay, 0.1f, 10.f);
			static char nickname[128] = "";
			static char nickseparator[128] = "";
			static bool init_nick = false;
			ImGui::Text(XorStr("Name"));
			ImGui::InputText(XorStr("##NICTEXT"), nickname, 127);
			if (ImGui::Button(XorStr("Set Nickname"), ImVec2(-1, 0)))
			{
				std::string ctWithEscapesProcessed = std::string(nickname);
				U::StdReplaceStr(ctWithEscapesProcessed, "\\n", "\n");

				E::Misc->ChangeName(ctWithEscapesProcessed.c_str());
			}

			if (!init_nick)
			{
				sprintf(nickname, "Picoloware <--");
				init_nick = true;
			}



		}
	}
	if (tabSelected == 1) // Movement And ClanTag
	{

		ImGui::Columns(2, NULL, true, true);
		{
			ImGui::Text(XorStr("Movement"));
			ImGui::Combo(XorStr("FakeLag"), &Vars.Misc.FakeLag, XorStr("Off\0\rFactor\0\rSwitch\0\rAdaptive\0\rAdaptive 2\0\0"), -1);
			ImGui::SliderInt(XorStr("##FAKELAG"), &Vars.Misc.FakeLags, 0, 16);
			ImGui::Checkbox(XorStr("Bunny Hop"), &Vars.Misc.Bhop);
			ImGui::SameLine();
			ImGui::Combo(XorStr("##STRAFER"), &Vars.Misc.AutoStrafe, XorStr("Off\0\rStrafe Helper\0\rFull Strafer\0\0"), -1);
			ImGui::Checkbox(XorStr("Air Stuck"), &Vars.Misc.AirStuck);
			if (Vars.Misc.AirStuck)
			{
				ImGui::SameLine();
				ImGui::Combo(XorStr("Key"), &Vars.Misc.AirStuckKey, keyNames, IM_ARRAYSIZE(keyNames));
			}
			ImGui::Text(XorStr("Miscellaneous"));
			ImGui::Checkbox(XorStr("Ranks Reveal"), &Vars.Misc.Ranks); ImGui::SameLine();
			ImGui::Checkbox(XorStr("Auto Accept"), &Vars.Misc.AutoAccept); ImGui::SameLine();
			ImGui::Checkbox(XorStr("Watermark"), &Vars.Misc.Watermark); ImGui::SameLine();
		}


		ImGui::NextColumn();
		{

			ImGui::Checkbox(XorStr("Enabled"), &Vars.Misc.ClantagChanger.enabled);
			ImGui::Text(XorStr("Text"));
			static bool init_clan = false;
			if (!init_clan)
			{
				sprintf(Vars.Misc.ClantagChanger.value, "Picoloware ^-^");
				init_clan = true;
			}

			if (ImGui::InputText(XorStr("##CLANTAGTEXT"), Vars.Misc.ClantagChanger.value, 30))
				E::ClantagChanger->UpdateClanTagCallback();
			ImGui::Text(XorStr("Clantag Type"));
			if (ImGui::Combo(XorStr("##ANIM"), &Vars.Misc.ClantagChanger.type, XorStr("Static\0\rMarquee\0\rWords\0\rLetters\0\rTime\0\0"), -1))
				E::ClantagChanger->UpdateClanTagCallback();
			ImGui::Text(XorStr("Animation Speed"));
			if (ImGui::SliderInt(XorStr("##ANIMSPEED"), &Vars.Misc.ClantagChanger.animation_speed, 0, 2000))
				E::ClantagChanger->UpdateClanTagCallback();

		}

	}
	if (tabSelected == 2) // Misc
	{

		ImGui::Columns(2, NULL, true, true);
		{

			GetConfigMassive();
			ImGui::Text(XorStr("Config Name"));
			//configs
			static int selectedcfg = 0;
			static std::string cfgname = "default";
			if (ImGui::Combo(XorStr("Configs"), &selectedcfg, [](void* data, int idx, const char** out_text)
			{
				*out_text = configs[idx].c_str();
				return true;
			}, nullptr, configs.size(), 10))
			{
				cfgname = configs[selectedcfg];
				cfgname.erase(cfgname.length() - 4, 4);
				strcpy(Vars.Misc.configname, cfgname.c_str());
			}

			ImGui::InputText(XorStr("Current Config"), Vars.Misc.configname, 128);
			if (ImGui::Button(XorStr("Save Config"), ImVec2(93.f, 20.f))) Config->Save();
			ImGui::SameLine();
			if (ImGui::Button(XorStr("Load Config"), ImVec2(93.f, 20.f))) {
				Config->Load(); color();
			}
			ImGui::Text("");
			ImGui::Text("");
			ImGui::Text("");
			ImGui::Text("");
			ImGui::Text("");
		}
		ImGui::NextColumn();
		{
			ImGui::Text(XorStr("Other"));
			if (ImGui::Button(XorStr("Unload"), ImVec2(93.f, 20.f)))
			{
				E::Misc->Panic();
			}
			if (ImGui::Button(XorStr("Font Fix"), ImVec2(93.f, 20.f)))
				D::SetupFonts();
		}
		ImGui::Columns(1);
	}
	///

}


void SkinsTab()
{
	ImGui::Checkbox(XorStr("Enable Skins"), &Vars.Skins.SSEnabled);


	if (Vars.Skins.SSEnabled)
	{
		ImGui::BeginChild(XorStr("##SKINS"), ImVec2(0, 0), true);
		{
			ImGui::PushItemWidth(270);
			if (G::LocalPlayer->GetAlive())
				ImGui::Text(XorStr("Current Weapon: %s"), G::LocalPlayer->GetWeapon()->GetWeaponName().c_str());
			else
				ImGui::Text(XorStr("Invalid weapon/Isnt Alive"));


			ImGui::Combo(XorStr("Paint Kit"), &Vars.Skins.Weapons[Vars.wpn].PaintKit, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_Skins[idx].strName.c_str();
				return true;
			}, nullptr, k_Skins.size(), 10);
			//static int pkit = 0;
			/*ImGui::Combo(XorStr("glove Kit"), &pkit, [](void* data, int idx, const char** out_text)
			{
			*out_text = k_Gloves[idx].strName.c_str();
			return true;
			}, nullptr, k_Gloves.size(), 10);*/
			//k_Skins[Vars.Skins.Weapons[pWeapon->GetItemDefinitionIndex()].PaintKit].iIndex;
			//ImGui::Text("%d", k_Gloves[pkit].iIndex);


			ImGui::Separator();
			//ImGui::Combo(XorStr("Knife Skin"), &Vars.Skins.KnifeSkin, KnifeSkin, IM_ARRAYSIZE(KnifeSkin));
			ImGui::Combo(XorStr("Knife Model"), &Vars.Skins.KnifeModel, charenc("No Knife\0\rBayonet\0\rFlip Knife\0\rGut Knife\0\rKarambit\0\rM9 Bayonet\0\rHuntsman Knife\0\rButterfly Knife\0\rFalchion Knife\0\rShadow Daggers\0\rBowie Knife\0\0"), -1, ImVec2(130, 0));
			ImGui::Separator();

			if (ImGui::Combo(XorStr("Glove Model"), &Vars.Skins.Glove, XorStr("No Glove\0\rBloodhound\0\rSport\0\rDriver\0\rWraps\0\rMoto\0\rSpecialist\0\0"), -1, ImVec2(130, 0)))
				U::CL_FullUpdate();

			const char* gstr;
			if (Vars.Skins.Glove == 1)
			{
				gstr = XorStr("Charred\0\rSnakebite\0\rBronzed\0\rGuerilla\0\0");
			}
			else if (Vars.Skins.Glove == 2)
			{
				gstr = XorStr("Hedge Maze\0\rPandoras Box\0\rSuperconductor\0\rArid\0\0");
			}
			else if (Vars.Skins.Glove == 3)
			{
				gstr = XorStr("Lunar Weave\0\rConvoy\0\rCrimson Weave\0\rDiamondback\0\0");
			}
			else if (Vars.Skins.Glove == 4)
			{
				gstr = XorStr("Leather\0\rSpruce DDPAT\0\rSlaughter\0\rBadlands\0\0");
			}
			else if (Vars.Skins.Glove == 5)
			{
				gstr = XorStr("Eclipse\0\rSpearmint\0\rBoom!\0\rCool Mint\0\0");
			}
			else if (Vars.Skins.Glove == 6)
			{
				gstr = XorStr("Forest DDPAT\0\rCrimson Kimono\0\rEmerald Web\0\rFoundation\0\0");
			}
			else
				gstr = XorStr("Select Glove type!\0\0");

			if (ImGui::Combo(XorStr("##2"), &Vars.Skins.GloveSkin, gstr, -1, ImVec2(130, 0)))
				U::CL_FullUpdate();

			if (ImGui::Button(XorStr("Full Update"), ImVec2(93.f, 20.f))) {
				U::CL_FullUpdate();
			}

			//ImGui::SliderFloat(XorStr("Wear"), &Vars.Skins.Weapons[Vars.wpn].Wear, 0.00000001f, 1.f);

			//ImGui::SliderInt(XorStr("StatTrak"), &Vars.Skins.Weapons[Vars.wpn].StatTrk, 0, 99999);
		}
		ImGui::EndChild();
	}








}
void LegitTab()
{
	ImGui::Columns(2, NULL, true, true);
	{
		ImGui::Checkbox(XorStr("Enable Legit Aimbot"), &Vars.Legitbot.Aimbot.Enabled);
		ImGui::Checkbox(XorStr("Ignore Key"), &Vars.Legitbot.Aimbot.AlwaysOn);
		ImGui::Combo(XorStr("Aimbot Key"), &Vars.Legitbot.Aimbot.Key, keyNames, IM_ARRAYSIZE(keyNames));
		//ImGui::KeyBindButton(Vars.Legitbot.Aimbot.Key);
		ImGui::Checkbox(XorStr("Aim on Key"), &Vars.Legitbot.Aimbot.OnKey);
		ImGui::Checkbox(XorStr("Friendly Fire"), &Vars.Legitbot.Aimbot.FriendlyFire);
		ImGui::Checkbox(XorStr("Smoke Check"), &Vars.Legitbot.Aimbot.SmokeCheck);
		ImGui::Checkbox(XorStr("AutoPistol"), &Vars.Legitbot.Aimbot.AutoPistol);
		//ImGui::SliderInt(XorStr("Delay"), &Vars.Legitbot.delay, 1, 100);
	}
	ImGui::NextColumn();
	{
		if (G::LocalPlayer->GetAlive() && G::LocalPlayer->GetWeapon()->IsGun()) {
			ImGui::Text("Current Weapon: %s", G::LocalPlayer->GetWeapon()->GetWeaponName().c_str());
			int curweapon = G::LocalPlayer->GetWeapon()->GetItemDefinitionIndex();
			ImGui::Checkbox(XorStr("pSilent Aim"), &Vars.Legitbot.Weapon[curweapon].pSilent);
			ImGui::Combo(XorStr("Hitbox"), &Vars.Legitbot.Weapon[curweapon].Hitbox, charenc("PELVIS\0\r\0\r\0\rHIP\0\rLOWER SPINE\0\rMIDDLE SPINE\0\rUPPER SPINE\0\rNECK\0\rHEAD\0\rNEAREST\0\0"), -1);
			if (Vars.Legitbot.Weapon[curweapon].pSilent)
				ImGui::SliderFloat(XorStr("pSilent FOV"), &Vars.Legitbot.Weapon[curweapon].PFOV, 0.1f, 3.f, "%.2f");
			ImGui::SliderFloat(XorStr("FOV"), &Vars.Legitbot.Weapon[curweapon].FOV, 0.1f, 50.f, "%.2f");
			if (!Vars.Legitbot.Weapon[curweapon].AdaptiveSmooth)
				ImGui::SliderFloat(XorStr("Smooth"), &Vars.Legitbot.Weapon[curweapon].Speed, 0.1f, 100.f, "%.2f");
			//ImGui::Checkbox(XorStr("Adaptive Smooth"), &Vars.Legitbot.Weapon[curweapon].AdaptiveSmooth);
			if (!G::LocalPlayer->GetWeapon()->IsSniper()) {
				ImGui::SliderFloat(XorStr("RCS Y"), &Vars.Legitbot.Weapon[curweapon].RCSAmountY, 1.f, 100.f, "%.0f", 1.f, ImVec2(80.5f, 30));
				ImGui::SameLine();
				ImGui::SliderFloat(XorStr("RCS X"), &Vars.Legitbot.Weapon[curweapon].RCSAmountX, 1.f, 100.f, "%.0f", 1.f, ImVec2(80.5f, 30));
			}
			//ImGui::Checkbox(XorStr("RCS Enabled"), &Vars.Legitbot.Weapon[curweapon].RCS);

			if (G::LocalPlayer->GetWeapon()->IsSniper()) {
				ImGui::Checkbox(XorStr("FastZoom"), &Vars.Legitbot.fastzoom);
				ImGui::SameLine();
				ImGui::Checkbox(XorStr("Switch wep"), &Vars.Legitbot.fastzoomswitch);
			}
		}
		else
		{
			ImGui::Text(XorStr("Invalid weapon/Isnt Alive"));
		}
	}
	ImGui::Columns(1);
}
//
void TriggerTab()
{
	ImGui::Text(XorStr("Triggerbot"));
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(XorStr("Enable Triggerbot"), &Vars.Legitbot.Triggerbot.Enabled);
		ImGui::Checkbox(XorStr("Auto Fire"), &Vars.Legitbot.Triggerbot.AutoFire);
		ImGui::Checkbox(XorStr("Auto Wall"), &Vars.Legitbot.Triggerbot.AutoWall);
		ImGui::Combo(XorStr("Triggerbot Key"), &Vars.Legitbot.Triggerbot.Key, keyNames, IM_ARRAYSIZE(keyNames));
		ImGui::Checkbox(XorStr("Hit Chance"), &Vars.Legitbot.Triggerbot.HitChance);
		ImGui::SliderFloat(XorStr("Amount"), &Vars.Legitbot.Triggerbot.HitChanceAmt, 1.f, 100.f, "%.0f");
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XorStr("Filter"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 19 * 6));
		{
			ImGui::Selectable(XorStr("Trigger When On Head"), &Vars.Legitbot.Triggerbot.Filter.Head);
			ImGui::Selectable(XorStr("Trigger When On Chest"), &Vars.Legitbot.Triggerbot.Filter.Chest);
			ImGui::Selectable(XorStr("Trigger When On Stomach"), &Vars.Legitbot.Triggerbot.Filter.Stomach);
			ImGui::Selectable(XorStr("Trigger When On Arms"), &Vars.Legitbot.Triggerbot.Filter.Arms);
			ImGui::Selectable(XorStr("Trigger When On Legs"), &Vars.Legitbot.Triggerbot.Filter.Legs);
			ImGui::Selectable(XorStr("Trigger On Teammates"), &Vars.Legitbot.Triggerbot.Filter.Friendly);
		}
		ImGui::EndChild();
	}
	ImGui::Columns(1);
	ImGui::Separator();
}
//
void RageTab()
 {
	static int page = 0;
	const char* tabNames[] = {
		"Aim" , "AntiAim" , "AntiAim Misc" };

	static int tabOrder[] = { 0 , 1 , 2 };
	static int tabSelected = 0;
	const bool tabChanged = ImGui::TabLabels(tabNames,
		sizeof(tabNames) / sizeof(tabNames[0]),
		tabSelected, tabOrder);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	float SpaceLineOne = 120.f;
	float SpaceLineTwo = 220.f;
	float SpaceLineThr = 320.f;

	if (tabSelected == 0) // Aim
	{
		ImGui::Columns(2, NULL, true, true);
		{
			//ImGui::Text(XorStr("-AimBot"));
			//ImGui::Separator();
		
			
			ImGui::Checkbox(XorStr("Ragebot Enabled"), &Vars.Ragebot.Enabled);
			ImGui::Checkbox(XorStr("On Key"), &Vars.Ragebot.Hold);
			ImGui::Checkbox(XorStr("Auto Wall"), &Vars.Ragebot.AutoWall);
			ImGui::SliderFloat(XorStr("Min Damage"), &Vars.Ragebot.AutoWallDmg, 0.1f, 120.f, "%.1f");
			if (Vars.Ragebot.Hold)
				ImGui::Combo(XorStr("Key"), &Vars.Ragebot.HoldKey, keyNames, IM_ARRAYSIZE(keyNames));
			ImGui::Checkbox(XorStr("Auto Fire"), &Vars.Ragebot.AutoFire);
			ImGui::Checkbox(XorStr("AutoPistol"), &Vars.Ragebot.AutoPistol);
			ImGui::SliderFloat(XorStr("FOV"), &Vars.Ragebot.FOV, 1.f, 180.f, "%.0f");
			//ImGui::Checkbox(XorStr("Aim Step"), &Vars.Ragebot.Aimstep);
			//ImGui::SliderFloat(XorStr("##AnglePer1Tick"), &Vars.Ragebot.AimstepAmount, 1.f, 180.f, "%.0f");
			//ImGui::Separator();
			ImGui::Text(XorStr("-Enable on VAC servers"));
			ImGui::SameLine();
			ImGui::Separator();
			ImGui::Checkbox(XorStr("Anti-Untrusted"), &Vars.Ragebot.UntrustedCheck);
		}
		ImGui::NextColumn();
		{
			ImGui::Text(XorStr("Resolver"));
			ImGui::Combo(XorStr("##RESOLV"), &Vars.Ragebot.Antiaim.Resolver, XorStr("Disabled\0\rMM\0\rMM Delta\0\0"), -1);
			ImGui::Text(XorStr("Position Adjusment"));
			ImGui::Combo(XorStr("##POSADJUST"), &Vars.Ragebot.ResolverHelper, XorStr("No\0\rLow(Pitch Standalone)\0\rMedium(Pitch Extended)\0\rHigh(Accuracy Boost)\0\0"), -1);
			ImGui::Checkbox(XorStr("Auto Stop"), &Vars.Ragebot.AutoStop);
			ImGui::SameLine();

			ImGui::Checkbox(XorStr("Auto Crouch"), &Vars.Ragebot.AutoCrouch);
			ImGui::Checkbox(XorStr("Auto Scope"), &Vars.Ragebot.AutoScope);
			ImGui::Spacing();
			ImGui::Text(XorStr("RCS"));
			ImGui::Combo(XorStr("##RCSTYPE"), &Vars.Ragebot.RCS, XorStr("Off\0\rpRCS\0\rMaximum\0\0"), -1, ImVec2(85, 0));
			
			if (Vars.Ragebot.RCS == 2) {
				ImGui::SameLine();
				ImGui::SliderFloat(XorStr("##RCSForce"), &Vars.Ragebot.RCShow, 1.f, 100.f, "%.0f");
			}
			ImGui::Checkbox(XorStr("Hit Chance"), &Vars.Ragebot.HitChance);
			ImGui::SameLine();
			ImGui::SliderFloat(XorStr("##HCAmount"), &Vars.Ragebot.HitChanceAmt, 1.f, 100.f, "%.1f");
			ImGui::Checkbox(XorStr("Silent"), &Vars.Ragebot.Silent);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("pSilent"), &Vars.Ragebot.pSilent);
		}
		ImGui::Columns(1);
	}
	else if (tabSelected == 1) // HitBox
	{

		const char* tabs[] = {
			"Target",
			"Target Adjust",

		};

		for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
		{
			int distance = i == page ? 0 : i > page ? i - page : page - i;

			if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0)))
				page = i;



			if (i < IM_ARRAYSIZE(tabs) - 1)
				ImGui::SameLine();
		}

		ImGui::Separator();

		switch (page)
		{
		case 0:
			ImGui::Checkbox(XorStr("Enabled"), &Vars.Ragebot.Antiaim.Enabled);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("Use Choked Yaw"), &Vars.Ragebot.Antiaim.FakeYaw);
			ImGui::Checkbox(XorStr("At Target"), &Vars.Ragebot.Antiaim.AtPlayer);
			//ImGui::SameLine();
			//ImGui::Checkbox(XorStr("Shuffle(TEST)"), &Vars.Ragebot.Antiaim.Shuffle);
			//ImGui::SliderInt(XorStr("##SpinSpeed"), &Vars.Ragebot.Antiaim.Zeta, 1, 4);
			ImGui::Combo(XorStr("Pitch"), &Vars.Ragebot.Antiaim.Pitch, charenc("Unselected\0\rUP\0\rDOWN\0\rJITTER\0\rZERO\0\rEmotion\0\rDOWN JITTER\0\rDOWN2\0\0"), -1, ImVec2(110.f, 20.f));
			ImGui::Combo(XorStr("Yaw"), &Vars.Ragebot.Antiaim.YawReal, charenc("Unselected\0\rSPIN\0\rDestroy LBY\0\rSIDEWAYS1\0\rSIDEWAYS2\0\rBACKWARDS\0\rLEFT\0\rRIGHT\0\rZERO\0\rFAKESPIN\0\rLowerBody Based\0\rLag FAKESPIN\0\rMEMESPIN\0\rFAKE 3\0\0"), -1, ImVec2(110.f, 20.f));
			ImGui::Text(XorStr("Yaw Choke"));
			ImGui::Combo(XorStr("##Choke"), &Vars.Ragebot.Antiaim.YawFake, charenc("Unselected\0\rSPIN\0\rJITTER\0\rSIDEWAYS1\0\rSIDEWAYS2\0\rBACKWARDS\0\rLEFT\0\rRIGHT\0\rZERO\0\rFAKESPIN\0\rLowerBody Based\0\rLag FAKESPIN\0\rMEMESPIN\0\rFAKE 3\0\0"), -1, ImVec2(110.f, 20.f));
			break;
		case 1:
			ImGui::Text(XorStr("Disable AntiAim When"));

			ImGui::Checkbox(XorStr("Knife In Hands"), &Vars.Ragebot.Antiaim.knife_held);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(XorStr("Stops your antiaim while you have your knife out."));
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("No Enemy Visible"), &Vars.Ragebot.Antiaim.no_enemy);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(XorStr("Stops your antiaim when there are no enemies visible."));
			ImGui::Text(XorStr("HitScan Bones"));
			ImGui::Selectable(XorStr("Head"), &Vars.Ragebot.bones[HITBOX_HEAD], 0, ImVec2(93.f, 20.f));
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(XorStr("HitScan head"));
			ImGui::SameLine();
			ImGui::Selectable(XorStr("Neck"), &Vars.Ragebot.bones[HITBOX_NECK], 0, ImVec2(93.f, 20.f));
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(XorStr("HitScan neck"));
			ImGui::Selectable(XorStr("Pelvis"), &Vars.Ragebot.bones[HITBOX_PELVIS], 0, ImVec2(93.f, 20.f));
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(XorStr("HitScan pelvis"));
			ImGui::SameLine();
			ImGui::Selectable(XorStr("Spine"), &Vars.Ragebot.bones[HITBOX_SPINE], 0, ImVec2(93.f, 20.f));
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(XorStr("HitScan spine"));
			ImGui::Selectable(XorStr("Legs"), &Vars.Ragebot.bones[HITBOX_LEGS], 0, ImVec2(93.f, 20.f));
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(XorStr("HitScan legs"));
			ImGui::SameLine();
			ImGui::Selectable(XorStr("Arms"), &Vars.Ragebot.bones[HITBOX_ARMS], 0, ImVec2(93.f, 20.f));
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(XorStr("HitScan arms"));
			break;
		}
	}
	else if (tabSelected == 2) // AntiAim
	{
		ImGui::Checkbox(XorStr("Enabled"), &Vars.Ragebot.Antiaim.Enabled);
		ImGui::SameLine();
		ImGui::SliderInt(XorStr("Spin Speed"), &Vars.Ragebot.Antiaim.spinspeed, 1, 1000);
		ImGui::SliderInt(XorStr("Speed on Fake"), &Vars.Ragebot.Antiaim.spinspeedz, 1, 1000);
		ImGui::Checkbox(XorStr("Use Choked Yaw"), &Vars.Ragebot.Antiaim.FakeYaw);
		ImGui::Checkbox(XorStr("At Target"), &Vars.Ragebot.Antiaim.AtPlayer);
		//ImGui::SameLine();
		ImGui::Checkbox(XorStr("Shuffle(TEST)"), &Vars.Ragebot.Antiaim.Shuffle);
		ImGui::SliderInt(XorStr("##SpinSpeed"), &Vars.Ragebot.Antiaim.Zeta, 1, 4);
		bool unt = Vars.Ragebot.UntrustedCheck;
		ImGui::Text(XorStr("Custom AA Adjustment"));
		ImGui::Checkbox(XorStr("Custom Pitch"), &Vars.Ragebot.Antiaim.PitchCustom); ImGui::SameLine(); ImGui::Checkbox(XorStr("Custom Yaw"), &Vars.Ragebot.Antiaim.YawCustom);
		ImGui::Text(XorStr("Real Pitch")); ImGui::SameLine();
		ImGui::SliderFloat(XorStr("##REALPITCH"), &Vars.Ragebot.Antiaim.PCReal, unt ? -89 : -180, unt ? 89 : 180);
		ImGui::Text(XorStr("Fake Pitch")); ImGui::SameLine();
		ImGui::SliderFloat(XorStr("##FAKEPITCH"), &Vars.Ragebot.Antiaim.PCFake, unt ? -89 : -180, unt ? 89 : 180);
		ImGui::Text(XorStr("Real Yaw")); ImGui::SameLine();
		ImGui::SliderFloat(XorStr("##RealYaw"), &Vars.Ragebot.Antiaim.YCReal, unt ? -179 : -360, unt ? 179 : 360);
		ImGui::Text(XorStr("Fake Yaw")); ImGui::SameLine();
		ImGui::SliderFloat(XorStr("##FakeYaw"), &Vars.Ragebot.Antiaim.YCFake, unt ? -179 : -360, unt ? 179 : 360);


		
	}
		

	
}













EndSceneFn oEndScene;
long __stdcall Hooks::EndScene(IDirect3DDevice9* pDevice)
{
	if (!G::d3dinit)
		GUI_Init(pDevice);

	H::D3D9->ReHook();

	ImGui::GetIO().MouseDrawCursor = Vars.Menu.Opened;

	ImGui_ImplDX9_NewFrame();

	if (Vars.Visuals.SpectatorList)
		E::Visuals->SpecList();

	if (Vars.Visuals.Radar.Enabled && I::Engine->IsConnected())
		DrawRadar();

	if (Vars.Menu.Opened)
	{
		I::Engine->GetScreenSize(width, height);
		
		if (1 == 1) {
			if (connva < 1.f) {
				R3animate = 0.f;
				Sleep(sleepanimate);
				connva = connva + periodicity1;
			}
			if (connva < 1.00f) {
				Sleep(sleepanimate);
				connva = connva + periodicity1;
			}

			if (connva >= 1.00f) {
				if (R3animate < 264.f) {
					Sleep(sleepanimate2);
					R3animate = R3animate + periodicity;
				}
			}
			if (buttonmenu > 0) {

				if (penis < 675) {
					Sleep(sleepanimate2);
					penis = penis + periodicity;
				}
			}
			if (buttonmenu == 0) {
				penis = 180;
			}
			width1 = (width / 2);
			height2 = (height / 2);

			int width2 = width1 - 90;
			int height3 = height2 - 152;
			if (buttonmenu > 0) {
				if (penis >= 675) {
					pizda = 247;
				}
			}


			ImGui::SetNextWindowPos(ImVec2(width2 - pizda, height3));
			ImGui::SetNextWindowSize(ImVec2(penis, 40.f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleColor(ImGuiCol_Button, ImColor(24, 24, 24, 240));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(24, 24, 24, 240));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(24, 24, 24, 240));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(24, 24, 24, 240));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

			
			

			if (ImGui::Begin("RXCHEATS", &Vars.Menu.Opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
				
				ImGui::Button("PICOLOWARE", ImVec2(160, 40));
		
			
				ImGui::End();
			}
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();


			ImGui::SetNextWindowSize(ImVec2(180.f, R3animate));
			ImGui::SetNextWindowPos(ImVec2(width2 - pizda, height3 + 40));

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
			ImColor mainColor = ImColor(54, 54, 54, 255);
			ImColor bodyColor = ImColor(24, 24, 24, 240);
			ImColor fontColor = ImColor(255, 255, 255, 255);


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.00f, 0.29f, 0.00f, 0.68f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(mainColor.Value.x + 0.2f, mainColor.Value.y + 0.2f, mainColor.Value.z + 0.2f, mainColor.Value.w));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(mainColor.Value.x + 0.1f, mainColor.Value.y + 0.1f, mainColor.Value.z + 0.1f, mainColor.Value.w));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(24, 24, 24, 240));
			if (ImGui::Begin("RXCHEATSfj1dfg", &Vars.Menu.Opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {

				if (ImGui::Button("LEGITBOT", ImVec2(179, 31))) {
					buttonmenu2(1);
				}
				if (ImGui::Button("RAGEBOT", ImVec2(179, 31))) {//2
					buttonmenu2(9);
				}
				if (ImGui::Button("TRIGGERBOT", ImVec2(179, 31))) {//7
					buttonmenu2(17);
				}
				if (ImGui::Button("ESP", ImVec2(179, 31))) {//6
					buttonmenu2(5);
				}

				if (ImGui::Button("MISC", ImVec2(179, 31))) {//7
					buttonmenu2(6);
				}
				if (ImGui::Button("RADAR", ImVec2(179, 31))) {//7
					buttonmenu2(11);
				}
				if (ImGui::Button("SKINCHANGER", ImVec2(179, 31))) {//7
					buttonmenu2(12);
				}



				ImGui::End();
			}
			if (buttonmenu > 0) {
				ImGui::SetNextWindowPos(ImVec2((width2 + 185) - pizda, height3 + 45));
				ImGui::SetNextWindowSize(ImVec2(490.f, 259.f));
				if (ImGui::Begin("RXCHEATàïïfgskjdfsdf", &Vars.Menu.Opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {

					if (buttonmenu == 17) {
						{
							ImVec2 siz = ImVec2(185, 370 - ImGui::GetCursorPosY() - 40);

							ImVec2 size = ImVec2(290, 372 - ImGui::GetCursorPosY() - 40);
							{
								ImGui::Checkbox(XorStr("Enable Triggerbot"), &Vars.Legitbot.Triggerbot.Enabled);
								ImGui::SameLine();
								ImGui::Checkbox(XorStr("Ignore Key"), &Vars.Legitbot.Triggerbot.AutoFire);
								if (Vars.Legitbot.Triggerbot.AutoFire == false)
									ImGui::Combo(XorStr("Key"), &Vars.Legitbot.Triggerbot.Key, keyNames, IM_ARRAYSIZE(keyNames));
								ImGui::Checkbox(XorStr("Auto Wall"), &Vars.Legitbot.Triggerbot.AutoWall);
								ImGui::SameLine();
								ImGui::Checkbox(XorStr("Hitchance"), &Vars.Legitbot.Triggerbot.HitChance);

								ImGui::SliderFloat(XorStr("Amount"), &Vars.Legitbot.Triggerbot.HitChanceAmt, 1.f, 100.f, "%.0f");
								ImGui::SliderInt(XorStr("Delay"), &Vars.Legitbot.Triggerbot.Delay, 1, 1000);
								ImGui::Text("");
							}
							ImGui::NextColumn();
							{
								ImGui::BeginChild(XorStr("Filter"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 19 * 6));
								{
									ImGui::Selectable(XorStr("Trigger When On Head"), &Vars.Legitbot.Triggerbot.Filter.Head);
									ImGui::Selectable(XorStr("Trigger When On Chest"), &Vars.Legitbot.Triggerbot.Filter.Chest);
									ImGui::Selectable(XorStr("Trigger When On Stomach"), &Vars.Legitbot.Triggerbot.Filter.Stomach);
									ImGui::Selectable(XorStr("Trigger When On Arms"), &Vars.Legitbot.Triggerbot.Filter.Arms);
									ImGui::Selectable(XorStr("Trigger When On Legs"), &Vars.Legitbot.Triggerbot.Filter.Legs);
									ImGui::Selectable(XorStr("Trigger On Teammates"), &Vars.Legitbot.Triggerbot.Filter.Friendly);
								}
								ImGui::EndChild();
							}
							ImGui::Columns(1);
						}
					}
				}

				if (buttonmenu == 11) {
					{
						ImGui::Checkbox(XorStr("Enable Radar"), &Vars.Visuals.Radar.Enabled);
						ImGui::SliderInt(XorStr("Radar Range"), &Vars.Visuals.Radar.range, 1, 100);
						ImGui::Checkbox(XorStr("Only Enemy"), &Vars.Visuals.Radar.EnemyOnly);
						ImGui::Checkbox(XorStr("Show Nicknames"), &Vars.Visuals.Radar.Nicks);
					}

				}
				if (buttonmenu == 12) {
					{
						ImGui::Checkbox(XorStr("Enable Skins"), &Vars.Skins.SSEnabled);


						if (Vars.Skins.SSEnabled)
						{
							ImGui::BeginChild(XorStr("##SKINS"), ImVec2(0, 0), true);
							{
								ImGui::PushItemWidth(270);
								if (G::LocalPlayer->GetAlive())
									ImGui::Text(XorStr("Current Weapon: %s"), G::LocalPlayer->GetWeapon()->GetWeaponName().c_str());
								else
									ImGui::Text(XorStr("Invalid weapon/Isnt Alive"));


								ImGui::Combo(XorStr("Paint Kit"), &Vars.Skins.Weapons[Vars.wpn].PaintKit, [](void* data, int idx, const char** out_text)
								{
									*out_text = k_Skins[idx].strName.c_str();
									return true;
								}, nullptr, k_Skins.size(), 10);
								//static int pkit = 0;
								/*ImGui::Combo(XorStr("glove Kit"), &pkit, [](void* data, int idx, const char** out_text)
								{
								*out_text = k_Gloves[idx].strName.c_str();
								return true;
								}, nullptr, k_Gloves.size(), 10);*/
								//k_Skins[Vars.Skins.Weapons[pWeapon->GetItemDefinitionIndex()].PaintKit].iIndex;
								//ImGui::Text("%d", k_Gloves[pkit].iIndex);


								ImGui::Separator();
								//ImGui::Combo(XorStr("Knife Skin"), &Vars.Skins.KnifeSkin, KnifeSkin, IM_ARRAYSIZE(KnifeSkin));
								ImGui::Combo(XorStr("Knife Model"), &Vars.Skins.KnifeModel, charenc("No Knife\0\rBayonet\0\rFlip Knife\0\rGut Knife\0\rKarambit\0\rM9 Bayonet\0\rHuntsman Knife\0\rButterfly Knife\0\rFalchion Knife\0\rShadow Daggers\0\rBowie Knife\0\0"), -1, ImVec2(130, 0));
								ImGui::Separator();

								if (ImGui::Combo(XorStr("Glove Model"), &Vars.Skins.Glove, XorStr("No Glove\0\rBloodhound\0\rSport\0\rDriver\0\rWraps\0\rMoto\0\rSpecialist\0\0"), -1, ImVec2(130, 0)))
									U::CL_FullUpdate();

								const char* gstr;
								if (Vars.Skins.Glove == 1)
								{
									gstr = XorStr("Charred\0\rSnakebite\0\rBronzed\0\rGuerilla\0\0");
								}
								else if (Vars.Skins.Glove == 2)
								{
									gstr = XorStr("Hedge Maze\0\rPandoras Box\0\rSuperconductor\0\rArid\0\0");
								}
								else if (Vars.Skins.Glove == 3)
								{
									gstr = XorStr("Lunar Weave\0\rConvoy\0\rCrimson Weave\0\rDiamondback\0\0");
								}
								else if (Vars.Skins.Glove == 4)
								{
									gstr = XorStr("Leather\0\rSpruce DDPAT\0\rSlaughter\0\rBadlands\0\0");
								}
								else if (Vars.Skins.Glove == 5)
								{
									gstr = XorStr("Eclipse\0\rSpearmint\0\rBoom!\0\rCool Mint\0\0");
								}
								else if (Vars.Skins.Glove == 6)
								{
									gstr = XorStr("Forest DDPAT\0\rCrimson Kimono\0\rEmerald Web\0\rFoundation\0\0");
								}
								else
									gstr = XorStr("Select Glove type!\0\0");

								if (ImGui::Combo(XorStr("##2"), &Vars.Skins.GloveSkin, gstr, -1, ImVec2(130, 0)))
									U::CL_FullUpdate();

								if (ImGui::Button(XorStr("Full Update"), ImVec2(93.f, 20.f))) {
									U::CL_FullUpdate();
								}

								//ImGui::SliderFloat(XorStr("Wear"), &Vars.Skins.Weapons[Vars.wpn].Wear, 0.00000001f, 1.f);

								//ImGui::SliderInt(XorStr("StatTrak"), &Vars.Skins.Weapons[Vars.wpn].StatTrk, 0, 99999);
							}
							ImGui::EndChild();
						}
					}

				}
				if (buttonmenu == 9) {
					{
						ImVec2 siz = ImVec2(185, 350 - ImGui::GetCursorPosY() - 40);
						ImVec2 size = ImVec2(240, 290 - ImGui::GetCursorPosY() - 40);
						const char* tabNames[] = {
							"Aim" , "AntiAim" , "AntiAim Misc" };

						static int tabOrder[] = { 0 , 1 , 2 };
						static int tabSelected = 0;
						const bool tabChanged = ImGui::TabLabels(tabNames,
							sizeof(tabNames) / sizeof(tabNames[0]),
							tabSelected, tabOrder);

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						float SpaceLineOne = 120.f;
						float SpaceLineTwo = 220.f;
						float SpaceLineThr = 320.f;

						if (tabSelected == 0) // Aim
						{
							ImGui::Checkbox(XorStr("Ragebot Enabled"), &Vars.Ragebot.Enabled);
							ImGui::Checkbox(XorStr("On Key"), &Vars.Ragebot.Hold);
							if (Vars.Ragebot.Hold)
								ImGui::Combo(XorStr("Key"), &Vars.Ragebot.HoldKey, keyNames, IM_ARRAYSIZE(keyNames));
							ImGui::Checkbox(XorStr("Auto Fire"), &Vars.Ragebot.AutoFire);
							ImGui::Checkbox(XorStr("AutoPistol"), &Vars.Ragebot.AutoPistol);
							ImGui::SliderFloat(XorStr("FOV"), &Vars.Ragebot.FOV, 1.f, 180.f, "%.0f");
							//ImGui::Checkbox(XorStr("Aim Step"), &Vars.Ragebot.Aimstep);
							//ImGui::SliderFloat(XorStr("##AnglePer1Tick"), &Vars.Ragebot.AimstepAmount, 1.f, 180.f, "%.0f");
							//ImGui::Separator();
							ImGui::Text(XorStr("-Enable on VAC servers"));
							ImGui::SameLine();
							//ImGui::Separator();
							ImGui::Checkbox(XorStr("Anti-Untrusted"), &Vars.Ragebot.UntrustedCheck);
								ImGui::Combo(XorStr("Key"), &Vars.Ragebot.HoldKey, keyNames, IM_ARRAYSIZE(keyNames));
							ImGui::Checkbox(XorStr("On Key"), &Vars.Ragebot.Hold);
							ImGui::Checkbox(XorStr("Auto Fire"), &Vars.Ragebot.AutoFire);
							ImGui::SliderFloat(XorStr("FOV"), &Vars.Ragebot.FOV, 1.f, 180.f, "%.0f");
							ImGui::Checkbox(XorStr("Silent"), &Vars.Ragebot.Silent);
							ImGui::Checkbox(XorStr("pSilent"), &Vars.Ragebot.pSilent);
							
							ImGui::SliderFloat(XorStr("NoRecoil Force"), &Vars.Ragebot.RCShow, 1.f, 100.f, "%.0f");
							ImGui::Checkbox(XorStr("Aim Step"), &Vars.Ragebot.Aimstep);
							ImGui::SliderFloat(XorStr("Angle Per Tick"), &Vars.Ragebot.AimstepAmount, 1.f, 180.f, "%.0f");
							ImGui::Text(XorStr("Resolver"));
							ImGui::Combo(XorStr("##RESOLV"), &Vars.Ragebot.Antiaim.Resolver, XorStr("Disabled\0\rMM\0\rMM Delta\0\0"), -1);
							ImGui::Text(XorStr("Position Adjusment"));
							ImGui::Combo(XorStr("##POSADJUST"), &Vars.Ragebot.ResolverHelper, XorStr("No\0\rLow(Pitch Standalone)\0\rMedium(Pitch Extended)\0\rHigh(Accuracy Boost)\0\0"), -1);
							ImGui::Checkbox(XorStr("Auto Stop"), &Vars.Ragebot.AutoStop);
							ImGui::SameLine();

							ImGui::Checkbox(XorStr("Auto Crouch"), &Vars.Ragebot.AutoCrouch);
							ImGui::Checkbox(XorStr("Auto Scope"), &Vars.Ragebot.AutoScope);
				
							ImGui::Text(XorStr("RCS"));
							ImGui::Combo(XorStr("##RCSTYPE"), &Vars.Ragebot.RCS, XorStr("Off\0\rpRCS\0\rMaximum\0\0"), -1, ImVec2(85, 0));

							if (Vars.Ragebot.RCS == 2) {
								ImGui::SameLine();
								ImGui::SliderFloat(XorStr("##RCSForce"), &Vars.Ragebot.RCShow, 1.f, 100.f, "%.0f");
							}
							ImGui::Checkbox(XorStr("Hit Chance"), &Vars.Ragebot.HitChance);
							ImGui::SameLine();
							ImGui::SliderFloat(XorStr("##HCAmount"), &Vars.Ragebot.HitChanceAmt, 1.f, 100.f, "%.1f");
							ImGui::Checkbox(XorStr("Silent"), &Vars.Ragebot.Silent);
							ImGui::SameLine();
							ImGui::Checkbox(XorStr("pSilent"), &Vars.Ragebot.pSilent);
							ImGui::Combo(XorStr("Hitbox"), &Vars.Ragebot.Hitbox, XorStr("PELVIS\0\r\0\r\0\rHIP\0\rLOWER SPINE\0\rMIDDLE SPINE\0\rUPPER SPINE\0\rNECK\0\rHEAD\0\0"), -1, ImVec2(100, 0));//XorStr("PELVIS\0\r\0\r\0\rHIP\0\rLOWER SPINE\0\rMIDDLE SPINE\0\rUPPER SPINE\0\rNECK\0\rHEAD\0\rNEAREST\0\0")
							ImGui::Checkbox(XorStr("Friendly Fire"), &Vars.Ragebot.FriendlyFire);
							ImGui::SameLine();
							ImGui::Checkbox(XorStr("Auto Wall"), &Vars.Ragebot.AutoWall);
							ImGui::Text(XorStr("Min Damage"));
							ImGui::SameLine();
							ImGui::SliderFloat(XorStr("##MinDamage"), &Vars.Ragebot.AutoWallDmg, 0.1f, 120.f, "%.1f");
							ImGui::Combo(XorStr("HitScan"), &Vars.Ragebot.HitScan, charenc("Off\0\rSelected\0\0"), -1, ImVec2(110.f, 20.f));
							ImGui::SameLine();
							ImGui::Checkbox(XorStr("MultiPoint"), &Vars.Ragebot.MultiPoints);
							ImGui::Text(XorStr("Scale"));
							ImGui::SameLine();
							ImGui::SliderFloat(XorStr("##MultiPointscale"), &Vars.Ragebot.PointScale, 0.f, 1.f, "%.01f");
							
							ImGui::Checkbox(XorStr("Friendly Fire"), &Vars.Ragebot.FriendlyFire);
							ImGui::Checkbox(XorStr("Auto Wall"), &Vars.Ragebot.AutoWall);
							ImGui::SliderFloat(XorStr("Min Damage"), &Vars.Ragebot.AutoWallDmg, 0.1f, 120.f, "%.1f");
							


							ImGui::Checkbox(XorStr("Auto Stop"), &Vars.Ragebot.AutoStop);
							ImGui::Checkbox(XorStr("Auto Scope"), &Vars.Ragebot.AutoScope);
							ImGui::Checkbox(XorStr("AutoStop until can shoot"), &Vars.Ragebot.AutoStopCanShoot);



							ImGui::Checkbox(XorStr("AutoStop fast"), &Vars.Ragebot.AutoStopFast);
							ImGui::Checkbox(XorStr("Shoot When stopped"), &Vars.Ragebot.ShootWhenStopped);
							ImGui::Checkbox(XorStr("Auto Crouch"), &Vars.Ragebot.AutoCrouch);

							ImGui::Columns(1);
							ImGui::Separator();
							ImGui::Checkbox(XorStr("Hit Chance"), &Vars.Ragebot.HitChance);
							ImGui::SliderFloat(XorStr("Amount"), &Vars.Ragebot.HitChanceAmt, 1.f, 100.f, "%.1f");
							ImGui::Separator();
							ImGui::Checkbox(XorStr("Enabled"), &Vars.Ragebot.Antiaim.Enabled);
							ImGui::SameLine();
							ImGui::Checkbox(XorStr("Use Choked Yaw"), &Vars.Ragebot.Antiaim.FakeYaw);
							ImGui::Checkbox(XorStr("At Target"), &Vars.Ragebot.Antiaim.AtPlayer);
							ImGui::SameLine();
							ImGui::Checkbox(XorStr("Shuffle(TEST)"), &Vars.Ragebot.Antiaim.Shuffle);
							ImGui::SliderInt(XorStr("##SpinSpeed"), &Vars.Ragebot.Antiaim.Zeta, 1, 4);
							ImGui::Combo(XorStr("Pitch"), &Vars.Ragebot.Antiaim.Pitch, charenc("Unselected\0\rUP\0\rDOWN\0\rJITTER\0\rZERO\0\rEmotion\0\rDOWN JITTER\0\rDOWN2\0\0"), -1, ImVec2(110.f, 20.f));
							ImGui::Combo(XorStr("Yaw"), &Vars.Ragebot.Antiaim.YawReal, charenc("Unselected\0\rSPIN\0\rDestroy LBY\0\rSIDEWAYS1\0\rSIDEWAYS2\0\rBACKWARDS\0\rLEFT\0\rRIGHT\0\rZERO\0\rFAKESPIN\0\rLowerBody Based\0\rLag FAKESPIN\0\rMEMESPIN\0\rFAKE 3\0\0"), -1, ImVec2(110.f, 20.f));
							ImGui::Text(XorStr("Yaw Choke"));
							ImGui::Combo(XorStr("##Choke"), &Vars.Ragebot.Antiaim.YawFake, charenc("Unselected\0\rSPIN\0\rJITTER\0\rSIDEWAYS1\0\rSIDEWAYS2\0\rBACKWARDS\0\rLEFT\0\rRIGHT\0\rZERO\0\rFAKESPIN\0\rLowerBody Based\0\rLag FAKESPIN\0\rMEMESPIN\0\rFAKE 3\0\0"), -1, ImVec2(110.f, 20.f));


							if (ImGui::TreeNode("Spin Adjust")) {
								ImGui::Text(XorStr("Spin Speed")); ImGui::SameLine();
								ImGui::SliderInt(XorStr("##SpinSpeed"), &Vars.Ragebot.Antiaim.spinspeed, 1, 1000);
								ImGui::Text(XorStr("Speed on Fake")); ImGui::SameLine();
								ImGui::SliderInt(XorStr("##SpinSpeedFake"), &Vars.Ragebot.Antiaim.spinspeedz, 1, 1000);
							}

							bool unt = Vars.Ragebot.UntrustedCheck;
							ImGui::Text(XorStr("Custom AA Adjustment"));
							ImGui::Checkbox(XorStr("Custom Pitch"), &Vars.Ragebot.Antiaim.PitchCustom); ImGui::SameLine(); ImGui::Checkbox(XorStr("Custom Yaw"), &Vars.Ragebot.Antiaim.YawCustom);
							ImGui::Text(XorStr("Real Pitch")); ImGui::SameLine();
							ImGui::SliderFloat(XorStr("##REALPITCH"), &Vars.Ragebot.Antiaim.PCReal, unt ? -89 : -180, unt ? 89 : 180);
							ImGui::Text(XorStr("Fake Pitch")); ImGui::SameLine();
							ImGui::SliderFloat(XorStr("##FAKEPITCH"), &Vars.Ragebot.Antiaim.PCFake, unt ? -89 : -180, unt ? 89 : 180);
							ImGui::Text(XorStr("Real Yaw")); ImGui::SameLine();
							ImGui::SliderFloat(XorStr("##RealYaw"), &Vars.Ragebot.Antiaim.YCReal, unt ? -179 : -360, unt ? 179 : 360);
							ImGui::Text(XorStr("Fake Yaw")); ImGui::SameLine();
							ImGui::SliderFloat(XorStr("##FakeYaw"), &Vars.Ragebot.Antiaim.YCFake, unt ? -179 : -360, unt ? 179 : 360);

							ImGui::Columns(1);
							ImGui::Separator();
							ImGui::Text(XorStr("Disable on..."));
							ImGui::Separator();
							ImGui::Checkbox(XorStr("Knife"), &Vars.Ragebot.Antiaim.knife_held);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(XorStr("Stops your antiaim while you have your knife out."));

							ImGui::Checkbox(XorStr("No Enemy"), &Vars.Ragebot.Antiaim.no_enemy);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(XorStr("Stops your antiaim when there are no enemies visible."));

							ImGui::Separator();
							ImGui::Checkbox(XorStr("Edge"), &Vars.Ragebot.Antiaim.DetectEdge);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(XorStr("If someone edge, you can enable this thing and cheat will find best hitbox to kill him."));
							ImGui::SliderFloat(XorStr("EdgeDistance"), &Vars.Ragebot.Antiaim.EdgeDistance, 10, 30);
							ImGui::Separator();
							ImGui::Text(XorStr("HitScan Bones"));
							ImGui::Separator();

							ImGui::Checkbox(XorStr("Head"), &Vars.Ragebot.bones[HITBOX_HEAD]);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(XorStr("HitScan head"));
							ImGui::Checkbox(XorStr("Neck"), &Vars.Ragebot.bones[HITBOX_NECK]);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(XorStr("HitScan neck"));
							ImGui::Checkbox(XorStr("Pelvis"), &Vars.Ragebot.bones[HITBOX_PELVIS]);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(XorStr("HitScan pelvis"));

							ImGui::Checkbox(XorStr("Spine"), &Vars.Ragebot.bones[HITBOX_SPINE]);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(XorStr("HitScan spine"));
							ImGui::Checkbox(XorStr("Legs"), &Vars.Ragebot.bones[HITBOX_LEGS]);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(XorStr("HitScan legs"));
							ImGui::Checkbox(XorStr("Arms"), &Vars.Ragebot.bones[HITBOX_ARMS]);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(XorStr("HitScan arms"));

							ImGui::Separator();
							ImGui::Checkbox(XorStr("Anti-Untrusted"), &Vars.Ragebot.UntrustedCheck);

						}


					}

				}




				if (buttonmenu == 1) {
					{
						//legitbot
						ImGui::Columns(2, NULL, true, true);
						{
							ImGui::Checkbox(XorStr("Enable"), &Vars.Legitbot.Aimbot.Enabled);
							ImGui::Checkbox(XorStr("Ignore Key"), &Vars.Legitbot.Aimbot.AlwaysOn);
							ImGui::Combo(XorStr("Aimbot Key"), &Vars.Legitbot.Aimbot.Key, keyNames, IM_ARRAYSIZE(keyNames));
							ImGui::Checkbox(XorStr("Aim on Key"), &Vars.Legitbot.Aimbot.OnKey);
							ImGui::Checkbox(XorStr("Friendly Fire"), &Vars.Legitbot.Aimbot.FriendlyFire);
							ImGui::Checkbox(XorStr("Smoke Check"), &Vars.Legitbot.Aimbot.SmokeCheck);
							ImGui::Checkbox(XorStr("AutoPistol"), &Vars.Legitbot.Aimbot.AutoPistol);
							ImGui::SliderInt(XorStr("Delay"), &Vars.Legitbot.delay, 1, 100);
						}
						ImGui::NextColumn();
						{
							if (G::LocalPlayer->GetAlive() && G::LocalPlayer->GetWeapon()->IsGun()) {
								ImGui::Text("Current Weapon: %s", G::LocalPlayer->GetWeapon()->GetWeaponName().c_str());
								int curweapon = G::LocalPlayer->GetWeapon()->GetItemDefinitionIndex();

								ImGui::Combo(XorStr("Hitbox"), &Vars.Legitbot.Weapon[curweapon].Hitbox, charenc("PELVIS\0\r\0\r\0\rHIP\0\rLOWER SPINE\0\rMIDDLE SPINE\0\rUPPER SPINE\0\rNECK\0\rHEAD\0\rNEAREST\0\0"), -1);
								ImGui::Checkbox(XorStr("pSilent Aim"), &Vars.Legitbot.Weapon[curweapon].pSilent);
								ImGui::SliderFloat(XorStr("pSilent FOV"), &Vars.Legitbot.Weapon[curweapon].PFOV, 0.1f, 3.f, "%.2f");
								ImGui::SliderFloat(XorStr("FOV"), &Vars.Legitbot.Weapon[curweapon].FOV, 0.1f, 50.f, "%.2f");
								ImGui::Checkbox(XorStr("Adaptive Smooth"), &Vars.Legitbot.Weapon[curweapon].AdaptiveSmooth);
								if (!Vars.Legitbot.Weapon[curweapon].AdaptiveSmooth)
									ImGui::SliderFloat(XorStr("Smooth"), &Vars.Legitbot.Weapon[curweapon].Speed, 0.1f, 100.f, "%.2f");
								if (!G::LocalPlayer->GetWeapon()->IsSniper()) {
									ImGui::SliderFloat(XorStr("RCS Y"), &Vars.Legitbot.Weapon[curweapon].RCSAmountY, 1.f, 1000.f, "%.0f", 1.f, ImVec2(80.5f, 30));
									ImGui::SameLine();
									ImGui::SliderFloat(XorStr("RCS X"), &Vars.Legitbot.Weapon[curweapon].RCSAmountX, 1.f, 1000.f, "%.0f", 1.f, ImVec2(80.5f, 30));
								}

								if (G::LocalPlayer->GetWeapon()->IsSniper()) {
									ImGui::Checkbox(XorStr("Fast Zoom"), &Vars.Legitbot.fastzoom);
									ImGui::SameLine();
									ImGui::Checkbox(XorStr("Switch weapon"), &Vars.Legitbot.fastzoomswitch);
								}
							}
							else
							{
								ImGui::Text(XorStr("Invalid Weapon/not alive!"));
							}
						}
						ImGui::Columns(1);
					}

				}

				if (buttonmenu == 5)
				{


					ImGui::SetNextWindowSize(ImVec2(720.f, 400.f));
					ImVec2 siz = ImVec2(200, 370 - ImGui::GetCursorPosY() - 40);
					ImVec2 size = ImVec2(290, 372 - ImGui::GetCursorPosY() - 40);
					//
					const char* tabNames[] =
					{
						"Visuals" , "Chams" , "Misc" };

					static int tabOrder[] = { 0 , 1 , 2 };
					static int tabSelected = 0;
					const bool tabChanged = ImGui::TabLabels(tabNames,
						sizeof(tabNames) / sizeof(tabNames[0]),
						tabSelected, tabOrder);

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					float SpaceLineOne = 320.f;
					float SpaceLineTwo = 320.f;
					float SpaceLineThr = 320.f;

					if (tabSelected == 0) // Visuals
					{
						ImGui::Columns(2, NULL, true, true);
						{
							ImGui::Checkbox(XorStr("Enable Visuals"), &Vars.Visuals.Enabled);
						
							ImGui::Checkbox(XorStr("Corner Box"), &Vars.Visuals.Box);
							ImGui::Checkbox(XorStr("3D Box"), &Vars.Visuals.esp3d);
							ImGui::Checkbox(XorStr("Skeleton"), &Vars.Visuals.Skeleton);
							ImGui::Checkbox(XorStr("Enemy View Line"), &Vars.Visuals.BulletTrace);
							ImGui::Checkbox(XorStr("Anti-Screenshot"), &Vars.Visuals.AntiSS);
						}


						ImGui::NextColumn();
						{

							ImGui::Selectable(XorStr(" Show Name"), &Vars.Visuals.InfoName);
							ImGui::Selectable(XorStr(" Show Health"), &Vars.Visuals.InfoHealth);
							ImGui::Selectable(XorStr(" Show Weapon"), &Vars.Visuals.InfoWeapon);
							ImGui::Selectable(XorStr(" Show If Flashed"), &Vars.Visuals.InfoFlashed);

							ImGui::Selectable(XorStr(" Show Enemies"), &Vars.Visuals.Filter.Enemies);
							ImGui::Selectable(XorStr(" Show Teammates"), &Vars.Visuals.Filter.Friendlies);
							ImGui::Selectable(XorStr(" Show Weapons"), &Vars.Visuals.Filter.Weapons);
							ImGui::Selectable(XorStr(" Show Decoy"), &Vars.Visuals.Filter.Decoy);
							ImGui::Selectable(XorStr(" Show C4"), &Vars.Visuals.Filter.C4);

						}

					}
					if (tabSelected == 1) // Chams
					{

						ImGui::Columns(2, NULL, true, true);
						{
							ImGui::Checkbox(XorStr("Chams Enable"), &Vars.Visuals.Chams.Enabled);
							ImGui::SameLine();
							ImGui::Combo(XorStr("Mode"), &Vars.Visuals.Chams.Mode, XorStr("Flat\0\rTextured\0\0"), -1, ImVec2(70, 0));
							ImGui::Checkbox(XorStr("XQZ"), &Vars.Visuals.Chams.XQZ);
							ImGui::Checkbox(XorStr("Hands"), &Vars.Visuals.Chams.hands);
							ImGui::SameLine();
							ImGui::Checkbox(XorStr("Weapon"), &Vars.Visuals.ChamsWeapon);
							ImGui::Checkbox(XorStr("Rainbow Hands"), &Vars.Visuals.Chams.RainbowHands);
							ImGui::Text(XorStr("Speed"));
							ImGui::SameLine();
							ImGui::SliderFloat(XorStr("##CHMSSpeed"), &Vars.Visuals.Chams.RainbowTime, 1.f, 150.f, "%.0f");

							ImGui::Text(XorStr("Removals"));
							ImGui::Checkbox(XorStr("No Visual Recoil"), &Vars.Visuals.RemovalsVisualRecoil);
							ImGui::SameLine();
							ImGui::Checkbox(XorStr("No Flash"), &Vars.Visuals.RemovalsFlash);
							ImGui::Checkbox(XorStr("No Scope"), &Vars.Visuals.NoScope);
						}


						ImGui::NextColumn();
						{
							ImGui::Text(XorStr("Chams Colors"));
							ImGui::ColorEdit3(XorStr("Chams CT Visible"), Vars.g_iChamsCTV);
							ImGui::ColorEdit3(XorStr("Chams CT Hidden"), Vars.g_iChamsCTH);
							ImGui::ColorEdit3(XorStr("Chams T Visible"), Vars.g_iChamsTV);
							ImGui::ColorEdit3(XorStr("Chams T Hidden"), Vars.g_iChamsTH);


						}

					}
					if (tabSelected == 2) // Misc
					{

						ImGui::Columns(2, NULL, true, true);
						{
							ImGui::Text(XorStr("Override Fov"));
							ImGui::SameLine();
							ImGui::SliderInt(XorStr("##OverrideFov"), &Vars.Misc.fov, -70, 70);
							ImGui::Text(XorStr("ViewModel Fov"));
							ImGui::SameLine();
							ImGui::SliderInt(XorStr("##ViewFov"), &Vars.Visuals.ViewmodelFov, -70, 70);
							ImGui::Checkbox(XorStr("ThirdPerson"), &Vars.Visuals.thirdperson.enabled);
							ImGui::Text(XorStr("Distance"));
							ImGui::SameLine();
							ImGui::SliderFloat(XorStr("##THRDPERSDIST"), &Vars.Visuals.thirdperson.distance, 30.f, 200.f, "%.0f");
							ImGui::Checkbox(XorStr("SpectatorList"), &Vars.Visuals.SpectatorList);
							ImGui::Combo(XorStr("Vector to Enemies"), &Vars.Visuals.Line, XorStr("Off\0\rCentered\0\rDown\0\0"), -1, ImVec2(70, 0));
						}


						ImGui::NextColumn();
						{
							ImGui::Checkbox(XorStr("Enable Hitmarker"), &Vars.Visuals.HitMarker);
							ImGui::SliderFloat(XorStr("##HitmarkerSize"), &Vars.Visuals.HitMarkerSize, 10.f, 50.f, "%.0f");
							ImGui::Combo(XorStr("Hitsound"), &Vars.Visuals.HitSound, XorStr("Off\0\rNormal\0\rSkeet\0\rCustom\0\0"), -1, ImVec2(100, 0));
							ImGui::Checkbox(XorStr("Enable LBY STATUS"), &Vars.Visuals.LBYSTATS);
							ImGui::Spacing();
							ImGui::Checkbox(XorStr("Crosshair Enabled"), &Vars.Visuals.CrosshairOn);
							ImGui::Checkbox(XorStr("Show Spread"), &Vars.Visuals.CrosshairSpread);
							ImGui::Checkbox(XorStr("Show FOV"), &Vars.Visuals.CrosshairFOV);
							ImGui::Checkbox(XorStr("Show Recoil"), &Vars.Visuals.CrosshairType);
							ImGui::Checkbox(XorStr("Rainbow Color"), &Vars.Visuals.CrosshairRainbow);
							ImGui::Text(XorStr("Crosshair Style"));
							ImGui::SameLine();
							ImGui::Combo(XorStr("##CRSHRStyle"), &Vars.Visuals.CrosshairStyle, XorStr("Plus\0\rCircle\0\rBox\0\0"), -1, ImVec2(70, 0));

						}
					}
				}





				if (buttonmenu == 6) {
					
					const char* tabNames[] =
					{
						"NameSteal & SpamChat" , "Movement & ClanTag" , "Config" };

					static int tabOrder[] = { 0 , 1 , 2 };
					static int tabSelected = 0;
					const bool tabChanged = ImGui::TabLabels(tabNames,
						sizeof(tabNames) / sizeof(tabNames[0]),
						tabSelected, tabOrder);

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					float SpaceLineOne = 320.f;
					float SpaceLineTwo = 320.f;
					float SpaceLineThr = 320.f;

					if (tabSelected == 0) // NameSteal and SpamChat
					{
						ImGui::Columns(2, NULL, true, true);
						{
							ImGui::Text(XorStr("Message"));
							ImGui::SameLine();
							ImGui::InputText(XorStr("##SpamMessage"), Vars.Misc.ChatSpamMode, 128);
							ImGui::Text(XorStr("Delay"));
							ImGui::SameLine();
							ImGui::SliderFloat(XorStr("##Spam Delay"), &Vars.Misc.ChatSpamDelay, 0.1f, 10.f);
							ImGui::Checkbox(XorStr("ChatSpam"), &Vars.Misc.ChatSpam);
							ImGui::SameLine();
							ImGui::Checkbox(XorStr("Location Spam"), &Vars.Misc.LocSpam);
							ImGui::Checkbox(XorStr("LocSpam Enemies only"), &Vars.Misc.LocSpamEnemies);
							ImGui::SameLine();
							ImGui::Checkbox(XorStr("LocSpam teamchat only"), &Vars.Misc.LocSpamChat);
							ImGui::Text(XorStr("Event Spam"));
							ImGui::Checkbox(XorStr("On Kill"), &Vars.Misc.SoundShit);


						}


						ImGui::NextColumn();
						{

							ImGui::Checkbox(XorStr("Name Steal"), &Vars.Misc.NameSteal);
							ImGui::SameLine();
							ImGui::SliderFloat(XorStr(""), &Vars.Misc.NameStealDelay, 0.1f, 10.f);
							static char nickname[128] = "";
							static char nickseparator[128] = "";
							static bool init_nick = false;
							ImGui::Text(XorStr("Name"));
							ImGui::InputText(XorStr("##NICTEXT"), nickname, 127);
							if (ImGui::Button(XorStr("Set Nickname"), ImVec2(-1, 0)))
							{
								std::string ctWithEscapesProcessed = std::string(nickname);
								U::StdReplaceStr(ctWithEscapesProcessed, "\\n", "\n");

								E::Misc->ChangeName(ctWithEscapesProcessed.c_str());
							}

							if (!init_nick)
							{
								sprintf(nickname, "Picoloware <--");
								init_nick = true;
							}



						}
					}
					 if (tabSelected == 1) // Movement And ClanTag
					{
						 ImGui::Columns(2, NULL, true, true);
						 {

						
						 }
						 ImGui::NextColumn();
						 {
							 ImGui::Text(XorStr("Other"));
							 if (ImGui::Button(XorStr("Unload"), ImVec2(93.f, 20.f)))
							 {
								 E::Misc->Panic();
							 }
							 if (ImGui::Button(XorStr("Font Fix"), ImVec2(93.f, 20.f)))
								 D::SetupFonts();
						 }
						 ImGui::Columns(1);
						

					}
					 if (tabSelected == 2) // Misc
					{

						ImGui::Columns(2, NULL, true, true);
						{

							GetConfigMassive();
						
							ImGui::Text(XorStr("Config Name"));
							
							//configs
							
							static int selectedcfg = 0;
							static std::string cfgname = "default";
							if (ImGui::Combo(XorStr("Configs"), &selectedcfg, [](void* data, int idx, const char** out_text)
							{
								*out_text = configs[idx].c_str();
								return true;
							}, nullptr, configs.size(), 10))
							{
								cfgname = configs[selectedcfg];
								cfgname.erase(cfgname.length() - 4, 4);
								strcpy(Vars.Misc.configname, cfgname.c_str());
							}

							ImGui::InputText(XorStr("Current Config"), Vars.Misc.configname, 128);
							if (ImGui::Button(XorStr("Save Config"), ImVec2(93.f, 20.f))) Config->Save();
							ImGui::SameLine();
							if (ImGui::Button(XorStr("Load Config"), ImVec2(93.f, 20.f))) {
								Config->Load(); color();
							}
							ImGui::Text("");
							ImGui::Text("");
							ImGui::Text("");
							ImGui::Text("");
							ImGui::Text("");
						}
						ImGui::NextColumn();
						{
							ImGui::Text(XorStr("Other"));
							if (ImGui::Button(XorStr("Unload"), ImVec2(93.f, 20.f)))
							{
								E::Misc->Panic();
							}
							if (ImGui::Button(XorStr("Font Fix"), ImVec2(93.f, 20.f)))
								D::SetupFonts();
						}
						ImGui::Columns(1);
					}
					///
					///
				}

				ImGui::End();
			}

		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();


	
	}

	
	
	ImGui::Render();
	return oEndScene(pDevice);
}

ResetFn oReset;
long __stdcall Hooks::Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (!G::d3dinit)
		return oReset(pDevice, pPresentationParameters);

	ImGui_ImplDX9_InvalidateDeviceObjects();

	auto hr = oReset(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();


	return hr;
}

void GUI_Init(IDirect3DDevice9* pDevice)
{
	ImGui_ImplDX9_Init(G::Window, pDevice);

	color();

	G::d3dinit = true;
}

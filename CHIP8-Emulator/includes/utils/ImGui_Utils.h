#pragma once

#include <string>
#include <vector>

namespace ImGui_Utils
{
	void DrawFloatControl(const std::string& label, float& value, float resetValue = 0.0f, float columnWidth = 75.0f);
	void DrawIntControl(const std::string& label, int& value, int resetValue = 0, float columnWidth = 75.0f);
	void DrawBoolControl(const std::string& label, bool& value, float columnWidth = 75.0f);
	void DrawComboBoxControl(const std::string& label, int& selectedItem, const std::vector<const char*>& options, float columnWidth = 75.0f);
	bool DrawButtonControl(const std::string& label, const std::string& buttonLabel = "APPLY", float columnWidth = 75.0f);

	void SliderFloat(const std::string& label, float& value, float min, float max, const std::string& format, float columnWidth);
	void SliderInt(const std::string& label, int& value, int min, int max, const std::string& format, float columnWidth);

	void SetPurpleTheme();
}
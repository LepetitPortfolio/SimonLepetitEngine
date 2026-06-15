#pragma once
#include <string>


struct WindowInfo
{
	std::string Name;
	int Width;
	int Height;
	int PositionX;
	int PositionY;
	bool IsReshapable;

	WindowInfo() = default
	{
		Name = "Basic Engine";
		Width = 800;
		Height = 600;
		PositionX = 300;
		PositionY = 300;
		IsReshapable = true;
	}

	WindowInfo(std::string _Name, int _PositionX, int _PositionY, int _Width, int _Height, bool _IsReshapable)
	{
		Name = _Name;
		Width = _Width;
		Height = _Height;
		PositionX = _PositionX;
		PositionY = _PositionY;
		IsReshapable = _IsReshapable;
	}

	WindowInfo(const WindowInfo& _WindowInfo)
	{
		Name = _WindowInfo.Name;
		Width = _WindowInfo.Width;
		Height = _WindowInfo.Height;
		PositionX = _WindowInfo.PositionX;
		PositionY = _WindowInfo.PositionY;
		IsReshapable = _WindowInfo.IsReshapable;
	}

	void operator=(const WindowInfo& _WindowInfo)
	{
		Name = _WindowInfo.Name;
		Width = _WindowInfo.Width;
		Height = _WindowInfo.Height;
		PositionX = _WindowInfo.PositionX;
		PositionY = _WindowInfo.PositionY;
		IsReshapable = _WindowInfo.IsReshapable;
	}
};

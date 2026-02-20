#pragma once
#include "Window.h"

Window::Window(uint32_t _XSize, uint32_t _YSize, std::string _Name)
{
	m_XSize = _XSize; 
	m_YSize = _YSize;
	m_Name = _Name;
	InitOSWindox();
}

Window::~Window()
{
	DeInitiOSWindow();
}

void Window::Close()
{
	m_WindowShouldRun = false;
}

bool Window::Update()
{
	UpdateOSWindow();
	return m_WindowShouldRun;
}
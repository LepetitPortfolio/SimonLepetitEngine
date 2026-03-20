#pragma once
#include "../../../Platform.h"

#include <memory>
#include <string>

#if PLATFORM_LINUX

std::shared_ptr<Display> OpenDisplay();

std::shared_ptr<_XIM> OpenXim();

Atom GetAtom(const std::string& _Name, bool _OnlyIfExists = false);

#endif
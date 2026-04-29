#pragma once
#include "Error.h"
#include "String.h"

#include <fstream>
#include <vector>

class FileReader
{
public:

	static std::vector<char> ReadBinaryFile(const std::string& _Ffilename);

	static String GetRootFolder();

private:

	static String m_RootFolder;
	
};
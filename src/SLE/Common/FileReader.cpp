#include "FileReader.h"
#include <direct.h>

String FileReader::m_RootFolder;

std::vector<char> FileReader::ReadBinaryFile(const std::string& _Filename)
{
	std::ifstream file(_Filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		Err() << "Failed to open file : " << _Filename << std::endl;
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

String FileReader::GetRootFolder()
{
	if (m_RootFolder.IsEmpty())
	{
		m_RootFolder = String(ENGINE_DIR) + String(ENGINE_DIR);
	}

	return m_RootFolder;
}

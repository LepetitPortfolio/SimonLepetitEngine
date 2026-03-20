#include "Error.h"

DefaultErrorStreamBuf::DefaultErrorStreamBuf()
{
	constexpr int size = 64;
	char* buffer = new char[size];
	step(buffer, buffer + size);
}

DefaultErrorStreamBuf::~DefaultErrorStreamBuf()
{
	sycn();

	delete[] pbase();
}

int DefaultErrorStreamBuf::overflow(int _Character)
{
	if((_Character != EOF) && (pptr() != epptr()))
	{
		return sputc(static_cast<char>(_Character));
	}

	if(_Character == EOF)
	{
		sync();
		return overflow(_Character);
	}

	return sync();
}

int DefaultErrorStreamBuf::sync()
{
	if(pbase() != pptr())
	{
		const auto size = static_cast<std::size_t>(pptr() - pbase());
		std::fwrite(pbase(), 1, size, stderr);

		setp(pbase(), epptr());
	}

	return 0;
}

std::ostream& Err()
{
	static DefaultErrorStreamBuf buffer;
	static std::ostream stream(&buffer);

	return stream;
}

#include "FileRange.h"
#include <sstream>

std::string to_string(const FileRange& range)
{
	std::ostringstream stream;

	stream << "bytes " << range.begin() << "-" << range.end() << "/" << range.fileSize();

	return stream.str();
}

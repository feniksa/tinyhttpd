#pragma once

#include <Poco/Path.h>

struct FileMime
{
	const char* text;
	const char* icon;
	const char* mime;
};

const FileMime* find_mime(const Poco::Path& path);
const FileMime& fileicon(const Poco::Path& file_path);
const char* mime(const Poco::Path& file_path);

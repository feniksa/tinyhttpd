#include "mime.h"

#include <string>
#include <string.h>
#include <Poco/File.h>
#include <algorithm>
#include <unordered_map>

static const FileMime UnknownFileType { "unknown", "f.gif", "application/octet-stream"} ;

static const FileMime Directory       { "dir",     "folder.gif" , "" };
static const FileMime BackDir         { "back",    "back.gif"   , "" };
static const FileMime File            { "file",    "f.gif"      , "" };
static const FileMime Index           { "html",    "index.gif"  , "text/html" };

using FileTypesContainer = std::unordered_map<std::string, FileMime>;

static const FileTypesContainer FileTypes
{
	{"png",  {"image",   "image2.gif", "image/png"  } },
	{"gif",  {"image",   "image1.gif", "image/gif"  } },
	{"jpeg", {"image",   "image2.gif", "image/jpeg" } },
	{"jpg",  {"image",   "image2.gif", "image/jpeg" } },
	{"ico",  {"image",   "image1.gif", "image/x-icon"} },
	{"tiff", {"image",   "image2.gif", "image/tiff" } },
	{"tif",  {"image",   "image2.gif", "image/tiff" } },
	{"bmp",  {"image",   "image2.gif", "image/x-bmp"} },
	{"svg",  {"image",   "image1.gif", "image/svg-xml" } },

	{"txt",  {"text",    "text.gif",   "text/plain" } },
	{"md",   {"text",    "text.gif",   "text/plain" } },
	{"markdown", {"text","text.gif",   "text/plain" } },

	{"html", {"html",    "world1.gif", "text/html"  } },

	{"js",   {"source",  "text.gif",   "application/javascript" } },
	{"css",   {"source", "text.gif",   "text/css" } },
	{"hh",   {"source",  "c.gif",      "text/x-c" } },
	{"h",    {"source",  "c.gif",      "text/x-c" } },
	{"c",    {"source",  "c.gif",      "text/x-c" } },
	{"cc",   {"source",  "c.gif",      "text/x-c++src" } },
	{"cpp",  {"source",  "c.gif",      "text/x-c++src" } },
	{"cxx",  {"source",  "c.gif",      "text/x-c++src" } },
	{"ry",   {"source",  "text.gif",   "text/plain" } },
	{"php",  {"source",  "text.gif",   "text/plain" } },
	{"sql",  {"source",  "text.gif",   "text/plain" } },
	{"patch",{"source",  "patch.gif",  "text/plain"} },

	{"md5",  {"checksum","text.gif",   "application/x-md5"} },
	{"sha1", {"checksum","text.gif",   "application/x-sha1 "} },

	{"json", {"json",    "text.gif",  "application/json"} },
	{"xml",  {"xml",     "text.gif",  "application/xml" } },
	{"xsd",  {"xml",     "text.gif",  "application/xml" } },
	{"xsl",  {"xml",     "text.gif",  "application/xml" } },
	{"dtd",  {"xml dtd", "text.gif",  "application/xml-dtd" } },

	{"htaccess",{"htaccess", "text.gif",  "text/plain" } },

	{"pdf",  {"edoc",    "pdf.gif",    "application/pdf"      } },
	{"epub", {"edoc",    "f.gif",      "application/epub+zip" } },
	{"djvu", {"edoc",    "f.gif",      "image/vnd.djvu"} },
	{"djv",  {"edoc",    "f.gif",      "image/vnd.djvu"} },

	{"avi",  {"video",   "movie.gif",  "video/x-msvideo"} },
	{"mkv",  {"video",   "movie.gif",  "video/x-matroska"} },
	{"mp4",  {"video",   "movie.gif",  "video/mp4" } },
	{"ogg",  {"video",   "movie.gif",  "video/ogg" } },

	{"aac", {"audio",    "sound2.gif", "audio/aac"  } },
	{"mp1", {"audio",    "sound2.gif", "audio/mpeg" } },
	{"mp2", {"audio",    "sound2.gif", "audio/mpeg" } },
	{"mp3", {"audio",    "sound2.gif", "audio/mpeg" } },
	{"mpg", {"audio",    "sound2.gif", "audio/mpeg" } },
	{"mpeg",{"audio",    "sound2.gif", "audio/mpeg" } },
	{"oga", {"audio",    "sound2.gif", "audio/ogg"  } },
	{"ogg", {"audio",    "sound2.gif", "audio/ogg"  } },
	{"wav", {"audio",    "sound1.gif", "audio/wav"  } },

	{"tar",  {"archive", "tar.gif",    "application/x-tar"} },
	{"zip",  {"archive", "compressed.gif", "application/zip"} },
	{"gzip", {"archive", "compressed.gif", "application/x-gzip"} },
	{"gz",   {"archive", "compressed.gif", "application/x-gzip"} },
	{"bz2",  {"archive", "compressed.gif", "application/x-bzip2"} },
	{"epk",  {"archive", "compressed.gif", "application/octet-stream"} },
};

const FileMime* find_mime(const Poco::Path& path)
{
	std::string extension = path.getExtension();
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	FileTypesContainer::const_iterator i = FileTypes.find(extension);

	if (i != FileTypes.end())
		return &i->second;

	return nullptr;
}

const FileMime& fileicon(const Poco::Path& path)
{
	if (path.toString() == "..")
		return BackDir;

	Poco::File file(path);
	if (file.isDirectory())
		return Directory;

	if (path.getFileName() == "index.html")
		return Index;

	const FileMime* mime = find_mime(path);
	if (mime)
		return *mime;
	else
		return File;
}

const char* mime(const Poco::Path& path)
{
	const FileMime* mime = find_mime(path);
	if (mime) {
		return mime->mime;
	} else {
		return UnknownFileType.mime;
	}
}

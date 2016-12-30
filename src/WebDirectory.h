#pragma once

#include <string>
#include <vector>

struct WebDirectory
{
	std::string m_path;
	std::string m_alias;
	bool m_followSymLink = false;
	bool m_showContent = false;

	bool load(const std::string& path);
	bool validate() const;
};

using WebDirectories = std::vector<WebDirectory>;

bool loadWebDirectories(const std::string& path, WebDirectories& dirs);

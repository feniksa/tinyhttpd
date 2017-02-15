#pragma once

#include <string>
#include <boost/property_tree/ptree.hpp>

class Configuration
{
public:
	bool load(const char* filePath);
	bool get(const char* key, std::string* value) const;
	void getRequired(const char* key, std::string* value) const;

private:
	boost::property_tree::ptree m_tree;
};

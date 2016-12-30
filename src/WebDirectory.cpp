#include "WebDirectory.h"
#include <Poco/Util/Application.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

bool WebDirectory::load(const std::string& filename)
{
	Poco::Util::Application &app = Poco::Util::Application::instance();

	boost::property_tree::ptree tree;

	if (!boost::filesystem::exists(filename)) {
		app.logger().warning("configuration file \"" + filename + "\" not found");
		return false;
	}

	if (!boost::filesystem::is_regular(filename)) {
		app.logger().warning("configuration file \"" + filename + "\" is not regular file");
		return false;
	}

	// Parse the XML into the property tree.
	boost::property_tree::read_xml(filename, tree);

	m_path = tree.get<std::string>("directory.path");
	m_followSymLink = tree.get<bool>("directory.followSymLinks");
	m_showContent = tree.get<bool>("directory.indexes");


	/*boost::property_tree::ptree::const_assoc_iterator it = tree.find("directory.alias");
	if( it != tree.not_found() )
	{
		it->get_value<std::string>();
		//m_alias = tree.get<std::string>("directory.alias", "/");
	}*/

	boost::optional< boost::property_tree::ptree& > child = tree.get_child_optional( "directory.alias" );
	if(child) {
		m_alias = child->get_value<std::string>();
	}

	return true;
}

bool WebDirectory::validate() const
{
	if (!boost::filesystem::exists(m_path)) {
		Poco::Util::Application &app = Poco::Util::Application::instance();
		app.logger().warning("Document directory: \"" + m_path + "\" doesn't exists");
		return false;
	}

	if (!boost::filesystem::is_directory(m_path)) {
		Poco::Util::Application &app = Poco::Util::Application::instance();
		app.logger().warning("Document path: \"" + m_path + "\" is not directory");
		return false;
	}

	return true;
}

bool loadWebDirectories(const std::string& directory, WebDirectories& dirs)
{
	Poco::Util::Application &app = Poco::Util::Application::instance();

	if (!boost::filesystem::exists(directory)) {
		app.logger().warning("Web Hosts configuration directory \"" + directory + "\" not found");
		return false;
	}

	if (!boost::filesystem::is_directory(directory)) {
		app.logger().warning("Web Hosts configuration directory \"" + directory + "\" is not directory");
		return false;
	}

	boost::filesystem::directory_iterator it(directory), eod;

	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(it, eod))
	{
		if(!boost::filesystem::is_regular_file(p)) {
			app.logger().warning("File \"" + p.string() +"\" is not regular file. Ignore it");
			continue;
		}

		if (boost::filesystem::extension(p) == "xml") {
			app.logger().warning("File \""+ p.string() +"\" is not xml file. Ignore it");
			continue;
		}

		WebDirectory directory;
		if (!directory.load(p.string())) {
			app.logger().warning("Can't load file \""+ p.relative_path().string() +"\". Ignore it");
			continue;
		}

		if (!directory.validate()) {
			app.logger().warning("Can't validate file \""+ p.relative_path().string() +"\". Ignore it");
			continue;
		}

		dirs.push_back(directory);

	} // end foreach

	return true;
}


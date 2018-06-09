#include <fstream>
#include <iostream>
#include <regex>
#include <helpers/Configuration.h>

#include "Log.h"
#include "helpers/Configuration.h"

namespace fs = std::filesystem;
using namespace helpers::configuration;

bool Configuration::read(const std::filesystem::path &path)
{
    if (!fs::exists(path))
    {
        return false;
    }
    _file_map.clear();
    std::string current_section = "default";
    _file_map[current_section] = SectionMap();
    std::fstream fin(path);
    std::regex comment_exp("^\\s*#", std::regex::ECMAScript|std::regex::optimize);
    std::regex section_exp("^\\s*\\[\\s*(\\w.*?)\\s*]", std::regex::ECMAScript|std::regex::optimize);
    std::regex key_value_exp("^\\s*(.+?)\\s*=\\s*(\\w.*?)\\s*$", std::regex::ECMAScript|std::regex::optimize);
    LOG_D("Parsing configuration %s...", path.c_str())
    for (std::string line; std::getline(fin, line);)
    {
        if (std::regex_search(line, comment_exp))
        {
            LOG_D("  ## Comment: \"%s\"", line.c_str())
            continue;
        }
        std::smatch section_match;
        if (std::regex_search(line, section_match, section_exp))
        {
            current_section = std::string {section_match[1].first, section_match[1].second};
            if (_file_map.count(current_section) == 0)
            {
                _file_map[current_section] = SectionMap();
            }
            LOG_D("  ## Section: \"%s\"", current_section.c_str())
            continue;
        }
        std::smatch key_value_match;
        if (std::regex_search(line, key_value_match, key_value_exp))
        {
            std::string key {key_value_match[1].first, key_value_match[1].second};
            std::string value {key_value_match[2].first, key_value_match[2].second};
            _file_map[current_section][key] = value;
            LOG_D("  ## Key/Value: \"%s\":\"%s\" -> \"%s\"", key.c_str(), value.c_str(), current_section.c_str())
            continue;
        }
        LOG_D("  ## Skip: \"%s\"", line.c_str())
    }
    return true;
}

bool Configuration::write(const std::filesystem::path &path) const
{
    std::ofstream fout(path);
    auto write_section = [&fout](const std::string& section_name, const SectionMap& section)
    {
        LOG_D("  ## Section: \"%s\"", section_name.c_str())
        fout << "[" << section_name << "]" << std::endl;
        for (const auto& item: section)
        {
            const auto&[key, value] = item;
            LOG_D("  ## \"%s\" = \"%s\"", key.c_str(), value.c_str())
            fout << key << " = " << value << std::endl;
        }

    };
    LOG_D("Saving configuration %s...", path.c_str())
    write_section("default", _file_map.at("default"));
    for (const auto& item: _file_map)
    {
        const auto&[section_name, section] = item;
        if (section_name != "default")
        {
            write_section(section_name, section);
        }
    }
    return false;
}

typename Configuration::iterator Configuration::begin()
{
    return _file_map.begin();
}
typename Configuration::const_iterator Configuration::cbegin() const
{
    return _file_map.cbegin();
}
typename Configuration::iterator Configuration::end()
{
    return _file_map.end();
}
typename Configuration::const_iterator Configuration::cend() const
{
    return _file_map.cend();
}

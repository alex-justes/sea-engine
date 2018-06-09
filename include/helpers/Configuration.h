#ifndef ENGINE_CONFIGURATION_H
#define ENGINE_CONFIGURATION_H

#include <filesystem>
#include <string>
#include <map>
#include <sstream>

namespace helpers::configuration
{
    class Configuration
    {
    public:
        using SectionMap = std::map<std::string, std::string>;
        using FileMap = std::map<std::string, SectionMap>;
        using const_iterator = typename FileMap::const_iterator;
        using iterator = typename FileMap::iterator;
        Configuration() = default;
        bool read(const std::filesystem::path& path);
        bool write(const std::filesystem::path& path) const;
        const_iterator cbegin() const;
        const_iterator cend() const;
        iterator begin();
        iterator end();
        template <class T>
        T get(const std::string& key, const std::string& section="default", T default_value=T()) const;
        template <class T>
        void set(const std::string& key, const T& value, const std::string& section="default");
        virtual ~Configuration() = default;
    private:
        FileMap _file_map;
    };

    // =================================================================================================

    template<class T>
    T Configuration::get(const std::string &key, const std::string &section, T default_value) const
    {
        const auto section_it = _file_map.find(section);
        if (section_it != _file_map.end())
        {
            const auto& section = section_it->second;
            const auto value_it = section.find(key);
            if (value_it != section.end())
            {
                const auto& value_str = value_it->second;
                std::istringstream sin(value_str);
                T value;
                sin >> value;
                return value;
            }
        }
        return default_value;
    }

    template<class T>
    void Configuration::set(const std::string &key, const T &value, const std::string &section)
    {
        if (_file_map.count(section) == 0)
        {
            _file_map[section] = SectionMap();
        }
        std::ostringstream sout;
        sout << value;
        _file_map[section][key] = sout.str();
    };

}

#endif //ENGINE_CONFIGURATION_H

#ifndef slic3r_Config_hpp_
#define slic3r_Config_hpp_

#include <myinit.h>
#include <map>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "Point.hpp"

namespace Slic3r {

typedef std::string t_config_option_key;
typedef std::vector<std::string> t_config_option_keys;

class ConfigOption {
    public:
    virtual ~ConfigOption() {};
    virtual std::string serialize() = 0;
    virtual void deserialize(std::string str) = 0;
};

class ConfigOptionFloat : public ConfigOption
{
    public:
    float value;
    ConfigOptionFloat() : value(0) {};
    
    operator float() const { return this->value; };
    
    std::string serialize() {
        std::ostringstream ss;
        ss << this->value;
        return ss.str();
    };
    
    void deserialize(std::string str) {
        this->value = ::atof(str.c_str());
    };
};

class ConfigOptionFloats : public ConfigOption
{
    public:
    std::vector<float> values;
    
    std::string serialize() {
        std::ostringstream ss;
        for (std::vector<float>::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            if (it - this->values.begin() != 0) ss << ",";
            ss << *it;
        }
        return ss.str();
    };
    
    void deserialize(std::string str) {
        this->values.clear();
        std::istringstream is(str);
        std::string item_str;
        while (std::getline(is, item_str, ',')) {
            this->values.push_back(::atof(item_str.c_str()));
        }
    };
};

class ConfigOptionInt : public ConfigOption
{
    public:
    int value;
    ConfigOptionInt() : value(0) {};
    
    operator int() const { return this->value; };
    
    std::string serialize() {
        std::ostringstream ss;
        ss << this->value;
        return ss.str();
    };
    
    void deserialize(std::string str) {
        this->value = ::atoi(str.c_str());
    };
};

class ConfigOptionInts : public ConfigOption
{
    public:
    std::vector<int> values;
    
    std::string serialize() {
        std::ostringstream ss;
        for (std::vector<int>::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            if (it - this->values.begin() != 0) ss << ",";
            ss << *it;
        }
        return ss.str();
    };
    
    void deserialize(std::string str) {
        this->values.clear();
        std::istringstream is(str);
        std::string item_str;
        while (std::getline(is, item_str, ',')) {
            this->values.push_back(::atoi(item_str.c_str()));
        }
    };
};

class ConfigOptionString : public ConfigOption
{
    public:
    std::string value;
    ConfigOptionString() : value("") {};
    
    operator std::string() const { return this->value; };
    
    std::string serialize() {
        std::string str = this->value;
        
        // s/\R/\\n/g
        size_t pos = 0;
        while ((pos = str.find("\n", pos)) != std::string::npos || (pos = str.find("\r", pos)) != std::string::npos) {
            str.replace(pos, 1, "\\n");
            pos += 2; // length of "\\n"
        }
        
        return str; 
    };
    
    void deserialize(std::string str) {
        // s/\\n/\n/g
        size_t pos = 0;
        while ((pos = str.find("\\n", pos)) != std::string::npos) {
            str.replace(pos, 2, "\n");
            pos += 1; // length of "\n"
        }
        
        this->value = str;
    };
};

class ConfigOptionFloatOrPercent : public ConfigOption
{
    public:
    float value;
    bool percent;
    ConfigOptionFloatOrPercent() : value(0), percent(false) {};
    
    std::string serialize() {
        std::ostringstream ss;
        ss << this->value;
        std::string s(ss.str());
        if (this->percent) s += "%";
        return s;
    };
    
    void deserialize(std::string str) {
        if (str.find_first_of("%") != std::string::npos) {
            sscanf(str.c_str(), "%f%%", &this->value);
            this->percent = true;
        } else {
            this->value = ::atof(str.c_str());
            this->percent = false;
        }
    };
};

class ConfigOptionPoint : public ConfigOption
{
    public:
    Pointf point;
    ConfigOptionPoint() : point(Pointf(0,0)) {};
    
    operator Pointf() const { return this->point; };
    
    std::string serialize() {
        std::ostringstream ss;
        ss << this->point.x;
        ss << ",";
        ss << this->point.y;
        return ss.str();
    };
    
    void deserialize(std::string str) {
        sscanf(str.c_str(), "%f%*1[,x]%f", &this->point.x, &this->point.y);
    };
};

class ConfigOptionPoints : public ConfigOption
{
    public:
    Pointfs points;
    
    std::string serialize() {
        std::ostringstream ss;
        for (Pointfs::const_iterator it = this->points.begin(); it != this->points.end(); ++it) {
            if (it - this->points.begin() != 0) ss << ",";
            ss << it->x;
            ss << "x";
            ss << it->y;
        }
        return ss.str();
    };
    
    void deserialize(std::string str) {
        this->points.clear();
        std::istringstream is(str);
        std::string point_str;
        while (std::getline(is, point_str, ',')) {
            Pointf point;
            sscanf(point_str.c_str(), "%fx%f", &point.x, &point.y);
            this->points.push_back(point);
        }
    };
};

class ConfigOptionBool : public ConfigOption
{
    public:
    bool value;
    ConfigOptionBool() : value(false) {};
    
    operator bool() const { return this->value; };
    
    std::string serialize() {
        return std::string(this->value ? "1" : "0");
    };
    
    void deserialize(std::string str) {
        this->value = (str.compare("1") == 0);
    };
};

class ConfigOptionBools : public ConfigOption
{
    public:
    std::vector<bool> values;
    
    std::string serialize() {
        std::ostringstream ss;
        for (std::vector<bool>::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            if (it - this->values.begin() != 0) ss << ",";
            ss << (*it ? "1" : "0");
        }
        return ss.str();
    };
    
    void deserialize(std::string str) {
        this->values.clear();
        std::istringstream is(str);
        std::string item_str;
        while (std::getline(is, item_str, ',')) {
            this->values.push_back(item_str.compare("1") == 0);
        }
    };
};

typedef std::map<std::string,int> t_config_enum_values;

template <class T>
class ConfigOptionEnum : public ConfigOption
{
    public:
    T value;
    
    operator T() const { return this->value; };
    
    std::string serialize() {
        t_config_enum_values enum_keys_map = ConfigOptionEnum<T>::get_enum_values();
        for (t_config_enum_values::iterator it = enum_keys_map.begin(); it != enum_keys_map.end(); ++it) {
            if (it->second == static_cast<int>(this->value)) return it->first;
        }
        return "";
    };

    void deserialize(std::string str) {
        t_config_enum_values enum_keys_map = ConfigOptionEnum<T>::get_enum_values();
        assert(enum_keys_map.count(str) > 0);
        this->value = static_cast<T>(enum_keys_map[str]);
    };

    static t_config_enum_values get_enum_values();
};

/* We use this one in DynamicConfig objects, otherwise it's better to use
   the specialized ConfigOptionEnum<T> containers. */
class ConfigOptionEnumGeneric : public ConfigOption
{
    public:
    int value;
    t_config_enum_values* keys_map;
    
    operator int() const { return this->value; };
    
    std::string serialize() {
        for (t_config_enum_values::iterator it = this->keys_map->begin(); it != this->keys_map->end(); ++it) {
            if (it->second == this->value) return it->first;
        }
        return "";
    };

    void deserialize(std::string str) {
        assert(this->keys_map->count(str) != 0);
        this->value = (*this->keys_map)[str];
    };
};

enum ConfigOptionType {
    coFloat,
    coFloats,
    coInt,
    coInts,
    coString,
    coFloatOrPercent,
    coPoint,
    coPoints,
    coBool,
    coBools,
    coEnum,
};

class ConfigOptionDef
{
    public:
    ConfigOptionType type;
    std::string label;
    std::string tooltip;
    std::string ratio_over;
    t_config_enum_values enum_keys_map;
};

typedef std::map<t_config_option_key,ConfigOptionDef> t_optiondef_map;

ConfigOptionDef* get_config_option_def(const t_config_option_key opt_key);

class ConfigBase
{
    public:
    t_optiondef_map* def;
    
    ConfigBase() : def(NULL) {};
    virtual ConfigOption* option(const t_config_option_key opt_key, bool create = false) = 0;
    virtual void keys(t_config_option_keys *keys) = 0;
    void apply(ConfigBase &other, bool ignore_nonexistent = false);
    std::string serialize(const t_config_option_key opt_key);
    void set_deserialize(const t_config_option_key opt_key, std::string str);
    float get_abs_value(const t_config_option_key opt_key);
    
    #ifdef SLIC3RXS
    SV* get(t_config_option_key opt_key);
    void set(t_config_option_key opt_key, SV* value);
    #endif
};

class DynamicConfig : public ConfigBase
{
    public:
    DynamicConfig(){};
    ~DynamicConfig();
    ConfigOption* option(const t_config_option_key opt_key, bool create = false);
    void keys(t_config_option_keys *keys);
    bool has(const t_config_option_key opt_key) const;
    
    private:
    DynamicConfig(const DynamicConfig& other);              // we disable this by making it private and unimplemented
    DynamicConfig& operator= (const DynamicConfig& other);  // we disable this by making it private and unimplemented
    typedef std::map<t_config_option_key,ConfigOption*> t_options_map;
    t_options_map options;
};

class StaticConfig : public ConfigBase
{
    public:
    void keys(t_config_option_keys *keys);
};

}

#endif
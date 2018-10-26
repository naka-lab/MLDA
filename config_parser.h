#pragma once
#include <map>
#include <string>
#include <string.h>
#include <fstream>
#include <cassert> 
#include "picojson.h"
#include "myio.h"

class config_parser
{
public:

	config_parser( int argc, char *argv[] )
	{
		for (size_t i = 1; i < argc; i++ )
		{
			std::string key;
			std::string value;

			if (strstr(argv[i],"-")==argv[i])
			{
				key = (argv[i]+1);
			}
			else
			{
				continue;
			}

			if ( argc>i+1 && strstr(argv[i+1], "-")!=argv[i+1])
			{
				value = argv[i + 1];
				i++;
			}
			m_cmdargs[key] = value;
		}

		if ( m_cmdargs.find( "config" )!=m_cmdargs.end() )
		{
			std::ifstream fs;
			picojson::value val;
			fs.open( m_cmdargs["config"].c_str(), std::ios::binary );

			if (!fs.is_open())
			{
				printf("Error: cannot open \"%s\"\n" , m_cmdargs["config"].c_str());
				exit(-1);
			}

			fs >> val;
			fs.close();
			//m_jsonobj = val.get<picojson::object>()["config"].get<picojson::object>();
			m_jsonobj = val.get<picojson::object>();
		}
	}

	~config_parser()
	{
	}


	template <typename T> bool get(const std::string key, T &value)
	{
		if (m_cmdargs.find(key) != m_cmdargs.end() && sscanfVar(&value, m_cmdargs[key].c_str()) == 1)
		{
			return true;
		}

		if (m_jsonobj.find(key)!=m_jsonobj.end())
		{
			std::string str = m_jsonobj[key].to_str();

			if (sscanfVar(&value, str.c_str()) == 1)
			{
				return true;
			}
		}
		return false;
	}

	template <typename T> void get_and_assert(const std::string key, T &value)
	{
		if ( !get(key, value) )
		{
			printf("Error: cannot get value \"%s\"\n", key.c_str());
			exit(-1);
		}
	}

	bool contains(const std::string key)
	{
		if (m_cmdargs.find(key) != m_cmdargs.end() || m_jsonobj.find(key)!=m_jsonobj.end()) return true;

		return false;
	}

protected:
	std::map<std::string, std::string> m_cmdargs;
	picojson::object m_jsonobj;



};
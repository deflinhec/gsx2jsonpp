/*-
* Copyright (c) 2021 Deflinhec, https://github.com/deflinhec
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

#include "gxs2json.h"
#include <iostream>
#include <uriparser/Uri.h>
#include <nlohmann/json.hpp>
#include <exception>

using namespace nlohmann;

namespace Gxs2Json
{
static bool is_number(const std::string& _s)
{
	if (_s.empty())
		return false;
	
	auto pred = [](unsigned char c) { return !std::isdigit(c); };
	return std::find_if(_s.begin(), _s.end(), pred) == _s.end();
}

void parse(const std::string& _uri, Config* _cfg, Identifier* _id)
{
	UriUriA uri;
	const char* error = nullptr;
	if (uriParseSingleUriA(&uri, _uri.c_str(), &error) != URI_SUCCESS)
	{
		throw std::invalid_argument("uriParseSingleUriA failed");
	}
	int count = 0;
	UriQueryListA* list = nullptr;
	if (uriDissectQueryMallocA(&list, &count, uri.query.first, uri.query.afterLast) != URI_SUCCESS)
	{
		throw std::invalid_argument("uriDissectQueryMallocA failed");
	}
	for (auto query = list; query; query = query->next)
	{
		if (_id && std::strcmp(query->key, "id") == 0)
		{
			_id->id = query->value;
		}
		else if (_id && std::strcmp(query->key, "sheet") == 0)
		{
			_id->sheet = std::stoi(query->value);
		}
		else if (_cfg && std::strcmp(query->key, "integers") == 0)
		{
			_cfg->useInteger = std::strcmp(query->value, "true") == 0;
		}
		else if (_cfg && std::strcmp(query->key, "rows") == 0)
		{
			_cfg->showRows = std::strcmp(query->value, "true") == 0;
		}
		else if (_cfg && std::strcmp(query->key, "columns") == 0)
		{
			_cfg->showColumns = std::strcmp(query->value, "true") == 0;
		}
		else if (_cfg && std::strcmp(query->key, "dict") == 0)
		{
			_cfg->showDict = std::strcmp(query->value, "true") == 0;
		}
		else if (_cfg && std::strcmp(query->key, "q") == 0)
		{
			_cfg->query = query->value;
		}
	}
	uriFreeQueryListA(list);
}
	
void parse(Content* _content, const std::string& _json, Config _cfg)
{
	json object, dict, rows, columns;
	auto raw = json::parse(_json);
	auto timestamp = raw["feed"]["updated"]["$t"];
	_content->timestamp = timestamp.get<std::string>();
	for (auto entry : raw["feed"]["entry"])
	{
		json row;
		unsigned pkey = 0;
		bool queried = _cfg.query.empty();
		for (auto it = entry.begin(); it != entry.end(); it++)
		{
			// left most column as key
			if (it.key().compare("title") == 0)
			{
				auto value = it.value()["$t"].get<std::string>();
				if (is_number(value))
				{
					pkey = std::atoi(value.c_str());
				}
			}
			// seek for actual data
			else if (it.key().rfind("gsx$", 0) == 0)
			{
				auto key = it.key();
				key = key.substr(strlen("gsx$"));
				// skip prefix with "noex" (non export)
				if (key.rfind("noex", 0) == 0)
					continue;
				auto value = it.value()["$t"].get<std::string>();
				if (!_cfg.query.empty())
				{
					std::string lkey, lvalue, lquery;
					std::transform(key.begin(), key.end(), lkey.begin(), ::tolower);
					std::transform(value.begin(), value.end(), lvalue.begin(), ::tolower);
					std::transform(_cfg.query.begin(), _cfg.query.end(), lquery.begin(), ::tolower);
					queried |= lkey.rfind(lquery, 0) >= 0;
					queried |= lvalue.rfind(lquery, 0) >= 0;
				}
				row[key] = value;
				if (_cfg.useInteger)
				{
					if (value.empty())
					{
						row[key] = 0;
					}
					else if (is_number(value))
					{
						row[key] = std::stoi(value);
					}
				}
				if (queried)
				{
					json jvalue = value;
					if (_cfg.useInteger)
					{
						if (value.empty())
						{
							jvalue = 0;
						}
						else if (is_number(value))
						{
							jvalue = std::stoi(value);
						}
					}
					columns[key].push_back(jvalue);
				}
			}
		}
		if (queried)
		{
			rows.push_back(row);
			dict[std::to_string(pkey)] = row;
		}
	}
	if (_cfg.showColumns)
	{
		object["columns"] = columns;
	}
	if (_cfg.showRows)
	{
		object["rows"] = rows;
	}
	if (_cfg.showDict)
	{
		object["dict"] = dict;
	}
	_content->payload = object.dump();
}
}

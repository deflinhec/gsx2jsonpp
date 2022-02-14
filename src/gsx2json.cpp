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

#include "gsx2json.h"
#include "utils.h"
#include <iostream>
#include <uriparser/Uri.h>
#include <nlohmann/json.hpp>
#include <exception>

using namespace nlohmann;

namespace Gsx2Json
{

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
			_id->sheet = query->value;
		}
		else if (_id && std::strcmp(query->key, "api_key") == 0)
		{
			_id->apiKey = query->value;
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
		else if (_cfg && std::strcmp(query->key, "meta") == 0)
		{
			_cfg->briefMeta = std::strcmp(query->value, "true") == 0;
		}
		else if (_cfg && std::strcmp(query->key, "pretty") == 0)
		{
			_cfg->prettyPrint = std::strcmp(query->value, "true") == 0;
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
	using json = ordered_json;
	json object, dict, rows, columns;
	auto raw = json::parse(_json);

	auto values = raw["values"];
	auto it = values.begin();
	auto headings = *it;
	for (++it; it != values.end(); it++) {
		json row;
		auto entry = *it;
		unsigned pkey = 0;
		bool queried = _cfg.query.empty();
		for (auto i = 0; i < headings.size(); i++) {
			auto key = headings[i].get<std::string>();
			// skip prefix with "NOEX_" (non export)
			if (key.rfind("NOEX_", 0) == 0)
				continue;
			std::string value;
			if (i < entry.size())
			{
				value = entry[i].get<std::string>();
			}
			if (is_number(value) && pkey == 0)
			{
				pkey = std::atoi(value.c_str());
			}
			if (!_cfg.query.empty())
			{
				queried |= key.rfind(_cfg.query, 0) >= 0;
				queried |= value.rfind(_cfg.query, 0) >= 0;
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
				columns[key].push_back(row[key]);
			}
		}
		if (queried)
		{
			rows.push_back(row);
			dict[std::to_string(pkey)] = row;
		}
	}
	if (_cfg.showColumns && !_cfg.briefMeta)
	{
		object["columns"] = columns;
	}
	if (_cfg.showRows && !_cfg.briefMeta)
	{
		object["rows"] = rows;
	}
	if (_cfg.showDict && !_cfg.briefMeta)
	{
		object["dict"] = dict;
	}
	if (_cfg.showColumns)
	{
		auto buffer = columns.dump();
		object["meta"]["columns"]["md5"] = md5(buffer);
		object["meta"]["columns"]["bytes"] = buffer.size();
	}
	if (_cfg.showRows)
	{
		auto buffer = rows.dump();
		object["meta"]["rows"]["md5"] = md5(buffer);
		object["meta"]["rows"]["bytes"] = buffer.size();
	}
	if (_cfg.showDict)
	{
		auto buffer = dict.dump();
		object["meta"]["dict"]["md5"] = md5(buffer);
		object["meta"]["dict"]["bytes"] = buffer.size();
	}	
	const int indent = _cfg.prettyPrint ? 1 : -1;
	_content->payload = object.dump(indent);
}
}

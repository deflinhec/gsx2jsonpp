//
//  gxs_content.c
//  Gsx2Jsonpp
//
//  Created by Deflinhec on 2021/1/30.
//

#include "gxs2json.h"
#include <iostream>
#include <uriparser/Uri.h>
#include <nlohmann/json.hpp>
#include <exception>

using namespace nlohmann;

namespace Gxs2Json
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
			_id->sheet = std::stoi(query->value);
		}
		else if (_cfg && std::strcmp(query->key, "integers") == 0)
		{
			_cfg->useInteger = std::strcmp(query->key, "true") == 0;
		}
		else if (_cfg && std::strcmp(query->key, "rows") == 0)
		{
			_cfg->showRows = std::strcmp(query->key, "true") == 0;
		}
		else if (_cfg && std::strcmp(query->key, "columns") == 0)
		{
			_cfg->showColumns = std::strcmp(query->key, "true") == 0;
		}
		else if (_cfg && std::strcmp(query->key, "q") == 0)
		{
			_cfg->query = query->value;
		}
		std::cout << query->key << ":" << query->value << std::endl;
	}
	uriFreeQueryListA(list);
}
	
void parse(Content* _content, const std::string& _json, Config _cfg)
{
	json object, rows, columns;
	auto raw = json::parse(_json);
	auto timestamp = raw["feed"]["updated"]["$t"];
	auto entries = raw["feed"]["entry"];
	_content->timestamp = timestamp.get<std::string>();
	for (auto entry : entries)
	{
		json newRow;
		unsigned pkey = 0;
		bool queried = _cfg.query.empty();
		for (auto it = entry.rbegin(); it != entry.rend(); it++)
		{
			// seek for actual data
			if (it.key().rfind("gsx$", 0) != 0)
				continue;
			auto key = it.key();
			key = key.substr(strlen("gsx$"));
			// skip prefix with "noex" (non export)
			if (key.rfind("noex", 0) == 0)
				continue;
			auto value = it.value()["$t"].get<std::string>();
			if (pkey == 0)
			{
				pkey = std::atoi(value.c_str());
			}
			if (!_cfg.query.empty())
			{
				std::string lkey, lvalue, lquery;
				std::transform(key.begin(), key.end(), lkey.begin(), ::tolower);
				std::transform(value.begin(), value.end(), lvalue.begin(), ::tolower);
				std::transform(_cfg.query.begin(), _cfg.query.end(), lquery.begin(), ::tolower);
				queried |= lkey.rfind(lquery, 0) >= 0;
				queried |= lvalue.rfind(lquery, 0) >= 0;
			}
			newRow[key] = value;
			if (_cfg.useInteger)
			{
				try {
					newRow[key] = std::stoi(value);
				}
				catch(const std::exception& e) {
					// Do not thing here
				}
			}
			if (queried)
			{
			  columns[key].push_back(value);
			}
		}
		if (queried)
		{
		  rows[std::to_string(pkey)] = newRow;
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
	_content->payload = object.dump();
}
}

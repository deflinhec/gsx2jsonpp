//
//  gxs_content.c
//  Gsx2Jsonpp
//
//  Created by Deflinhec on 2021/1/30.
//

#include "gxs2json.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <exception>

using namespace nlohmann;

namespace Gxs2Json
{
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

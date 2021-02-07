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
#include <iostream>
#include <uriparser/Uri.h>
#include <nlohmann/json.hpp>
#include <openssl/md5.h>
#include <exception>

using namespace nlohmann;

namespace Gxs2Json
{
static bool is_number(const std::string& _s)
{
	if(_s.empty()) 
		return false;
	
	if ((!isdigit(_s[0])) && (_s[0] != '-') && (_s[0] != '+'))
		return false;

	char* p = nullptr;
	strtol(_s.c_str(), &p, 10);

	return (*p == 0);
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
	auto timestamp = raw["feed"]["updated"]["$t"];
	_content->timestamp = timestamp.get<std::string>();
	for (auto entry : raw["feed"]["entry"])
	{
		json row;
		unsigned pkey = 0;
		bool queried = _cfg.query.empty();
		for (auto it = entry.begin(); it != entry.end(); it++)
		{
			// seek for actual data
			if (it.key().rfind("gsx$", 0) == 0)
			{
				auto key = it.key();
				key = key.substr(strlen("gsx$"));
				// skip prefix with "noex" (non export)
				if (key.rfind("noex", 0) == 0)
					continue;
				auto value = it.value()["$t"].get<std::string>();
				if (is_number(value) && pkey == 0)
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
		MD5_CTX ctx; MD5_Init(&ctx);
		MD5_Update(&ctx, (unsigned char *)buffer.data(), buffer.size());
		unsigned char md5[MD5_DIGEST_LENGTH]; MD5_Final(md5, &ctx);
		std::string checksum; checksum.reserve(32);
		for (std::size_t i = 0; i != 16; ++i)
		{
			checksum += "0123456789ABCDEF"[md5[i] / 16];
			checksum += "0123456789ABCDEF"[md5[i] % 16];
		}
		object["meta"]["columns"]["md5"] = checksum;
		object["meta"]["columns"]["bytes"] = buffer.size();
	}
	if (_cfg.showRows)
	{
		auto buffer = rows.dump();
		MD5_CTX ctx; MD5_Init(&ctx);
		MD5_Update(&ctx, (unsigned char *)buffer.data(), buffer.size());
		unsigned char md5[MD5_DIGEST_LENGTH]; MD5_Final(md5, &ctx);
		std::string checksum; checksum.reserve(32);
		for (std::size_t i = 0; i != 16; ++i)
		{
			checksum += "0123456789ABCDEF"[md5[i] / 16];
			checksum += "0123456789ABCDEF"[md5[i] % 16];
		}
		object["meta"]["rows"]["md5"] = checksum;
		object["meta"]["rows"]["bytes"] = buffer.size();
	}
	if (_cfg.showDict)
	{
		auto buffer = dict.dump();
		MD5_CTX ctx; MD5_Init(&ctx);
		MD5_Update(&ctx, (unsigned char *)buffer.data(), buffer.size());
		unsigned char md5[MD5_DIGEST_LENGTH]; MD5_Final(md5, &ctx);
		std::string checksum; checksum.reserve(32);
		for (std::size_t i = 0; i != 16; ++i)
		{
			checksum += "0123456789ABCDEF"[md5[i] / 16];
			checksum += "0123456789ABCDEF"[md5[i] % 16];
		}
		object["meta"]["dict"]["md5"] = checksum;
		object["meta"]["dict"]["bytes"] = buffer.size();
	}	
	const int indent = _cfg.prettyPrint ? 1 : -1;
	object["meta"]["time"] = _content->timestamp;
	_content->payload = object.dump(indent);
}
}

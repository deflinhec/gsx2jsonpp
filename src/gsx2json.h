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

#pragma once

#include <stdio.h>
#include <string>
#include <vector>

#define SPREADSHEET_HOST "spreadsheet.google.com"
#define SPREADSHEET_URI_FORMAT "/feeds/list/%s/%d/public/values?alt=json"

namespace Gxs2Json
{

struct Identifier
{
	std::string id;
	unsigned int sheet = 1;
};

struct Config
{
	std::string query;
	bool useInteger = true;
	bool showDict = true;
	bool showRows = true;
	bool showColumns = true;
	bool briefMeta = false;
	bool prettyPrint = false;
};

void parse(const std::string& _uri, Config* _cfg, Identifier* _id = nullptr);

struct Content
{
	std::string payload;
};

void parse(Content* _content, const std::string& _json, Config _cfg = Config());
}

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

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <httplib.h>
#include <fstream>
#include "gsx2json.h"
#include "cache.h"
#include "utils.h"

#define CA_CERT_FILE "./ca-bundle.crt"
using namespace httplib;
using namespace nlohmann;

static const char* SpreadsheetID = "1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4";

TEST(SSLClient, GoogleSpreadSheet)
{
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	struct stat buffer;
	ASSERT_FALSE(stat(CA_CERT_FILE, &buffer)) << CA_CERT_FILE << " not found";
	SSLClient ins(SPREADSHEET_HOST);
	ins.set_ca_cert_path(CA_CERT_FILE);
	ins.enable_server_certificate_verification(true);
	char url[BUFSIZ] = {0};
	snprintf(url, sizeof(url), SPREADSHEET_URI_FORMAT, SpreadsheetID, 1);
	auto res = ins.Get(url);
	ASSERT_TRUE(res);
	EXPECT_EQ(res->status, 200);
	ASSERT_EQ(res->get_header_value("Content-Type"), "application/json; charset=UTF-8");
	ASSERT_FALSE(res->body.empty());
#endif
}

TEST(Client, GoogleSpreadSheet)
{
	Client ins(SPREADSHEET_HOST);
	char url[BUFSIZ] = {0};
	snprintf(url, sizeof(url), SPREADSHEET_URI_FORMAT, SpreadsheetID, 1);
	auto res = ins.Get(url);
	ASSERT_TRUE(res);
	EXPECT_EQ(res->status, 200);
	ASSERT_EQ(res->get_header_value("Content-Type"), "application/json; charset=UTF-8");
	ASSERT_FALSE(res->body.empty());
}

TEST(URI, ParseIdentifier)
{
	char url[BUFSIZ] = {0};
	const char* fmt = "/api?id=%s&sheet=%d";
	snprintf(url, sizeof(url), fmt, SpreadsheetID, 1);
	using namespace Gsx2Json; Identifier id;
	EXPECT_EQ(id.sheet, 1);
	EXPECT_TRUE(id.id.empty());
	EXPECT_NO_THROW(parse(url, nullptr, &id));
	EXPECT_STREQ(id.id.c_str(), SpreadsheetID);
	EXPECT_EQ(id.sheet, 1);
}

TEST(URI, ParseConfigShowColumns)
{
	char url[BUFSIZ] = {0};
	const char* fmt = "/api?columns=false";
	snprintf(url, sizeof(url), fmt, "");
	using namespace Gsx2Json; Config config;
	EXPECT_TRUE(config.showColumns);
	EXPECT_NO_THROW(parse(url, &config));
	EXPECT_FALSE(config.showColumns);
}

TEST(URI, ParseConfigShowRows)
{
	const char* url = "/api?rows=false";
	using namespace Gsx2Json; Config config;
	EXPECT_TRUE(config.showRows);
	EXPECT_NO_THROW(parse(url, &config));
	EXPECT_FALSE(config.showRows);
}

TEST(URI, ParseConfigShowDict)
{
	const char* url = "/api?dict=false";
	using namespace Gsx2Json; Config config;
	EXPECT_TRUE(config.showDict);
	EXPECT_NO_THROW(parse(url, &config));
	EXPECT_FALSE(config.showDict);
}

TEST(URI, ParseConfigShowMeta)
{
	const char* url = "/api?meta=true";
	using namespace Gsx2Json; Config config;
	EXPECT_FALSE(config.briefMeta);
	EXPECT_NO_THROW(parse(url, &config));
	EXPECT_TRUE(config.briefMeta);
}

TEST(URI, ParseConfigPrettyPrint)
{
	const char* url = "/api?pretty=true";
	using namespace Gsx2Json; Config config;
	EXPECT_FALSE(config.prettyPrint);
	EXPECT_NO_THROW(parse(url, &config));
	EXPECT_TRUE(config.prettyPrint);
}

TEST(URI, ParseConfigUseInteger)
{
	const char* url = "/api?integers=false";
	using namespace Gsx2Json; Config config;
	EXPECT_TRUE(config.useInteger);
	EXPECT_NO_THROW(parse(url, &config));
	EXPECT_FALSE(config.useInteger);
}

TEST(URI, ParseConfigQuerySearch)
{
	const char* url = "/api?q=aabb";
	using namespace Gsx2Json; Config config;
	EXPECT_TRUE(config.query.empty());
	EXPECT_NO_THROW(parse(url, &config));
	EXPECT_STREQ(config.query.c_str(), "aabb");
}

TEST(URI, ParseCompondQueries)
{
	char url[BUFSIZ] = {0};
	const char* fmt =
	"/api?q=aabb&integers=false&dict=false"
	"&columns=false&rows=false&id=%s&sheet=%d"
	"&pretty=true&meta=true";
	snprintf(url, sizeof(url), fmt, SpreadsheetID, 1);
	using namespace Gsx2Json;
	Config config; Identifier id;
	EXPECT_EQ(id.sheet, 1);
	EXPECT_TRUE(id.id.empty());
	EXPECT_TRUE(config.showDict);
	EXPECT_TRUE(config.showRows);
	EXPECT_TRUE(config.showColumns);
	EXPECT_FALSE(config.briefMeta);
	EXPECT_FALSE(config.prettyPrint);
	EXPECT_TRUE(config.useInteger);
	EXPECT_TRUE(config.query.empty());
	EXPECT_NO_THROW(parse(url, &config, &id));
	EXPECT_EQ(id.sheet, 1);
	EXPECT_STREQ(id.id.c_str(), SpreadsheetID);
	EXPECT_FALSE(config.showDict);
	EXPECT_FALSE(config.showRows);
	EXPECT_FALSE(config.showColumns);
	EXPECT_FALSE(config.useInteger);
	EXPECT_TRUE(config.briefMeta);
	EXPECT_TRUE(config.prettyPrint);
	EXPECT_STREQ(config.query.c_str(), "aabb");
}

TEST(Number, LiteralNumber)
{
	using namespace Gsx2Json;
	EXPECT_TRUE(is_number("1"));
	EXPECT_TRUE(is_number("0"));
	EXPECT_TRUE(is_number("-1"));
	EXPECT_TRUE(is_number("1.0"));
	EXPECT_TRUE(is_number("0.0"));
	EXPECT_TRUE(is_number("-1.0"));
	EXPECT_FALSE(is_number("-1.0A"));
	EXPECT_FALSE(is_number("1.0A"));
	EXPECT_FALSE(is_number("-1A"));
	EXPECT_FALSE(is_number("1A"));
}

class IOTests: public ::testing::Test
{
protected:
  
    virtual void SetUp();

	virtual void TearDown();
   
   	json Object;
	std::string filename = "output.json";
};

void IOTests::SetUp()
{
	Client ins(SPREADSHEET_HOST);
	char url[BUFSIZ] = {0};
	snprintf(url, sizeof(url), SPREADSHEET_URI_FORMAT, SpreadsheetID, 1);
	auto res = ins.Get(url);
	ASSERT_TRUE(res);
	EXPECT_EQ(res->status, 200);
	ASSERT_EQ(res->get_header_value("Content-Type"), "application/json; charset=UTF-8");
	ASSERT_FALSE(res->body.empty());
	EXPECT_NO_THROW(Object = json::parse(res->body));
	std::ofstream output(filename);
	output << Object;
}

void IOTests::TearDown()
{
	remove(filename.c_str());
}

TEST_F(IOTests, ReadAndParse)
{
	json object;
	EXPECT_TRUE(object.empty());
	std::ifstream input(filename);
	input >> object;
	EXPECT_FALSE(object.empty());
	EXPECT_EQ(json::diff(Object, object).size(), 0);
}

TEST_F(IOTests, LoadFromMemory)
{
	using namespace Gsx2Json; 
	Content content; 
	Identifier id;
	id.sheet = 1;
	id.id = SpreadsheetID;
	json object;
	std::string gsxcontent;
	using Type = Cache::Manager::Type;
	Cache::Manager manager(Type::MEMORY);
	EXPECT_NO_THROW(manager.save(Object.dump(), id));
	EXPECT_NO_THROW(manager.load(gsxcontent, id));
	EXPECT_NO_THROW(object = json::parse(gsxcontent));
	EXPECT_EQ(json::diff(Object, object).size(), 0);
	EXPECT_NO_THROW(parse(&content, gsxcontent));
	ASSERT_FALSE(content.payload.empty());
	EXPECT_NO_THROW(manager.flush());
}

TEST_F(IOTests, ThreadSafeLoadFromMemory)
{
	using namespace Gsx2Json; 
	using Type = Cache::Manager::Type;
	Cache::Manager manager(Type::MEMORY);
	std::thread threads[100];
    for (int i = 0; i != 100; i++)
    {
        threads[i] = std::thread([&]{
			Content content; 
			Identifier id;
			id.sheet = 1;
			id.id = SpreadsheetID;
			std::string gsxcontent;
			json object;
			EXPECT_NO_THROW(manager.save(Object.dump(), id));
			EXPECT_NO_THROW(manager.load(gsxcontent, id));
			EXPECT_NO_THROW(object = json::parse(gsxcontent));
			EXPECT_EQ(json::diff(Object, object).size(), 0);
			EXPECT_NO_THROW(parse(&content, gsxcontent));
			ASSERT_FALSE(content.payload.empty());
		});
    }
    for (auto &task : threads)
        task.join();
	EXPECT_NO_THROW(manager.flush());
}

TEST_F(IOTests, LoadFromFile)
{
	using namespace Gsx2Json; 
	Content content; 
	Identifier id;
	id.sheet = 1;
	id.id = SpreadsheetID;
	json object;
	std::string gsxcontent;
	using Type = Cache::Manager::Type;
	Cache::Manager manager(Type::FILE);
	EXPECT_NO_THROW(manager.save(Object.dump(), id));
	EXPECT_NO_THROW(manager.load(gsxcontent, id));
	EXPECT_NO_THROW(object = json::parse(gsxcontent));
	EXPECT_EQ(json::diff(Object, object).size(), 0);
	EXPECT_NO_THROW(parse(&content, gsxcontent));
	ASSERT_FALSE(content.payload.empty());
	EXPECT_NO_THROW(manager.flush());
}

TEST_F(IOTests, ThreadSafeLoadFromFile)
{
	using namespace Gsx2Json; 
	using Type = Cache::Manager::Type;
	Cache::Manager manager(Type::FILE);
	std::thread threads[100];
    for (int i = 0; i != 100; i++)
    {
        threads[i] = std::thread([&]{
			Content content; 
			Identifier id;
			id.sheet = 1;
			id.id = SpreadsheetID;
			std::string gsxcontent;
			json object;
			EXPECT_NO_THROW(manager.save(Object.dump(), id));
			EXPECT_NO_THROW(manager.load(gsxcontent, id));
			EXPECT_NO_THROW(object = json::parse(gsxcontent));
			EXPECT_EQ(json::diff(Object, object).size(), 0);
			EXPECT_NO_THROW(parse(&content, gsxcontent));
			ASSERT_FALSE(content.payload.empty());
		});
    }
    for (auto &task : threads)
        task.join();
	EXPECT_NO_THROW(manager.flush());
}

class ParserTests: public ::testing::Test
{
protected:
  
    virtual void SetUp();
   
	std::string GsxContent;
};

void ParserTests::SetUp()
{
	Client ins(SPREADSHEET_HOST);
	char url[BUFSIZ] = {0};
	snprintf(url, sizeof(url), SPREADSHEET_URI_FORMAT, SpreadsheetID, 1);
	auto res = ins.Get(url);
	ASSERT_TRUE(res);
	EXPECT_EQ(res->status, 200);
	ASSERT_EQ(res->get_header_value("Content-Type"), "application/json; charset=UTF-8");
	ASSERT_FALSE(res->body.empty());
	GsxContent = res->body;
}

TEST_F(ParserTests, ToJson)
{
	ASSERT_FALSE(GsxContent.empty());
	using namespace Gsx2Json; Content content;
	EXPECT_NO_THROW(parse(&content, GsxContent.c_str()));
	ASSERT_FALSE(content.payload.empty());
}

TEST_F(ParserTests, RowCompactibility)
{
	Client ins("gsx2json.com");
	char url[BUFSIZ] = {0};
	const char* fmt = "/api?id=%s&sheet=%d&columns=false";
	snprintf(url, sizeof(url), fmt, SpreadsheetID, 1);
	auto res = ins.Get(url);
	ASSERT_TRUE(res) << "Connection failed";
	EXPECT_EQ(res->status, 200) << "Connection failed";
	const char* ctype = "application/json; charset=utf-8";
	ASSERT_EQ(res->get_header_value("Content-Type"), ctype);
	ASSERT_FALSE(res->body.empty());
	auto a = json::parse(res->body);
	using namespace Gsx2Json; Content content;
	EXPECT_NO_THROW(parse(&content, GsxContent.c_str()));
	auto b = json::parse(content.payload);
	ASSERT_FALSE(content.payload.empty());
	EXPECT_FALSE(a["rows"].empty());
	EXPECT_FALSE(b["rows"].empty());
	EXPECT_EQ(json::diff(a["rows"], b["rows"]).size(), 0);
}

TEST_F(ParserTests, ColumnsCompactibility)
{
	Client ins("gsx2json.com");
	char url[BUFSIZ] = {0};
	const char* fmt = "/api?id=%s&sheet=%d&rows=false";
	snprintf(url, sizeof(url), fmt, SpreadsheetID, 1);
	auto res = ins.Get(url);
	ASSERT_TRUE(res) << "Connection failed";
	EXPECT_EQ(res->status, 200) << "Connection failed";
	const char* ctype = "application/json; charset=utf-8";
	ASSERT_EQ(res->get_header_value("Content-Type"), ctype);
	ASSERT_FALSE(res->body.empty());
	auto a = json::parse(res->body);
	using namespace Gsx2Json; Content content;
	EXPECT_NO_THROW(parse(&content, GsxContent.c_str()));
	auto b = json::parse(content.payload);
	ASSERT_FALSE(content.payload.empty());
	EXPECT_FALSE(a["columns"].empty());
	EXPECT_FALSE(b["columns"].empty());
	EXPECT_EQ(json::diff(a["columns"], b["columns"]).size(), 0);
}

TEST_F(ParserTests, QueryCompactibility)
{
	Client ins("gsx2json.com");
	char url[BUFSIZ] = {0};
	const char* fmt = "/api?id=%s&sheet=%d&query=1b";
	snprintf(url, sizeof(url), fmt, SpreadsheetID, 1);
	auto res = ins.Get(url);
	ASSERT_TRUE(res) << "Connection failed";
	EXPECT_EQ(res->status, 200) << "Connection failed";
	const char* ctype = "application/json; charset=utf-8";
	ASSERT_EQ(res->get_header_value("Content-Type"), ctype);
	ASSERT_FALSE(res->body.empty());
	auto a = json::parse(res->body);
	using namespace Gsx2Json; Content content;
	EXPECT_NO_THROW(parse(&content, GsxContent.c_str()));
	auto b = json::parse(content.payload);
	ASSERT_FALSE(content.payload.empty());
	EXPECT_FALSE(a["rows"].empty());
	EXPECT_FALSE(b["rows"].empty());
	EXPECT_EQ(json::diff(a["rows"], b["rows"]).size(), 0);
	EXPECT_FALSE(a["columns"].empty());
	EXPECT_FALSE(b["columns"].empty());
	EXPECT_EQ(json::diff(a["columns"], b["columns"]).size(), 0);
}
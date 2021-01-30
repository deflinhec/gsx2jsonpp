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
#include "gxs2json.h"

#define CA_CERT_FILE "./ca-bundle.crt"
#define GSX2JSON_URI_FORMAT "/api?id=%s&sheet=%d&columns=false&rows=false"
using namespace httplib;
using namespace nlohmann;

static const char* SpreadsheetID = "1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4";

TEST(SSLClient, GoogleSpreadSheet)
{
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	struct stat buffer;
	ASSERT_FALSE(stat(CA_CERT_FILE, &buffer));
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

TEST(URI, Parse)
{
	char url[BUFSIZ] = {0};
	snprintf(url, sizeof(url), GSX2JSON_URI_FORMAT, SpreadsheetID, 1);
	using namespace Gxs2Json;
	Config config; Identifier id;
	EXPECT_NO_THROW(parse(url, &config, &id));
	EXPECT_TRUE(config.query.empty());
	EXPECT_FALSE(config.showColumns);
	EXPECT_TRUE(config.useInteger);
	EXPECT_FALSE(config.showRows);
	EXPECT_TRUE(config.showDict);
	EXPECT_STREQ(id.id.c_str(), SpreadsheetID);
	EXPECT_EQ(id.sheet, 1);
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
	using namespace Gxs2Json; Content content;
	EXPECT_NO_THROW(parse(&content, GsxContent.c_str()));
	ASSERT_FALSE(content.timestamp.empty());
	ASSERT_FALSE(content.payload.empty());
}

TEST_F(ParserTests, RowCompactibility)
{
	Client ins("gsx2json.com");
	char url[BUFSIZ] = {0};
	const char* fmt = "/api?id=%s&sheet=%d&columns=false";
	snprintf(url, sizeof(url), fmt, SpreadsheetID, 1);
	auto res = ins.Get(url);
	ASSERT_TRUE(res);
	EXPECT_EQ(res->status, 200);
	const char* ctype = "application/json; charset=utf-8";
	ASSERT_EQ(res->get_header_value("Content-Type"), ctype);
	ASSERT_FALSE(res->body.empty());
	auto a = json::parse(res->body);
	using namespace Gxs2Json; Content content;
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
	ASSERT_TRUE(res);
	EXPECT_EQ(res->status, 200);
	const char* ctype = "application/json; charset=utf-8";
	ASSERT_EQ(res->get_header_value("Content-Type"), ctype);
	ASSERT_FALSE(res->body.empty());
	auto a = json::parse(res->body);
	using namespace Gxs2Json; Content content;
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
	ASSERT_TRUE(res);
	EXPECT_EQ(res->status, 200);
	const char* ctype = "application/json; charset=utf-8";
	ASSERT_EQ(res->get_header_value("Content-Type"), ctype);
	ASSERT_FALSE(res->body.empty());
	auto a = json::parse(res->body);
	using namespace Gxs2Json; Content content;
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

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
#define GSX2JSON_URI_FORMAT "/api?id=%s&sheet=%d&columns=false"
#define SPREADSHEET_URI_FORMAT "/feeds/list/%s/%d/public/values?alt=json"
using namespace httplib;
using namespace nlohmann;

static const char* Host = "spreadsheet.google.com";
static const char* SpreadsheetID = "1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4";

TEST(SSLClient, GoogleSpreadSheet)
{
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	struct stat buffer;
	ASSERT_FALSE(stat(CA_CERT_FILE, &buffer));
	SSLClient ins(Host);
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
	Client ins(Host);
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
	ASSERT_TRUE(config.query.empty());
	ASSERT_FALSE(config.showColumns);
	ASSERT_TRUE(config.useInteger);
	ASSERT_TRUE(config.showRows);
	ASSERT_STREQ(id.id.c_str(), SpreadsheetID);
	ASSERT_EQ(id.sheet, 1);
}

class ParserTests: public ::testing::Test
{
protected:
  
    virtual void SetUp();
   
	std::string GsxContent;
};

void ParserTests::SetUp()
{
	Client ins(Host);
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
	auto object = json::parse(content.payload);
	std::cout << object.dump(4) << std::endl;
}

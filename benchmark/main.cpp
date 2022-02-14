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

#include <benchmark/benchmark.h>
#include <httplib.h>
#include "gsx2json.h"

using namespace httplib;

static const char* SheetName = "worksheet1";
static const char* ApiKey = "YOUR_GOOGLE_SHEET_API_KEY";
static const char* SpreadsheetID = "1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4";

class ConfigFixture : public benchmark::Fixture 
{
public:
    virtual void SetUp(const ::benchmark::State& state) 
    {
        if (GsxContent.empty())
        {
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
            SSLClient ins(SPREADSHEET_HOST);
#else
            Client ins(SPREADSHEET_HOST);
#endif
            char url[BUFSIZ] = {0};
            snprintf(url, sizeof(url), SPREADSHEET_URI_FORMAT, SpreadsheetID, SheetName, ApiKey);
            auto res = ins.Get(url);
            GsxContent = res->body;
        }
    }
    static std::string GsxContent;
};
std::string ConfigFixture::GsxContent;

BENCHMARK_F(ConfigFixture, Row)(benchmark::State& state) 
{
    using namespace Gsx2Json; 
    Config config;
    config.showColumns = false;
    config.showDict = false;
    for (auto _ : state) 
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_F(ConfigFixture, Dict)(benchmark::State& state) 
{
    using namespace Gsx2Json; 
    Config config;
    config.showColumns = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_F(ConfigFixture, Column)(benchmark::State& state) 
{
    using namespace Gsx2Json; 
    Config config;
    config.showDict = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_F(ConfigFixture, Default)(benchmark::State& state) 
{
    using namespace Gsx2Json; 
    Config config;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_F(ConfigFixture, RowMeta)(benchmark::State& state)
{
    using namespace Gsx2Json; 
    Config config;
    config.briefMeta = true;
    config.showColumns = false;
    config.showDict = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_F(ConfigFixture, DictMeta)(benchmark::State& state)
{
    using namespace Gsx2Json; 
    Config config;
    config.briefMeta = true;
    config.showColumns = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_F(ConfigFixture, ColumnMeta)(benchmark::State& state)
{
    using namespace Gsx2Json; 
    Config config;
    config.briefMeta = true;
    config.showDict = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_F(ConfigFixture, DefaultMeta)(benchmark::State& state)
{
    using namespace Gsx2Json; 
    Config config;
    config.briefMeta = true;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_F(ConfigFixture, RowPretty)(benchmark::State& state)
{
    using namespace Gsx2Json; 
    Config config;
    config.prettyPrint = true;
    config.showColumns = false;
    config.showDict = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_F(ConfigFixture, DictPretty)(benchmark::State& state)
{
    using namespace Gsx2Json; 
    Config config;
    config.prettyPrint = true;
    config.showColumns = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_F(ConfigFixture, ColumnPretty)(benchmark::State& state)
{
    using namespace Gsx2Json; 
    Config config;
    config.prettyPrint = true;
    config.showDict = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_F(ConfigFixture, DefaultPretty)(benchmark::State& state)
{
    using namespace Gsx2Json; 
    Config config;
    config.prettyPrint = true;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, GsxContent, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}

BENCHMARK_MAIN();
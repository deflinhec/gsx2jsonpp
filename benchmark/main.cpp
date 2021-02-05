#include <benchmark/benchmark.h>
#include <httplib.h>
#include "gsx2json.h"

using namespace httplib;

static const char* Sample = u8R"(
{
  "version": "1.0",
  "encoding": "UTF-8",
  "feed": {
    "xmlns": "http://www.w3.org/2005/Atom",
    "xmlns$openSearch": "http://a9.com/-/spec/opensearchrss/1.0/",
    "xmlns$gsx": "http://schemas.google.com/spreadsheets/2006/extended",
    "id": {
      "$t": "https://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values"
    },
    "updated": {
      "$t": "2021-02-05T08:22:00.978Z"
    },
    "category": [
      {
        "scheme": "http://schemas.google.com/spreadsheets/2006",
        "term": "http://schemas.google.com/spreadsheets/2006#list"
      }
    ],
    "title": {
      "type": "text",
      "$t": "工作表1"
    },
    "link": [
      {
        "rel": "alternate",
        "type": "application/atom+xml",
        "href": "https://docs.google.com/spreadsheets/d/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/pubhtml"
      },
      {
        "rel": "http://schemas.google.com/g/2005#feed",
        "type": "application/atom+xml",
        "href": "http://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values"
      },
      {
        "rel": "http://schemas.google.com/g/2005#post",
        "type": "application/atom+xml",
        "href": "http://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values"
      },
      {
        "rel": "self",
        "type": "application/atom+xml",
        "href": "http://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values?alt=json"
      }
    ],
    "author": [
      {
        "name": {
          "$t": "falconlegend810518"
        },
        "email": {
          "$t": "falconlegend810518@gmail.com"
        }
      }
    ],
    "openSearch$totalResults": {
      "$t": "4"
    },
    "openSearch$startIndex": {
      "$t": "1"
    },
    "entry": [
      {
        "id": {
          "$t": "https://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values/cokwr"
        },
        "updated": {
          "$t": "2021-02-05T08:22:00.978Z"
        },
        "category": [
          {
            "scheme": "http://schemas.google.com/spreadsheets/2006",
            "term": "http://schemas.google.com/spreadsheets/2006#list"
          }
        ],
        "title": {
          "type": "text",
          "$t": "1"
        },
        "content": {
          "type": "text",
          "$t": "column1: 1b, column2: 11"
        },
        "link": [
          {
            "rel": "self",
            "type": "application/atom+xml",
            "href": "http://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values/cokwr"
          }
        ],
        "gsx$key": {
          "$t": "1"
        },
        "gsx$column1": {
          "$t": "1b"
        },
        "gsx$column2": {
          "$t": "11"
        }
      },
      {
        "id": {
          "$t": "https://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values/cpzh4"
        },
        "updated": {
          "$t": "2021-02-05T08:22:00.978Z"
        },
        "category": [
          {
            "scheme": "http://schemas.google.com/spreadsheets/2006",
            "term": "http://schemas.google.com/spreadsheets/2006#list"
          }
        ],
        "title": {
          "type": "text",
          "$t": "2"
        },
        "content": {
          "type": "text",
          "$t": "column1: 2b, column2: 22"
        },
        "link": [
          {
            "rel": "self",
            "type": "application/atom+xml",
            "href": "http://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values/cpzh4"
          }
        ],
        "gsx$key": {
          "$t": "2"
        },
        "gsx$column1": {
          "$t": "2b"
        },
        "gsx$column2": {
          "$t": "22"
        }
      },
      {
        "id": {
          "$t": "https://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values/cre1l"
        },
        "updated": {
          "$t": "2021-02-05T08:22:00.978Z"
        },
        "category": [
          {
            "scheme": "http://schemas.google.com/spreadsheets/2006",
            "term": "http://schemas.google.com/spreadsheets/2006#list"
          }
        ],
        "title": {
          "type": "text",
          "$t": "3"
        },
        "content": {
          "type": "text",
          "$t": "column1: 3b, column2: 33"
        },
        "link": [
          {
            "rel": "self",
            "type": "application/atom+xml",
            "href": "http://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values/cre1l"
          }
        ],
        "gsx$key": {
          "$t": "3"
        },
        "gsx$column1": {
          "$t": "3b"
        },
        "gsx$column2": {
          "$t": "33"
        }
      },
      {
        "id": {
          "$t": "https://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values/chk2m"
        },
        "updated": {
          "$t": "2021-02-05T08:22:00.978Z"
        },
        "category": [
          {
            "scheme": "http://schemas.google.com/spreadsheets/2006",
            "term": "http://schemas.google.com/spreadsheets/2006#list"
          }
        ],
        "title": {
          "type": "text",
          "$t": "4"
        },
        "content": {
          "type": "text",
          "$t": "column1: 4b, column2: 44"
        },
        "link": [
          {
            "rel": "self",
            "type": "application/atom+xml",
            "href": "http://spreadsheets.google.com/feeds/list/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/1/public/values/chk2m"
          }
        ],
        "gsx$key": {
          "$t": "4"
        },
        "gsx$column1": {
          "$t": "4b"
        },
        "gsx$column2": {
          "$t": "44"
        }
      }
    ]
  }
}
)";

static void BM_ConfigRow(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    config.showColumns = false;
    config.showDict = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigRow);

static void BM_ConfigDict(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    config.showColumns = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigDict);

static void BM_ConfigColumn(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    config.showDict = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigColumn);

static void BM_ConfigDefault(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigDefault);

static void BM_ConfigRowMeta(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    config.briefMeta = true;
    config.showColumns = false;
    config.showDict = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigRowMeta);

static void BM_ConfigDictMeta(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    config.briefMeta = true;
    config.showColumns = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigDictMeta);

static void BM_ConfigColumnMeta(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    config.briefMeta = true;
    config.showDict = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigColumnMeta);

static void BM_ConfigDefaultMeta(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    config.briefMeta = true;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigDefaultMeta);

static void BM_ConfigRowPretty(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    config.prettyPrint = true;
    config.showColumns = false;
    config.showDict = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigRowPretty);

static void BM_ConfigDictPretty(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    config.prettyPrint = true;
    config.showColumns = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigDictPretty);

static void BM_ConfigColumnPretty(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    config.prettyPrint = true;
    config.showDict = false;
    config.showRows = false;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigColumnPretty);

static void BM_ConfigDefaultPretty(benchmark::State& state) 
{
    using namespace Gxs2Json; 
    Config config;
    config.prettyPrint = true;
    for (auto _ : state)
    {
        Content content;
        try {
            parse(&content, Sample, config);
        } 
        catch (const std::exception& _e)
        {
            std::cout << _e.what() << std::endl;
        }
    }
}
BENCHMARK(BM_ConfigDefaultPretty);

BENCHMARK_MAIN();
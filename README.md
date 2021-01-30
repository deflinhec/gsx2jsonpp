# GSX2JSONPP - Google Spreadsheet to JSON API C/C++ service.

Inspired by [55sketch/gsx2Json](https://github.com/55sketch/gsx2json), preserve all functinality of origin and implement additional features.

:coffee: [buy me a coffee](https://ko-fi.com/deflinhec)
 
## :label: Build & Startup

- Run `cd build`
- Run `./build.sh Release`
- Run `cd ../bin`
- Run `./Gsx2Jsonpp`

## :label: Usage

First, you must publish your spreadsheet to the web, using `File -> Publish To Web` in your Google Spreadsheet.

![](https://raw.githubusercontent.com/deflinhec/GodotGoogleSheet/master/screenshots/step01.png) ![](https://raw.githubusercontent.com/deflinhec/GodotGoogleSheet/master/screenshots/step02.png)<img src="https://raw.githubusercontent.com/deflinhec/GodotGoogleSheet/master/screenshots/step03.png" width="250" />

Second, locate to your sheet id and sheet table index.

```
https://docs.google.com/spreadsheets/d/[SPREADSHEET_ID]/edit#gid=[SHEET_NUMBER]
```

![](https://raw.githubusercontent.com/deflinhec/GodotGoogleSheet/master/screenshots/step04.png)

You can then access your readable JSON API using the `/api` endpoint.

```
http://example.com/api?id=SPREADSHEET_ID&sheet=SHEET_NUMBER
```

For file versioning purpose, you can access spreadsheet updated time by using the `/timestamp` endpoint. 

```
http://example.com/timestamp?id=SPREADSHEET_ID&sheet=SHEET_NUMBER
```

## :label: Spreadsheet rule

- Column name begin with `NOEX_` will not export to the final result.

- Make sure to add a left most column represents as an unique integer key.

### :label: Parameters

**id (required):** The ID of your document. This is the big long aplha-numeric code in the middle of your document URL.

**sheet (optional):** The number of the individual sheet you want to get data from. Your first sheet is 1, your second sheet is 2, etc. If no sheet is entered then 1 is the default.

**q (optional):** A simple query string. This is case insensitive and will add any row containing the string in any cell to the filtered result.

**integers (optional - default: true)**: Setting 'integers' to false will return numbers as a string.

**dict (optional - default: true)**: Setting 'dict' to false will return rows and columns view.

**rows (optional - default: true)**: Setting 'rows' to false will return dictionary and columns view.

**columns (optional - default: true)**: Setting 'columns' to false will return dictionary and rows view.

## :bookmark: Example Response

There are three sections to the returned data.

- Columns (containing the names of each column)
- Dictionary (used left most column as primary key)
- Rows (containing each row of data as an object.

```
{
  "columns": {
    "column1": [
      "1b",
      "2b",
      "3b",
      "4b"
    ],
    "column2": [
      11,
      22,
      33,
      44
    ],
    "key": [
      1,
      2,
      3,
      4
    ]
  },
  "dict": {
    "1": {
      "column1": "1b",
      "column2": 11,
      "key": 1
    },
    "2": {
      "column1": "2b",
      "column2": 22,
      "key": 2
    },
    "3": {
      "column1": "3b",
      "column2": 33,
      "key": 3
    },
    "4": {
      "column1": "4b",
      "column2": 44,
      "key": 4
    }
  },
  "rows": [
    {
      "column1": "1b",
      "column2": 11,
      "key": 1
    },
    {
      "column1": "2b",
      "column2": 22,
      "key": 2
    },
    {
      "column1": "3b",
      "column2": 33,
      "key": 3
    },
    {
      "column1": "4b",
      "column2": 44,
      "key": 4
    }
  ]
}

```

[Example spreadsheet](https://docs.google.com/spreadsheets/d/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/edit#gid=0)


## :clipboard: TODO-List

- :white_large_square: json file lockdown

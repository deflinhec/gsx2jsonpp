![Build CMake](https://github.com/deflinhec/gsx2jsonpp/workflows/Build%20CMake/badge.svg?branch=master)
[![GitHub license](https://img.shields.io/github/license/deflinhec/gsx2jsonpp.svg)](https://github.com/deflinhec/gsx2jsonpp/blob/master/LICENSE) 
[![GitHub release](https://img.shields.io/github/release/deflinhec/gsx2jsonpp.svg)](https://github.com/deflinhec/gsx2jsonpp/releases/)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/deflinhec/gsx2jsonpp/graphs/commit-activity)
# GSX2JSONPP - Google Spreadsheet to JSON API C/C++ service.

Inspired by [55sketch/gsx2Json](https://github.com/55sketch/gsx2json), preserve all functinality of origin and implement additional features.

## :speech_balloon: About
One useful feature of Google Spreadsheets is the ability to access the data as JSON by using a particular feed URL. However, this is a bit fiddly to do, and the resulting JSON is pretty unreadable, with usable data buried deep inside objects.

This API connects to your spreadsheet and santizes the data, providing simple, readable JSON for you to use in your app.

## :coffee: Donation

[buy me a coffee](https://ko-fi.com/deflinhec) 
 
## :whale: Using docker image

Make sure [docker engine](https://www.docker.com/products/docker-desktop) has already install in your operating-system.

In this example below I'm going to use `5000` as port, and output log file under `bin/volume` directory.

- Launch with a remote image

    ```
    docker pull deflinhec/gsx2jsonpp:latest
    ```
    
    ```
    docker run -it -d --expose 5000 \
    -v ${PWD}/bin/volume:/workspace \
    -e ARGUMENTS="--host 0.0.0.0 --port 5000" \
    --name gsx2jsonpp-latest deflinhec/gsx2jsonpp
    ```

    <details>
      <summary>Command line for Windows platform</summary>
 
       docker run -it -d --expose 5000 `
       -v ${PWD}\bin\volume:/workspace `
       -e ARGUMENTS="--host 0.0.0.0 --port 5000" `
       --name gsx2jsonpp-latest deflinhec/gsx2jsonpp
      
    </details>

- Launch with a local image
    
    Follow instructions below, :toolbox: Build from source.

    ```
    docker build --rm -t gsx2jsonpp .
    ```
    
    ```
    docker run -it -d --expose 5000 \ 
    -v ${PWD}/bin/volume:/workspace \
    -e ARGUMENTS="--host 0.0.0.0 --port 5000" \
    --name gsx2jsonpp gsx2jsonpp
    ```
    
    <details>
      <summary>Command line for Windows platform</summary>
 
      docker run -it -d --expose 5000 `
      -v ${PWD}\bin\volume:/workspace `
      -e ARGUMENTS="--host 0.0.0.0 --port 5000" `
      --name gsx2jsonpp gsx2jsonpp
      
    </details>

    Avaliable arguments: 
    - -p, --port
    - --host 
    - --cache (file|memory)

      Cache mode is disabled by default, this feature allows client
      to query on specific data version. When cache is configure with
      file mode, cache file will be preserve under 'cache/' folder. 

    - --ssl (true|false)

      SSL mode is disabled by default, if you prefer using SSL mode
      with your certification and private key. Copy your files into
      `${PWD}/bin/volume` and rename as `cert.pem, key.pem`.

After launched, Gsx2Jsonpp should be accessable in your browser [localhost:5000](http://localhost:5000/hi).

Supervisor have been setup within the container to guarantee an auto restart after accidentially crashed(hopefully not).

## :toolbox: Build from source

CMake version must greater than 3.14.0, requires openssl, zlib, brotli installed.

Comparing to docker image, executable size will be way more smaller than using an docker image.

- Using build script

    ```
    git submodule update --init
    ```

    Avaliable argument: Release|Debug|Project (Default: Release)
    
    ```
    bash scripts/build.sh Release
    ```
    
    <details>
      <summary>Command line for Windows platform</summary>
 
      Push-Location scripts; .\build.ps1; Pop-Location
      
    </details>
    
    - Launch with executable
        
        ```
        bin/Gsx2Jsonpp --host 0.0.0.0 -p 5000
        ```

        <details>
          <summary>Command line for Windows platform</summary>

          Push-Location bin; .\Gsx2Jsonpp.exe --host 0.0.0.0 -p 5000; Pop-Location

        </details>

## :label: Spreadsheet configuration

First, you must publish your spreadsheet to the web, using `File -> Publish To Web` in your Google Spreadsheet.

![](https://raw.githubusercontent.com/deflinhec/GodotGoogleSheet/master/screenshots/step01.png) ![](https://raw.githubusercontent.com/deflinhec/GodotGoogleSheet/master/screenshots/step02.png)<img src="https://raw.githubusercontent.com/deflinhec/GodotGoogleSheet/master/screenshots/step03.png" width="250" />

Second, locate to your spreadsheet id and sheet number.

```
https://docs.google.com/spreadsheets/d/[SPREADSHEET_ID]/edit#gid=[SHEET_NUMBER]
```

![](https://raw.githubusercontent.com/deflinhec/GodotGoogleSheet/master/screenshots/step04.png)

You can then access your readable JSON API using the `/api` endpoint.

```
http://example.com/api?id=SPREADSHEET_ID&sheet=SHEET_NUMBER
```

## :dart: Spreadsheet rule

- Column name begin with `NOEX_` will not export to the final result.

- Make sure to add a left most column represents as an unique integer key.

### :memo: Parameters

**id (required):** The ID of your document. This is the big long aplha-numeric code in the middle of your document URL.

**sheet (optional):** The number of the individual sheet you want to get data from. Your first sheet is 1, your second sheet is 2, etc. If no sheet is entered then 1 is the default.

**q (optional):** A simple query string. This is case insensitive and will add any row containing the string in any cell to the filtered result.

**integers (optional - default: true)**: Setting 'integers' to false will return numbers as a string.

**dict (optional - default: true)**: Setting 'dict' to false will return rows and columns view.

**rows (optional - default: true)**: Setting 'rows' to false will return dictionary and columns view.

**columns (optional - default: true)**: Setting 'columns' to false will return dictionary and rows view.

**meta (optional - default: false)**: Setting 'meta' to true will return only meta data.

**pretty (optional - default: false)**: Pretty print the result if sets to true.

## :bookmark: Example Response

[Test localhost:5000 with example spreadsheet](http://localhost:5000/api?id=1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4&sheet=1)

[Example spreadsheet](https://docs.google.com/spreadsheets/d/1-DGS8kSiBrPOxvyM1ISCxtdqWt-I7u1Vmcp-XksQ1M4/edit#gid=0)

There are four sections to the returned data.

- Columns (containing the names of each column)
- Dictionary (used left most column as primary key)
- Rows (containing each row of data as an object)
- Meta (contains short brief of target data)

```
{
 "columns": {
  "key": [
   1,
   2,
   3,
   4
  ],
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
  ]
 },
 "rows": [
  {
   "key": 1,
   "column1": "1b",
   "column2": 11
  },
  {
   "key": 2,
   "column1": "2b",
   "column2": 22
  },
  {
   "key": 3,
   "column1": "3b",
   "column2": 33
  },
  {
   "key": 4,
   "column1": "4b",
   "column2": 44
  }
 ],
 "dict": {
  "1": {
   "key": 1,
   "column1": "1b",
   "column2": 11
  },
  "2": {
   "key": 2,
   "column1": "2b",
   "column2": 22
  },
  "3": {
   "key": 3,
   "column1": "3b",
   "column2": 33
  },
  "4": {
   "key": 4,
   "column1": "4b",
   "column2": 44
  }
 },
 "meta": {
  "columns": {
   "md5": "EAC2F0EF3EA62CEEDD3B65B627B06CBA",
   "bytes": 73
  },
  "rows": {
   "md5": "7767981744A818A7574B4A0B8EBE1C25",
   "bytes": 153
  },
  "dict": {
   "md5": "76C73EAEAFC8BA2ACD890C50E20C1613",
   "bytes": 169
  },
  "time": "2021-02-07T20:00:04.552Z"
 }
}

```


## :clipboard: TODO-List

- :white_check_mark:  docker image

- :white_check_mark:  md5 checksum

- :white_check_mark:  cache


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

#include "cache.h"
#include "gsx2json.h"
#include "utils.h"
#include <sys/stat.h>
#include <nlohmann/json.hpp>
#include <dirent.h>
#include <zlib.h>
#include <fstream>
#include <set>

using namespace nlohmann;

namespace Gsx2Json
{
namespace Cache
{

struct FileImpl : public Impl
{
    FileImpl();

    virtual void save(const std::string& _json, const Identifier& _id);

    virtual void load(std::string& _content, const Identifier& _id) const;

    virtual void flush();

    const std::string Folder = "cache/";
};

struct MemoryImpl : public Impl
{
    virtual void save(const std::string& _json, const Identifier& _id);

    virtual void load(std::string& _content, const Identifier& _id) const;

    virtual void flush();

    std::map<unsigned, std::string> Objects;
};

FileImpl::FileImpl()
{
    mkdir(Folder.c_str(), 0777);
}

void FileImpl::save(const std::string& _json, const Identifier& _id)
{
    using json = ordered_json;
    std::string name = _id.id + "_";
    name += _id.sheet;
    name += ".json";
    const std::string filepath = Folder + name;
    
    Config config;
    Content content;
    std::ofstream output(filepath);
    output << json::parse(_json);
}

void FileImpl::load(std::string& _content, const Identifier& _id) const
{
    using json = ordered_json;
    std::string name = _id.id + "_";
    name += _id.sheet;
    name += ".json";
    const std::string filepath = Folder + name;

    struct stat buffer;
    if (stat(filepath.c_str(), &buffer) != 0)
        return;

    json object;
    std::ifstream input(filepath);
    input >> object;
    _content = object.dump();
}

void FileImpl::flush()
{
    DIR* dir;
    struct dirent *ent;
    std::set<std::string> files;
    const std::string directory = "./" + Folder;
    if ((dir = opendir(directory.c_str())) != nullptr) 
    {
        while ((ent = readdir(dir)) != nullptr) 
        {
            files.insert(Folder + ent->d_name);
        }
        closedir(dir);
    }
    for (const auto& file : files)
    {
        remove(file.c_str());
    }
}

void MemoryImpl::save(const std::string& _json, const Identifier& _id)
{
    std::string name = _id.id + "_";
    name += _id.sheet;
    name += ".json";
    const auto idx = crc32(0, (const Bytef*)&name[0], (unsigned)name.size());

    Config config;
    Content content;
    Objects[idx] = _json;
}

void MemoryImpl::load(std::string& _content, const Identifier& _id) const
{
    std::string name = _id.id + "_";
    name += _id.sheet;
    name += ".json";
    const auto idx = crc32(0, (const Bytef*)&name[0], (unsigned)name.size());

    std::map<unsigned, std::string>::const_iterator it;
    if ((it = Objects.find(idx)) == Objects.end())
        return;
    
    _content = it->second;
}

void MemoryImpl::flush()
{
    Objects.clear();
}

Manager::Manager(const Type& _type)
{
    switch (_type)
    {
    case Type::MEMORY:
        Implement = std::make_unique<MemoryImpl>();
        break;
    case Type::FILE:
        Implement = std::make_unique<FileImpl>();
        break;
    default:
        break;
    }
}

void Manager::save(const std::string& _json, const Identifier& _id)
{
    if (Implement == nullptr)
        return;
    std::lock_guard<std::mutex> lock(Mutex);
    Implement->save(_json, _id);
}

bool Manager::load(std::string& _json, const Identifier& _id) const
{
    if (Implement == nullptr)
        return false;
    std::lock_guard<std::mutex> lock(Mutex);
    Implement->load(_json, _id);
    return !_json.empty();
}

void Manager::flush()
{
    if (Implement == nullptr)
        return;
    std::lock_guard<std::mutex> lock(Mutex);
    Implement->flush();
}
}
}

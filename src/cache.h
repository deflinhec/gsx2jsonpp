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

#include <string>
#include <memory>
#include <mutex>

namespace Gsx2Json
{
struct Config;
struct Content;
struct Identifier;
namespace Cache
{
struct Impl
{
    virtual void save(const std::string& _json, const Identifier& _id) = 0;

    virtual void load(std::string& _content, const Identifier& _id) const = 0;

    virtual void flush() = 0;
};

class Manager
{
public:
    enum class Type
    {
        MEMORY, FILE,
    };

    Manager(const Type& _type);

    void save(const std::string& _json, const Identifier& _id);

    bool load(std::string& _json, const Identifier& _id) const;

    void flush();

private:
    mutable std::mutex Mutex;
    std::unique_ptr<Impl> Implement;
};
}
}

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

#include "utils.h"
#include <openssl/md5.h>

namespace Gxs2Json
{

bool is_number(const std::string& _s)
{
	if(_s.empty()) 
		return false;
	
	if ((!isdigit(_s[0])) && (_s[0] != '-') && (_s[0] != '+'))
		return false;

	char* p = nullptr;
	strtod(_s.c_str(), &p);

	return (*p == 0);
}

std::string md5(const std::string& _s)
{
	MD5_CTX ctx; MD5_Init(&ctx);
	MD5_Update(&ctx, (unsigned char *)_s.data(), _s.size());
	unsigned char md5[MD5_DIGEST_LENGTH]; MD5_Final(md5, &ctx);
	std::string checksum; checksum.reserve(32);
	for (std::size_t i = 0; i != 16; ++i)
	{
		checksum += "0123456789ABCDEF"[md5[i] / 16];
		checksum += "0123456789ABCDEF"[md5[i] % 16];
	}
	return checksum;
}

}
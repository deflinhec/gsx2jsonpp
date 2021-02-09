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

#include <chrono>
#include <cstdio>
#include <dirent.h>
#include <httplib.h>
#include <cxxopts.hpp>
#include <uriparser/Uri.h>
#include "gsx2json.h"

#define SERVER_CERT_FILE "./cert.pem"
#define SERVER_PRIVATE_KEY_FILE "./key.pem"
#define CA_CERT_FILE "./ca-bundle.crt"
using namespace httplib;

std::string dump_headers(const Headers &headers)
{
	std::string s;
	char buf[BUFSIZ];
	
	for (auto it = headers.begin(); it != headers.end(); ++it) {
		const auto &x = *it;
		snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
		s += buf;
	}
	
	return s;
}

std::string log(const Request &req, const Response &res)
{
	std::string s;
	char buf[BUFSIZ];
	
	s += "================================\n";
	
	snprintf(buf, sizeof(buf), "%s %s %s", req.method.c_str(),
			 req.version.c_str(), req.path.c_str());
	s += buf;
	
	std::string query;
	for (auto it = req.params.begin(); it != req.params.end(); ++it)
	{
		const auto &x = *it;
		snprintf(buf, sizeof(buf), "%c%s=%s",
				 (it == req.params.begin()) ? '?' : '&', x.first.c_str(),
				 x.second.c_str());
		query += buf;
	}
	snprintf(buf, sizeof(buf), "%s\n", query.c_str());
	s += buf;
	
	s += dump_headers(req.headers);
	
	s += "--------------------------------\n";
	
	snprintf(buf, sizeof(buf), "%d %s\n", res.status, res.version.c_str());
	s += buf;
	s += dump_headers(res.headers);
	s += "\n";
	
	if (!res.body.empty()) { s += res.body; }
	
	s += "\n";
	
	return s;
}

int main(int argc, char *argv[])
{
	const char* descp = R"(
	A service reduces significant large amount of bytes
	responsed from spreadsheet.google.com.
	
	SSL mode:
		SSL mode is disabled by default, if you prefer using SSL mode
		with your certification and private key. Copy your files right
		next to Gsx2Jsonpp and rename as `cert.pem, key.pem`.
	)";
	cxxopts::Options options("Gsx2Jsonpp", descp);
	
	options.add_options()
		("ssl", "SSL mode", cxxopts::value<bool>()->default_value("false"))
		("host", "Host", cxxopts::value<std::string>()->default_value("localhost"))
		("p,port", "Port", cxxopts::value<unsigned short>()->default_value("8080"))
		("h,help", "Print usage")
	;
	
	auto result = options.parse(argc, argv);
	
	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		return 0;
	}
	
	std::shared_ptr<Server> ins;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	if (result["ssl"].as<bool>())
	{
		struct stat buffer;
		bool missing = stat(SERVER_CERT_FILE, &buffer);
		if ((missing |= (bool)stat(SERVER_PRIVATE_KEY_FILE, &buffer)))
		{
			EVP_PKEY *key = EVP_PKEY_new();
			RSA *rsa = RSA_new();
			int ret = 0;
			BIGNUM* bne = BN_new();
			ret = BN_set_word(bne, RSA_F4);
			ret = RSA_generate_key_ex(rsa, 2048, bne, NULL);
			EVP_PKEY_assign_RSA(key, rsa);
			
			X509* x509 = X509_new();
			ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
			X509_gmtime_adj(X509_get_notBefore(x509), 0);
			X509_gmtime_adj(X509_get_notAfter(x509), 31536000L);
			
			X509_set_pubkey(x509, key);
			X509_NAME * name;
			name = X509_get_subject_name(x509);
			X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC,
									   (unsigned char *)"TW", -1, -1, 0);
			X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC,
									   (unsigned char *)"Gsx2Jsonpp", -1, -1, 0);
			X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
									   (unsigned char *)"localhost", -1, -1, 0);
			X509_set_issuer_name(x509, name);
			X509_sign(x509, key, EVP_sha1());
			
			FILE* key_file = fopen(SERVER_PRIVATE_KEY_FILE, "wb");
			PEM_write_PrivateKey(key_file, key, NULL, NULL, 10, NULL, NULL);
			fclose(key_file);
			
			key_file = fopen(SERVER_CERT_FILE, "wb");
			PEM_write_X509(key_file, x509);
			fclose(key_file);
			
			BN_free(bne);
			RSA_free(rsa);
			X509_free(x509);
		}
		ins = std::make_shared<SSLServer>(SERVER_CERT_FILE, SERVER_PRIVATE_KEY_FILE);
	}
#else
	if (result["ssl"].as<bool>())
	{
		std::cout << "not support for ssl mode" << std::endl;
	}
#endif
	if (ins == nullptr)
	{
		ins = std::make_shared<Server>();
	}
	
	Server& srv = *ins;
	if (!srv.is_valid())
	{
		std::cout << "server has an error..." << std::endl;
		return -1;
	}
	
	srv.Get("/", [=](const Request & /*req*/, Response &res) {
		res.set_redirect("/hi");
	});
	
	srv.Get("/hi", [](const Request & /*req*/, Response &res) {
		res.set_content("Hello World!\n", "text/plain");
	});
	
	srv.Get("/api", [](const Request & req, Response &res) {
		using namespace Gsx2Json;
		Config config; Identifier id;
		try {
			parse(req.target, &config, &id);
		}
		catch (const std::exception& e) {
			res.set_content(e.what(), "text/plain");
		}
		Client cli(SPREADSHEET_HOST);
		char buf[BUFSIZ];
		snprintf(buf, sizeof(buf), SPREADSHEET_URI_FORMAT, id.id.c_str(), id.sheet);
		if (auto cli_res = cli.Get(buf))
		{
			if (cli_res->status == 200)
			{
				Content content;
				try {
					parse(&content, cli_res->body, config);
					res.set_content(content.payload, "application/json");
				}
				catch (const std::exception& e)
				{
					std::cout << "exception: " << e.what() << std::endl;
				}
			}
			else
			{
				std::cout << SPREADSHEET_HOST << " connection failed: " << cli_res->status << std::endl;
			}
		}
		else
		{
			std::cout << SPREADSHEET_HOST << " connection failed " << std::endl;
		}
	});
	
	srv.set_error_handler([](const Request & /*req*/, Response &res) {
		const char *fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
		char buf[BUFSIZ];
		snprintf(buf, sizeof(buf), fmt, res.status);
		res.set_content(buf, "text/html");
	});
	
	srv.set_logger([](const Request &req, const Response &res) {
		std::cout << log(req, res) << std::endl;
	});
	
	const std::string host = result["host"].as<std::string>();
	const unsigned short port = result["port"].as<unsigned short>();
	std::cout << "listening " << host;
	std::cout << ":" << port << std::endl;
	srv.listen(host.c_str(), port);
	
	return 0;
}

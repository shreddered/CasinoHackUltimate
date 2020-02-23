#ifndef __VK_API_HPP_INCLUDED
#define __VK_API_HPP_INCLUDED

#include <curl/curl.h>
#include <string>
#include "json.hpp"
#include <cstdlib>
#include <fstream>

#if __cplusplus < 201103L
	#include <iostream>
#endif

using std::string;
using json = nlohmann::json;


namespace http {

class error : public std::runtime_error {
public:
	error(const string& msg) noexcept: std::runtime_error(msg) {}
	virtual ~error() noexcept = default;	
};

static size_t write_function(void* ptr, size_t size, size_t nmemb, std::string* data) {
	data->append(static_cast<char*>(ptr), size * nmemb);
	return size * nmemb;
}

string post(const string& url, const string& params) {
	auto curl = curl_easy_init();
	if (curl) {
#define setopt(opt, data) curl_easy_setopt(curl, opt, data)
		setopt(CURLOPT_URL, url.c_str());
		setopt(CURLOPT_WRITEFUNCTION, write_function);	
		string response;
		setopt(CURLOPT_WRITEDATA, &response);
		setopt(CURLOPT_POSTFIELDS, params.c_str());
		auto res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		curl = nullptr;
		if (res != CURLE_OK) {
#if __cplusplus < 201103L
			std::cerr << "Пошла фигня: post не работает" << std::endl;
			exit(1);
#else
			throw error("Пошла фигня: post не работает");
#endif
		}
#undef setopt
		return response;
	}
	else {
#if __cplusplus < 201103L
		std::cerr << "Пошла фигня: не запустил curl" << std::endl;
		exit(1);
#else
		throw error("Пошла фигня не запустил curl");
#endif
	}
}

string get(const string& url) {
	auto curl = curl_easy_init();
	if (curl) {
#define setopt(opt, data) curl_easy_setopt(curl, opt, data)
		setopt(CURLOPT_URL, url.c_str());
		setopt(CURLOPT_WRITEFUNCTION, write_function);
		string response;
		setopt(CURLOPT_WRITEDATA, &response);
		auto res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		curl = nullptr;
		if (res != CURLE_OK) {
#if __cplusplus < 201103L
			std::cerr << "get не работает" << std::endl;
			exit(1);
#else
			throw error("get не работает");
#endif	
		}
#undef setopt 
		return response;
	}
	else
#if __cplusplus < 201103L
		std::cerr << "не запустил curl" << std::endl;
		exit(1);
#else
		throw error("не запустил curl");
#endif
}

}; // namespace http

namespace VK {

const string API_VERSION = "5.101";

class error : public std::runtime_error {
public:
	error(const string& msg) noexcept: std::runtime_error(msg) {}
	virtual ~error() noexcept = default;
};

class Client {
private:
	string token;
	const string url = "https://api.vk.com/method/";
public:
	explicit Client(const string& token) noexcept: token(token) {}
	json method(const string& name, const string& params = "") const {
		auto j = json::parse(http::post(this->url + name, "&access_token=" + this->token + "&v=" + API_VERSION + params));
		if (j["response"].is_null())
			throw error("Error from VK:\n" + j.dump(4));
		return j["response"];
	}
	void send_message(const string& msg, const string& peer_id) const {
		srand(time(0));
		//std::cout << rand() % 2147483648 << std::endl;
		this->method("messages.send", 
				"&random_id=" + std::to_string(rand() % 2147483648) + 
				"&message=" + msg + 
				"&peer_id=" + peer_id);
		// don't need returned id
	}
	json get_last_message() const {
		auto code = string(R"(var message = API.messages.getHistory({"peer_id":-181671440, "count":1}).items[0];
if (message.attachments.length == 0) {
	return {"text":message.text};
}
else {
	var sizes = message.attachments[0].photo.sizes;
	return {"url":sizes[sizes.length - 1].url};
})");
		return this->method("execute", "&code=" + code);
	}
	json get_last_message(const string& peer_id) const {
		auto message = this->method("messages.getHistory", "&peer_id=" + peer_id + "&count=1")["items"][0];
		if(message["attachments"].size()) {
			auto sizes = message["attachments"][0]["photo"]["sizes"];
			json j;
			j["url"] = sizes[sizes.size() - 1]["url"];
			return j;
		}
		else {
			json j;
			j["text"] = message["text"];
			return j;
		}
	}
	inline void clear() noexcept {
		token.clear();
	}
};


};

namespace image {

inline void download(const string& url) {
	auto data = http::get(url);
	std::fstream f("image.jpg", std::fstream::in | std::fstream::out | std::fstream::trunc);
	f << data;
	f.close();
}

}; //namespace image

#endif /* __VK_API_HPP_INCLUDED */

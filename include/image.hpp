#ifndef __IMAGE_HPP_INCLUDED
#define __IMAGE_HPP_INCLUDED

#include <cstdlib>
#include <regex>
#include <fstream>
#include <string>
#include <sstream>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <cstdlib>
#include <iostream>
#include <tuple>

using std::wstring, std::string;


namespace image {

constexpr bool is_junk(const wchar_t& c) noexcept {
	return !( (c >= '0' && c <= '9') || c == '+' || c == '-' || c == L'—'|| c == '/' || c == '*' || 
			c == ']' || c == '[' || c == 'H' || c == 'N' || c == '\'' ||
			c == '"' || c == 'l' || c == 'I' || c == 'z' || c == 'Z' || c == 'O' || c == 'o' || c == 'C' || c == 'i' || c == 'c' || c == L'°' || c == '(' || c == ')');
}

wstring wrap(const wstring& str) noexcept {
	wstring result;
	for(auto it = str.begin(); it != str.end(); ++it) // для s[i] может быть переполнение из-за int i
		if(wchar_t c = *it; !is_junk(c) ) {
			if (c == '"' || c == L'°' || c == '\'')
				result += '*';
			else if (c == ']' || c == '[' || c == 'l' || c == 'I' || c == 'i')
				result += '1';
			else if (c == 'z' || c == 'Z')
				result += '2';
			else if (c == 'o' || c == 'O')
				result += '0';
			else if (c == 'c' || c == 'C')
				result += '(';
			else if (c == 'H' || c == 'N')
				result += L"11";
			else
				result += c;
		}
		else if(*it == '=')
			break;
    std::wregex r1(L"([0-9]+)(\\()", std::wregex::extended),
                r2(L"(\\))([0-9]+)", std::wregex::extended);
	result = std::regex_replace(result, r1, L"\\1*\\2", std::regex_constants::format_sed);
    return std::regex_replace(result, r2, L"\\1*\\2", std::regex_constants::format_sed);
}

wstring parse(const string& path_to_image, const string& filename) {
	auto api = tesseract::TessBaseAPI();
	if(api.Init("data", "eng")) {
#if __cplusplus < 201103L
		std::cerr << "Пошла хуйня: не могу открыть tesseract" << '\n';
		exit(1);
#else
		throw std::runtime_error("Пошла хуйня: не могу открыть tesseract");
#endif	
	}
	auto image = pixRead(path_to_image.c_str());
	auto box = boxCreate(80, 80, 700-160, 540-160);
	image = pixClipRectangle(image, box, NULL);
    api.SetVariable("tessedit_char_whitelist", "0123456789-+=*()\"'zli");
	api.SetPageSegMode(tesseract::PSM_AUTO);
	api.SetImage(image);
	auto c = api.GetUTF8Text();
	auto size = strlen(c) + 1;
	wchar_t dst[size];
	mbstowcs(dst, c, size); 
	api.End();
	pixDestroy(&image);
	boxDestroy(&box);
	std::wcout << dst << L"--->";
	return wrap(wstring(dst));
}

//void download(const string&); // writes raw data to ./image.jpg

/*typedef std::tuple<string, string, string, string, string> response_t;

response_t split_fraction(const string& path_to_image) {
    auto api = tesseract::TessBaseAPI();
	if(api.Init("data", "eng")) {
#if __cplusplus < 201103L
		std::cerr << "Пошла хуйня: не могу открыть tesseract" << '\n';
		exit(1);
#else
		throw std::runtime_error("Пошла хуйня: не могу открыть tesseract");
#endif
    }*/
	/*
	 * Using boxCreate(x, y, width, height) and pixClipRectangle(image, box, nullptr) o crop an image
	 */ 
    /*auto image = pixRead(path_to_image.c_str());
#define _rw 60
#define _rh 40
    auto box1 = boxCreate(); 
}*/

}; //namespace image

#endif /* __IMAGE_HPP_INCLUDED */

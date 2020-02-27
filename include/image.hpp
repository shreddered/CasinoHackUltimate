#ifndef __IMAGE_HPP_INCLUDED
#define __IMAGE_HPP_INCLUDED

#include <cstdlib>
#include <regex>
#include <fstream>
#include <stdexcept>
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

class Image;

class Box {
    int32_t x, y, w, h;
    BOX* box = nullptr;
public:
    friend class Image;
    explicit Box(const int32_t& x, const int32_t& y, const int32_t& w, const int32_t& h) 
        noexcept : x(x), y(y), w(w), h(h), box(boxCreate(x, y, w, h)) {}
    Box(const Box& other) = delete;
    Box& operator =(const Box& other) = delete;
    explicit Box(Box&& other) noexcept : box(other.box), x(other.x), y(other.y),
                                                         w(other.w), h(other.h)
    {
        other.box = nullptr;
    }
    Box& operator =(Box&& other) noexcept {
        box = other.box;
        other.box = nullptr;
        x = other.x;
        y = other.y;
        w = other.w;
        h = other.h;
        return *this;
    }
    virtual ~Box() noexcept {
        if (box)
            boxDestroy(&box);
    }
};

class Image {
    PIX* pix;
public:
    explicit inline Image(const std::string& name) noexcept : pix(pixRead(name.c_str())) {}
    explicit inline Image(Image&& other) noexcept : pix(other.pix) {
        other.pix = nullptr;
    }
    explicit inline Image(PIX* ptr) noexcept : pix(ptr) {}
    inline Image& operator =(Image&& other) noexcept {
        pix = other.pix;
        other.pix = nullptr;
        return *this;
    }
    Image(const Image& other) = delete;
    Image& operator =(const Image& other) = delete;
    virtual inline ~Image() noexcept {
        if (pix)
            pixDestroy(&pix); 
    }
    inline Image clipRectangle(const Box& box) noexcept {
        return Image(pixClipRectangle(pix, box.box, nullptr)); 
    }
    inline PIX* ptr() {
        return pix;
    }
};

wstring parse(const string& path_to_image, const string& filename) {
	auto api = tesseract::TessBaseAPI();
	if(api.Init("data", "eng"))
		throw std::runtime_error("Пошла хуйня: не могу открыть tesseract");
	//auto image = pixRead(path_to_image.c_str());
    Image image(path_to_image); 
	Box box(80, 80, 700-160, 540-160);
	image = image.clipRectangle(box);
    api.SetVariable("tessedit_char_whitelist", "0123456789-+=*()\"'zli");
	api.SetPageSegMode(tesseract::PSM_AUTO);
	api.SetImage(image.ptr());
	auto c = api.GetUTF8Text();
	auto size = strlen(c) + 1;
	wchar_t dst[size];
	mbstowcs(dst, c, size); 
	api.End();
	std::wcout << dst << L"--->";
	return wrap(wstring(dst));
}

namespace {
    struct Temp {
        /*TODO: implement tuple-like struct with additional method processing 
         * (private?) fields
         */
    }; // struct
}

std::string parse12lvl(const string& name) {
    auto api = tesseract::TessBaseAPI();
	if(api.Init("data", "eng"))
		throw std::runtime_error("Пошла хуйня: не могу открыть tesseract");
	//auto image = pixRead(path_to_image.c_str());
    Image image(name); 
    //TODO: make 5 boxes for 4 numbers and operator
    /*
     *
     *    num1         num3
     * ---------- op ---------
     *    num2         num4
     *
     */    
    Box b1(220 - 60, 214 - 40, 120, 90), // num1
        b2(220 - 60, 334 - 40, 120, 90), // num2
        op(350 - 60, 214 - 40, 120, 140), // op
        b3(480 - 60, 214 - 40, 120, 90),
        b4(480 - 60, 334 - 40, 120, 90);
    image = image.clipRectangle(b4);
    pixWrite("test.png", image.ptr(), IFF_PNG);
    api.SetImage(image.ptr());
    auto c = api.GetUTF8Text();
    api.End();
    return std::string(c);
}

}; //namespace image

#endif /* __IMAGE_HPP_INCLUDED */

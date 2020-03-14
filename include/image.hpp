#ifndef __IMAGE_HPP_INCLUDED
#define __IMAGE_HPP_INCLUDED

#include <cstdlib>
#include <cstring>
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
#include <memory>

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
    explicit Box() noexcept {}
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
    struct __temp {
        int num1, num2, num3, num4;
        char op;
    }; // struct
    inline void process(__temp& tmp) {
        if (tmp.num1 == 47)
            tmp.num1 = 7;
        if (tmp.num2 == 47)
            tmp.num2 = 7;
        if (tmp.num3 == 47)
            tmp.num3 = 7;
        if (tmp.num4 == 47)
            tmp.num4 = 7;
    }
}

auto parse12lvl(const string& name) {
    auto api = tesseract::TessBaseAPI();
	if(api.Init("data", "eng"))
		throw std::runtime_error("Пошла хуйня: не могу открыть tesseract");
	//auto image = pixRead(path_to_image.c_str());
    enum option {
        num1,
        num2,
        num3,
        num4,
        op  
    };
    auto get = [&name, &api](const option& opt) {
        Image image(name);
        Box b;
        switch (opt) {
            case option::num1:
                b = std::move(Box(220 - 80, 214 - 40, 140, 90));
                break;
            case option::num2:
                b = std::move(Box(220 - 60, 334 - 40, 120, 90));
                break;
            case option::num3:
                b = std::move(Box(480 - 60, 214 - 40, 120, 90));
                break;
            case option::num4:
                b = std::move(Box(480 - 60, 334 - 40, 120, 90));
                break;
            case option::op:
                b = std::move(Box(350 - 60, 214 - 40, 120, 140));
                break;
            default:
                break;
        }
        image = image.clipRectangle(b);
        api.SetImage(image.ptr());
        auto buf = api.GetUTF8Text();
        struct __lambda_result {
            union {
                int num;
                char op;
            };
        } r;
        std::cout << strlen(buf) << std::endl;
        switch (buf[0]) {
            case '+':
                [[fallthrough]];
            case '-':
                [[fallthrough]];
            case '/':
                [[fallthrough]];
            case '*':
                r.op = buf[0];
                return r;
                break;
            default:
                r.num = atoi(buf);
                return r;
                break;
        }
    };
    /*
     *
     *    num1         num3
     * ---------- op ---------
     *    num2         num4
     *
     */
    auto _num1 = get(option::num1).num,
         _num2 = get(option::num2).num,
         _num3 = get(option::num3).num,
         _num4 = get(option::num4).num;
    auto _op = get(option::op).op;
    std::cout << _num1 << '/' << _num2 << ' ' << _op  << ' ' 
              << _num3 << '/' << _num4 << '\n';
    api.End();
    __temp result = { _num1, _num2, _num3, _num4, _op };
    process(result);
    return result;
}

}; //namespace image

#endif /* __IMAGE_HPP_INCLUDED */

#ifndef __PROCESS_HPP_INCLUDED
#define __PROCESS_HPP_INCLUDED

#include <string>
#include <algorithm>


namespace process {

enum class replace_type {
	first, // 4 -> +
	second	
};

template<class T = std::wstring> void correct(T& str, const replace_type& type) {
	if (type == replace_type::first)
		std::replace(str.begin(), str.end(), '4', '+');
	else
		str.replace(str.find("4+"), 2, "+");
}

};

#endif /* __PROCESS_HPP_INCLUDED */

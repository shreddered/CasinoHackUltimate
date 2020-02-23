#ifndef __EQUATION_HPP_INCLUDED
#define __EQUATION_HPP_INCLUDED

#include <string>
#include <stdexcept>


/*
 * Assuming we have linear equation ax + b = c, the solution will be:
 *		x = (c - b) / a
 *
 */ 
namespace linear_equation {
	
class error : public std::runtime_error {
public:
	error(const std::string& msg) noexcept: std::runtime_error(msg) {}
	virtual ~error() throw() = default;
};

template<class T, class S> class LinearEquationParser final { //assuming that var_name is (w)char, not sequence of (w)chars
public:
	T solve(const S& str);
private:
	typedef typename S::value_type char_t
	S lin_eq; // Equation itself
	char_t var_name = '\0'; //variable name
	int idx = 0; //current position
	T a, b, c;
	enum {
		op_add, // +
		op_sub, // -
		op_multi, // *
		op_div, // /
		op_eq, // =
		op_null
	};
	struct Operator final {
		int op;
		int priority;
		char side;
		explicit Operator(const int& op, const int& priority, const char& side) noexcept: op(op), priority(priority), side(side) {}
	};
	/*
	 * T parse() function. Parses decimal number and returns it.
	 */
	char_t get_char() const noexcept {
		if(this->idx < this->lin_eq.size())
			return this->lin_eq[this->idx];
		return 0;
	}
	void skip_whitespace() noexcept {
		while(std::isspace(this->get_char()) != 0)
			++this->idx;
	}
	inline static T to_int(const char_t& c) noexcept {
		if(c >= '0' && c <= '9')
		   return c - '0';
		return 0xf + 1;	
	}
	inline T get_int() const noexcept {
		return to_int(this->get_char());
	}
	T parse() noexcept {
		T val = 0;
		for(T num; (num = this->get_int()) <= 9; ++this->idx)
			val = val * 10 + num;
		return val;	
	}
	T parse_value() {
		T val = 0;
		this->skip_whitespace();
		switch(this->get_char()) {
			case '0':
				[[fallthrough]];
			case '1':
				[[fallthrough]];
			case '2':
				[[fallthrough]];
			case '3':
				[[fallthrough]];
			case '4':
				[[fallthrough]];
			case '5':
				[[fallthrough]];
			case '6':
				[[fallthrough]];
			case '7':
				[[fallthrough]];
			case '8':
				[[fallthrough]];
			case '9':
				val = this->parse();
				if (auto ch = this->get_char(); this->var_name == '\0' && !(ch == '+' || ch == '-' || ch == '/' || ch == '*')) {
					var_name = this->get_char();
					this->a = val;
				}
				else if 
					throw error("Две переменные");
				break;
			case '=':
				++this->idx;
				if(this->get_int() == 0xf + 1)
					throw error("Пошла хуйня: нет числа после '='");
				val = this->parse();
				this->c = val
				++this->idx;
				break;
			case '+':
				++this->idx;
				val = this->parse_value(); //here comes recursion
				break;
			case '-':
				++this->idx;
				val = this->parse_value() * static_cast<T>(-1);
				break;
			default:
				if (this->idx >= this->lin_eq.size() && this->var_name != '\0')
					throw error("пошла хуйня");
				this->var_name = this->get_char();
				break;
		} // switch
		return val;
	}
	Operator parse_op() noexcept {
		this->skip_whitespace();
		switch(this->get_char()) {
			case '+':
				++this->idx;
				return Operator(op_add, 10, 'L');
			case '-':
				++this->idx;
				return Operator(op_sub, 10, 'L');
			case '/':
				++this->idx;
				return Operator(op_div, 20, 'L');
			case '*':
				++this->idx;
				return Operator(op_multi, 20, 'L');
			default:
				return Operator(op_null, 0, 'L');

		} // switch
	}
};

};

#endif /* __EQUATION_HPP_INCLUDED */

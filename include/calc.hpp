#ifndef __CALC_HPP_INCLUDED
#define __CALC_HPP_INCLUDED

#include <string>
#include <stack>
#include <cmath>
#include <stdexcept>

using std::string, std::stack, std::wstring;


namespace calculator {

class error : public std::runtime_error {
public:
	explicit error(const string& msg) noexcept: std::runtime_error(msg) {}
	virtual ~error() noexcept = default;
};

template <class T, class S>
class ExpressionParser final {
private:
	enum {
		op_add, // +
		op_sub, // -
		op_multi, // *
		op_div, // /
		op_pow, // **
		op_null
	};
	struct Operator final {
		int op;
		int priority;
		char side;
		explicit constexpr Operator(const int& op, const int& priority, const char& side) noexcept: op(op), priority(priority), side(side) {}
	};
	struct OperatorValue final {
		Operator op;
		T value;
		explicit constexpr OperatorValue(const Operator& op, const T& value) noexcept : op(op), value(value) {}
		inline constexpr const int& priority() const noexcept {
			return this->op.priority;
		}
		inline constexpr bool is_null() const {
			return this->op.op == op_null;
		}
	};
	S expr;
	int idx;
	stack<OperatorValue> _stack;
	constexpr T calculate(const T& val_1, const T& val_2, const Operator& op) const noexcept {
		switch(op.op) {
			case op_add: 
				return val_2 + val_1;
			case op_sub:
				return val_1 - val_2;
			case op_multi:
				return val_1 * val_2;
			case op_div:
				return val_1 / val_2;
			case op_pow:
				return pow(val_1, val_2);
			default:
				return 0;
		}
	}
	constexpr typename S::value_type get_char() const noexcept {
		if(this->idx < std::size(this->expr))
			return this->expr[this->idx];
		return 0;
	}
	constexpr void skip_whitespace() noexcept {
		while(std::isspace(this->get_char()) != 0)
			++this->idx;
	}
	constexpr Operator parse_op() noexcept {
		this->skip_whitespace();
		switch(get_char()) {
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
				if (this->get_char() != '*')
					return Operator(op_multi, 20, 'L');
				++this->idx;
				return Operator(op_pow, 30, 'R');
			default:
				return Operator(op_null, 0, 'L');
		}
	}
	inline static constexpr T to_int(const typename S::value_type& c) noexcept {
		if (c >= '0' && c <= '9')
			return c - '0';
		return 0xf + 1;
	}
	inline constexpr T get_int() const noexcept {
		return to_int(this->get_char());
	}
	constexpr T parse() noexcept {
		T val = 0;
		for(T num; (num = this->get_int()) <= 9; ++idx)
			val = val * 10 + num;
		return val;
	}
	constexpr T parse_value() {
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
				break;
			case '(':
				++this->idx;
				val = this->parse_expr();
				this->skip_whitespace();
				if (this->get_char() != ')' && this->idx >= std::size(this->expr))
					throw error("Пошла фигня: ')' не нашёл");
				++this->idx;
				break;
			case '+':
				++this->idx;
				val = parse_value();
				break;
			case '-':
				++this->idx;
				val = parse_value() * static_cast<T>(-1);
				break;
			default:
				if (this->idx >= std::size(this->expr))
					throw error("Пошла фигня");
				break;
		} //switch
		return val;
	}
	constexpr T parse_expr() {
		this->_stack.push(OperatorValue(Operator(op_null, 0, 'L'), 0));
		T val = this->parse_value();
		while(!_stack.empty()) {
			auto op = Operator(this->parse_op());
			while(op.priority < this->_stack.top().priority() || (op.priority == this->_stack.top().priority() && op.side == 'L')) {
				if (this->_stack.top().is_null()) {
					this->_stack.pop();
					return val;
				}
				val = calculate(this->_stack.top().value, val, this->_stack.top().op);
				this->_stack.pop();
			}
			this->_stack.push(OperatorValue(op, val));
			val = this->parse_value();
		}
		return 0;
	}
public:
	constexpr T eval(const S& expr) {
		T result = 0;
		this->idx = 0;
		this->expr = expr;
		result = this->parse_expr();
		return result;
	}
};

template <class T, class S>
inline constexpr T eval(const S& expr) {
	auto parser = ExpressionParser<T, S>();
	return parser.eval(expr);
}

inline constexpr int eval(const string& expr) {
	return eval<int, string>(expr);
}

template<class T>
class basic_fraction final {
public:
    T num, den;
    explicit constexpr basic_fraction(const T& _num, const T& _den) {
        if (!_den)
            throw std::invalid_argument("Denominator must be != 0");
        this->num = _num;
        this->den = _den;
        this->reduce();
    }
    inline constexpr basic_fraction<T>& reverse() noexcept {
        std::swap(this->num, this->den);
        return *this;
    }
    inline string dump() const noexcept {
        return (((this->num < 0) != (this->den < 0)) ? "-" : "") + std::to_string(abs(this->num)) + 
            ((abs(this->den) == 1) ? "" : ('/' + std::to_string(abs(this->den))));
    }
    /*
     * '+', '-', '*', '/' unary/binary operators
     * Note that reduce happens on construction
     */ 
    template<class T1> 
        friend inline constexpr basic_fraction<T1> operator +(const basic_fraction<T1>& f1, const basic_fraction<T1>& f2) noexcept {
            return basic_fraction<T1>(f1.num * f2.den + f2.num * f1.den, f1.den * f2.den);
        }
    template<class T1>
        friend inline constexpr basic_fraction<T1> operator -(const basic_fraction<T1>& f1, const basic_fraction<T1>& f2) noexcept {
            return f1 + (-f2);
        }
    template<class T1>
        friend inline constexpr basic_fraction<T1> operator *(const basic_fraction<T1>& f1, const basic_fraction<T1>& f2) noexcept {
            return basic_fraction<T1>(f1.num * f2.num, f2.den * f1.den);
        }
    template<class T1>
        friend inline constexpr basic_fraction<T1> operator /(const basic_fraction<T1>& f1, const basic_fraction<T1>& f2) noexcept {
            return f1 * f2.reverse();
        }
    template<class T1>
        friend std::ostream& operator <<(std::ostream& os, const basic_fraction<T1>& frac) noexcept {
            return os << frac.dump();
        }
    inline constexpr basic_fraction<T>& operator -() noexcept {
        this->num *= static_cast<T>(-1);
        return *this;
    }
    inline constexpr basic_fraction<T>& operator +() noexcept {
        return *this; //lol
    }
private:
    static constexpr T gcd(T a, T b) noexcept { //greatest CUMmon divisor
        while(b) {
            a %= b;
            std::swap(a, b);
        }
        return a;
    }
    constexpr basic_fraction<T>& reduce() noexcept {
        T div = basic_fraction::gcd(this->num, this->den);
        this->num /= div;
        this->den /= div;
        return *this;
    }
};

typedef basic_fraction<int> fraction;

}; //namespace calculator

int operator "" _solve(const char* s, unsigned long n) {
	return calculator::eval<int>(string(s, n));
}	

int operator "" _solve(const wchar_t* s, unsigned long n) {
	return calculator::eval<int>(wstring(s, n));
}

#endif /* __CALC_HPP_INCLUDED */

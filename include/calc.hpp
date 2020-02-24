#ifndef __CALC_HPP_INCLUDED
#define __CALC_HPP_INCLUDED

#include <string>
#include <stack>
#include <cmath>
#include <stdexcept>


namespace calculator {

class Error : public std::runtime_error {
public:
	explicit Error(const std::string& msg) noexcept: std::runtime_error(msg) {}
	virtual ~Error() noexcept = default;
};

template <class T, class S>
class ExpressionParser final {
private:
	enum {
		opAdd, // +
		opSub, // -
		opMulti, // *
		opDiv, // /
		opPow, // **
		opNull
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
		inline constexpr bool isNull() const {
			return this->op.op == opNull;
		}
	};
	S expr;
	int idx;
    std::stack<OperatorValue> _Stack;
	constexpr T calculate(const T& val_1, const T& val_2, const Operator& op) const noexcept {
		switch(op.op) {
			case opAdd: 
				return val_2 + val_1;
			case opSub:
				return val_1 - val_2;
			case opMulti:
				return val_1 * val_2;
			case opDiv:
				return val_1 / val_2;
			case opPow:
				return pow(val_1, val_2);
			default:
				return 0;
		}
	}
	constexpr typename S::value_type getChar() const noexcept {
		if(this->idx < std::size(this->expr))
			return this->expr[this->idx];
		return 0;
	}
	constexpr void skipWhitespace() noexcept {
		while(std::isspace(this->getChar()) != 0)
			++this->idx;
	}
	constexpr Operator parseOp() noexcept {
		this->skipWhitespace();
		switch(getChar()) {
			case '+':
				++this->idx;
				return Operator(opAdd, 10, 'L');
			case '-':
				++this->idx;
				return Operator(opSub, 10, 'L');
			case '/':
				++this->idx;
				return Operator(opDiv, 20, 'L');
			case '*':
				++this->idx;
				if (this->getChar() != '*')
					return Operator(opMulti, 20, 'L');
				++this->idx;
				return Operator(opPow, 30, 'R');
			default:
				return Operator(opNull, 0, 'L');
		}
	}
	inline static constexpr T toInt(const typename S::value_type& c) noexcept {
		if (c >= '0' && c <= '9')
			return c - '0';
		return 0xf + 1;
	}
	inline constexpr T getInt() const noexcept {
		return toInt(this->getChar());
	}
	constexpr T parse() noexcept {
		T val = 0;
		for(T num; (num = this->getInt()) <= 9; ++idx)
			val = val * 10 + num;
		return val;
	}
	constexpr T parseValue() {
		T val = 0;
		this->skipWhitespace();
		switch(this->getChar()) {
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
				val = this->parseExpr();
				this->skipWhitespace();
				if (this->getChar() != ')' && this->idx >= std::size(this->expr))
					throw Error("Пошла фигня: ')' не нашёл");
				++this->idx;
				break;
			case '+':
				++this->idx;
				val = parseValue();
				break;
			case '-':
				++this->idx;
				val = parseValue() * static_cast<T>(-1);
				break;
			default:
				if (this->idx >= std::size(this->expr))
					throw Error("Пошла фигня");
				break;
		} //switch
		return val;
	}
	constexpr T parseExpr() {
		this->_Stack.push(OperatorValue(Operator(opNull, 0, 'L'), 0));
		T val = this->parseValue();
		while(!_Stack.empty()) {
			auto op = Operator(this->parseOp());
			while(op.priority < this->_Stack.top().priority() || (op.priority == this->_Stack.top().priority() && op.side == 'L')) {
				if (this->_Stack.top().isNull()) {
					this->_Stack.pop();
					return val;
				}
				val = calculate(this->_Stack.top().value, val, this->_Stack.top().op);
				this->_Stack.pop();
			}
			this->_Stack.push(OperatorValue(op, val));
			val = this->parseValue();
		}
		return 0;
	}
public:
	constexpr T eval(const S& expr) {
		T result = 0;
		this->idx = 0;
		this->expr = expr;
		result = this->parseExpr();
		return result;
	}
}; // class 

template <class T, class S>
inline constexpr T eval(const S& expr) {
	auto parser = ExpressionParser<T, S>();
	return parser.eval(expr);
}

inline constexpr int eval(const std::string& expr) {
	return eval<int, std::string>(expr);
}

template<class T>
class Fraction final {
public:
    T num, den;
    explicit constexpr Fraction(const T& _num, const T& _den) : num(_num), den(_den) {
        if (!_den)
            throw std::invalid_argument("Denominator must be != 0");
        this->reduce();
    }
    inline constexpr Fraction<T>& reverse() noexcept {
        std::swap(this->num, this->den);
        return *this;
    }
    inline std::string toString() const noexcept {
        return (((this->num < 0) != (this->den < 0)) ? "-" : "") + std::string(abs(this->num)) + 
            ((abs(this->den) == 1) ? "" : ('/' + std::string(abs(this->den))));
    }
    /*
     * '+', '-', '*', '/' unary/binary operators
     * Note that reduce happens on construction
     */ 
    template<class T1> 
        friend inline constexpr Fraction<T1> operator +(const Fraction<T1>& f1, const Fraction<T1>& f2) noexcept {
            return Fraction<T1>(f1.num * f2.den + f2.num * f1.den, f1.den * f2.den);
        }
    template<class T1>
        friend inline constexpr Fraction<T1> operator -(const Fraction<T1>& f1, const Fraction<T1>& f2) noexcept {
            return f1 + (-f2);
        }
    template<class T1>
        friend inline constexpr Fraction<T1> operator *(const Fraction<T1>& f1, const Fraction<T1>& f2) noexcept {
            return Fraction<T1>(f1.num * f2.num, f2.den * f1.den);
        }
    template<class T1>
        friend inline constexpr Fraction<T1> operator /(const Fraction<T1>& f1, const Fraction<T1>& f2) noexcept {
            return f1 * f2.reverse();
        }
    template<class T1>
        friend std::ostream& operator <<(std::ostream& os, const Fraction<T1>& frac) noexcept {
            return os << frac.dump();
        }
    inline constexpr Fraction<T>& operator -() noexcept {
        this->num *= static_cast<T>(-1);
        return *this;
    }
    inline constexpr Fraction<T>& operator +() noexcept {
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
    constexpr Fraction<T>& reduce() noexcept {
        T div = Fraction::gcd(this->num, this->den);
        this->num /= div;
        this->den /= div;
        return *this;
    }
}; // class

}; //namespace calculator

int operator "" _solve(const char* s, unsigned long n) {
	return calculator::eval<int>(std::string(s, n));
}	

int operator "" _solve(const wchar_t* s, unsigned long n) {
	return calculator::eval<int>(std::wstring(s, n));
}

#endif /* __CALC_HPP_INCLUDED */

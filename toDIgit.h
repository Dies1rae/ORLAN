#pragma once

#include <string>
#include <iostream>
#include <variant>

namespace digcnv {
	using Data = std::variant<int, double>;

	struct DataPrinter {
		std::ostream& out;
		void operator()(int value) const {
			out << value;
		}
		void operator()(double value) const {
			out << value;
		}
	};

	std::ostream& operator<<(std::ostream& out, const Data& value) {
		std::visit(DataPrinter{ out }, value);
		return out;
	}

	class ParsingError : public std::runtime_error {
	public:
		using runtime_error::runtime_error;
	};

	class toDigit {
	public:
        explicit toDigit(const std::string& str) : base_str_(str.c_str()) {
            if (*base_str_ == '\0') {
                throw ParsingError("Empty string");
            }
            this->convertToDigit();
        }

		explicit toDigit(const char* str) : base_str_(str) {
			if (*base_str_ == '\0') {
				throw ParsingError("Empty string");
			}
			this->convertToDigit();
		}
		~toDigit(){}

		std::string_view origin() const {
			return { this->base_str_ };
		}

		bool IsInt() const {
			return std::holds_alternative<int>(this->converted_);
		}
		bool IsDouble() const {
			return std::holds_alternative<double>(this->converted_);
		}

		int AsInt() const {
			if (this->IsInt()) {
				return std::get<int>(this->converted_);
			} else {
				throw ParsingError("No INT in " + std::string(this->origin()));
			}
		}
		double AsDouble() const {
			if (this->IsDouble()) {
				return std::get<double>(this->converted_);
            } else if (this->IsInt()) {
                return (double)std::get<int>(this->converted_);
            } else {
				throw ParsingError("No DOUBLE in " + std::string(this->origin()));
			}
		}

	private:
		enum class State {
			sign, intpart, dot, fractional, power, powersign, powerint
		};

		struct DigitPart {
		public:
			int int_part_ = 0;
			double fract_part_ = 0;
			double delim_part_ = 10;
			int e_part_ = 0;
		};

		void convertToDigit() {
			DigitPart val_parts;
			auto ptr = this->base_str_;
			while (*ptr != '\0') {
				const char str_char = *ptr;
				switch (this->state_) {
					case(State::sign):
						if (str_char == '-') {
							this->sign_ = true;
							this->state_ = State::intpart;
						} else if (str_char == '+') {
							this->state_ = State::intpart;
						} else if (str_char == '.' || str_char == ',') {
							this->state_ = State::dot;
						} else if (str_char >= '0' && str_char <= '9') {
							this->state_ = State::intpart;
							continue;
						} else {
							throw ParsingError("Convert error - sign statement " + str_char);
						}
						break;
					case(State::intpart):
						if (str_char >= '0' && str_char <= '9') {
							val_parts.int_part_ *= 10;
							val_parts.int_part_ += (str_char - 48);
						} else if (str_char == '.' || str_char == ',') {
							this->state_ = State::dot;
						} else if (str_char == 'e') {
							this->state_ = State::power;
						} else {
							throw ParsingError("Convert error - intpart statement " + str_char);
						}
						break;
					case(State::dot):
						if (str_char >= '0' && str_char <= '9') {
							this->state_ = State::fractional;
							continue;
						} else {
							throw ParsingError("Convert error - dot statement " + str_char);
						}
						break;
					case(State::fractional):
						if (str_char >= '0' && str_char <= '9') {
							val_parts.fract_part_ += ((str_char - 48) / val_parts.delim_part_);
							val_parts.delim_part_ *= 10;
						} else if (str_char == 'e') {
							this->state_ = State::power;
						} else {
                            throw ParsingError("Convert error - fractional statement " + str_char);
						}
						break;
					case(State::power):
						if (str_char == '-') {
							this->power_sign_ = true;
							this->state_ = State::powersign;
						} else if (str_char == '+') {
							this->state_ = State::powersign;
						} else if (str_char >= '0' && str_char <= '9') {
							this->state_ = State::powerint;
							continue;
						} else {
							throw ParsingError("Convert error - power statement " + str_char);
						}
						break;
					case(State::powersign):
						if (str_char >= '0' && str_char <= '9') {
							this->state_ = State::powerint;
							continue;
						} else {
							throw ParsingError("Convert error - powersign statement " + str_char);
						}
						break;
					case(State::powerint):
						if (str_char >= '0' && str_char <= '9') {
							val_parts.e_part_ *= 10;
							val_parts.e_part_ += (str_char - 48);
						} else {
							throw ParsingError("Convert error - powerint statement " + str_char);
						}
						break;
					default:
						throw ParsingError("Convert error - main statement " + str_char);
						break;
				}
				ptr++;
			}

            if (this->IsInt()) {
				this->converted_ = val_parts.int_part_;
			} else {
				double res = (double)val_parts.int_part_ + val_parts.fract_part_;
				if (val_parts.e_part_ > 0) {
					powEksp(val_parts.e_part_, res);
				}
				this->converted_ = res;
			}
			if(this->sign_) {
				this->changeSign();
			}
		}

		void changeSign() {
			if (this->IsDouble()) {
				double value_tmp = this->AsDouble();
				this->converted_ = value_tmp * -1.0;
			} else {
				int value_tmp = AsInt();
				this->converted_ = value_tmp * -1;
			}
		}

		void powEksp(int E, double& value) {
			if (this->power_sign_) {
				while (E) {
					value /= 10;
					E--;
				}
			} else {
				while (E) {
					value *= 10;
					E--;
				}
			}
		}

		bool sign_ = false;
		bool power_sign_ = false;
		State state_{ State::sign };
		const char* base_str_{ nullptr };
		Data converted_;
	};
} //namespace digcnv

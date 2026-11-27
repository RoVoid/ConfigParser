#include "config.h"

#include <charconv> // чтоб из текста вытащить число
#include <fstream>
#include <stdexcept>
#include <type_traits> // для проверки типов

namespace config {
	namespace {
		std::string_view trim(std::string_view str) {
			const auto begin = str.find_first_not_of(" \t\r");
			if (begin == std::string_view::npos) return {};

			const auto end = str.find_last_not_of(" \t\r");
			return str.substr(begin, end - begin + 1);
		}

		template <typename T>
		T parse(std::string_view name, std::string_view text) {
			if constexpr (std::is_same_v<T, std::string>) {
				return std::string(text);
			} else if constexpr (std::is_same_v<T, bool>) {
				if (text == "true" || text == "1") return true;
				if (text == "false" || text == "0") return false;
				throw std::runtime_error("Invalid bool for key: " + std::string(name));
			} else {
				T result{};
				auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), result);
				if (ec != std::errc() || ptr != text.data() + text.size()) throw std::runtime_error("Invalid value for key: " + std::string(name));
				return result;
			}
		}

		void assign(Value& value, std::string_view name, std::string_view text) {
			std::visit([&]<typename T>(T& v) {
				v = parse<T>(name, text);
			}, value);
		}
	}

	template <typename T>
	void Config::add(const std::string& name, const T& defaultValue) {
		values_[name] = defaultValue;
	}

	void Config::load(const std::string& filename) {
		std::ifstream file(filename);
		if (!file.is_open()) throw OpenError("Cannot open config file: " + filename);

		std::string buffer;
		for (std::size_t lineNumber = 1; std::getline(file, buffer); ++lineNumber) {
			const std::string_view line = trim(buffer);
			if (line.empty() || line[0] == '#') continue;

			const auto where = filename + ":" + std::to_string(lineNumber) + ": ";

			const auto pos = line.find('=');
			if (pos == std::string_view::npos) throw std::runtime_error(where + "Invalid line: " + std::string(line));

			try {
				trySet(trim(line.substr(0, pos)), trim(line.substr(pos + 1)));
			} catch (const std::runtime_error& e) {
				throw std::runtime_error(where + e.what());
			}
		}
	}

	void Config::set(const std::string& name, const std::string& text) {
		auto it = values_.find(name);
		if (it == values_.end()) throw std::runtime_error("Unknown key: " + name);

		assign(it->second, name, text);
	}

	void Config::trySet(std::string_view name, std::string_view text) {
		auto it = values_.find(name);
		if (it == values_.end()) return;

		assign(it->second, name, text);
	}

	bool Config::contains(const std::string& name) const {
		return values_.contains(name);
	}

	const Map& Config::values() const {
		return values_;
	}

	template <typename T>
	const T& Config::get(const std::string& name) const {
		auto it = values_.find(name);
		if (it == values_.end()) throw std::runtime_error("Unknown key: " + name);

		const T* value = std::get_if<T>(&it->second);
		if (!value) throw std::runtime_error("Wrong type for key: " + name);

		return *value;
	}

	template void Config::add<std::string>(const std::string&, const std::string&);
	template void Config::add<bool>(const std::string&, const bool&);
	template void Config::add<int>(const std::string&, const int&);
	template void Config::add<unsigned>(const std::string&, const unsigned&);
	template void Config::add<long long>(const std::string&, const long long&);
	template void Config::add<double>(const std::string&, const double&);

	template const std::string& Config::get<std::string>(const std::string&) const;
	template const bool& Config::get<bool>(const std::string&) const;
	template const int& Config::get<int>(const std::string&) const;
	template const unsigned& Config::get<unsigned>(const std::string&) const;
	template const long long& Config::get<long long>(const std::string&) const;
	template const double& Config::get<double>(const std::string&) const;
}

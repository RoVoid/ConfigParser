#include "config.h"

#include <charconv>
#include <fstream>
#include <stdexcept>
#include <type_traits>

namespace config {
	namespace {
		std::string trim(const std::string& str) {
			const auto begin = str.find_first_not_of(" \t\r");
			if (begin == std::string::npos) return "";

			const auto end = str.find_last_not_of(" \t\r");
			return str.substr(begin, end - begin + 1);
		}

		template <typename T>
		T parse(const std::string& name, const std::string& text) {
			if constexpr (std::is_same_v<T, std::string>) {
				return text;
			} else if constexpr (std::is_same_v<T, bool>) {
				if (text == "true" || text == "1") return true;
				if (text == "false" || text == "0") return false;
				throw std::runtime_error("Invalid bool for key: " + name);
			} else {
				T result{};
				auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), result);
				if (ec != std::errc() || ptr != text.data() + text.size()) throw std::runtime_error("Invalid value for key: " + name);
				return result;
			}
		}
	}

	template <typename T>
	void Config::add(const std::string& name, const T& defaultValue) {
		values_[name] = defaultValue;
	}

	void Config::load(const std::string& filename) {
		std::ifstream file(filename);
		if (!file.is_open()) throw std::runtime_error("Cannot open config file: " + filename);

		std::string line;
		while (std::getline(file, line)) {
			line = trim(line);
			if (line.empty() || line[0] == '#') continue;

			const auto pos = line.find('=');
			if (pos == std::string::npos) throw std::runtime_error("Invalid line in " + filename + ": " + line);

			set(trim(line.substr(0, pos)), trim(line.substr(pos + 1)));
		}
	}

	void Config::set(const std::string& name, const std::string& text) {
		auto it = values_.find(name);
		if (it == values_.end()) throw std::runtime_error("Unknown key: " + name);

		std::visit([&](auto& value) {
			value = parse<std::decay_t<decltype(value)>>(name, text);
		}, it->second);
	}

	bool Config::contains(const std::string& name) const {
		return values_.contains(name);
	}

	const std::unordered_map<std::string, Value>& Config::values() const {
		return values_;
	}

	template <typename T>
	T Config::get(const std::string& name) const {
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

	template std::string Config::get<std::string>(const std::string&) const;
	template bool Config::get<bool>(const std::string&) const;
	template int Config::get<int>(const std::string&) const;
	template unsigned Config::get<unsigned>(const std::string&) const;
	template long long Config::get<long long>(const std::string&) const;
	template double Config::get<double>(const std::string&) const;
}

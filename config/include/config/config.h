#pragma once

#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>

namespace config {
	using Value = std::variant<std::string, bool, int, unsigned, long long, double>;
	using Map = std::map<std::string, Value, std::less<>>;

	// бросается из load, если файл не удалось открыть
	class OpenError : public std::runtime_error {
	public:
		using std::runtime_error::runtime_error;
	};

	class Config {
	private:
		Map values_;

		void trySet(std::string_view name, std::string_view text);
	public:
		template <typename T>
		void add(const std::string& name, const T& defaultValue);

		void load(const std::string& filename);

		void set(const std::string& name, const std::string& text);

		bool contains(const std::string& name) const;

		const Map& values() const;

		template <typename T>
		const T& get(const std::string& name) const;
	};
}

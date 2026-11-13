#pragma once

#include <string>
#include <unordered_map>
#include <variant>

namespace config {
	using Value = std::variant<std::string, bool, int, unsigned, long long, double>;

	class Config {
	private:
		std::unordered_map<std::string, Value> values_;
	public:
		template <typename T>
		void add(const std::string& name, const T& defaultValue);

		void load(const std::string& filename);

		void set(const std::string& name, const std::string& text);

		bool contains(const std::string& name) const;

		const std::unordered_map<std::string, Value>& values() const;

		template <typename T>
		T get(const std::string& name) const;
	};
}

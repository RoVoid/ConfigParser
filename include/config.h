#pragma once

#include <string>
#include <unordered_map>
#include <variant>

namespace config {
	using Value = std::variant<std::string, bool, int, unsigned, long long, double>;
	using Map = std::unordered_map<std::string, Value>;

	class Config {
	private:
		Map values_;
	public:
		template <typename T>
		void add(const std::string& name, const T& defaultValue);

		void load(const std::string& filename);

		void set(const std::string& name, const std::string& text);

		bool contains(const std::string& name) const;

		const Map& values() const;

		template <typename T>
		T get(const std::string& name) const;
	};
}

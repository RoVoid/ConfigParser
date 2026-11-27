#include <config.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <regex>
#include <vector>

namespace po = boost::program_options;

std::ostream& operator<<(std::ostream& os, const config::Map& map) {
	os << std::boolalpha << "{\n";

	for (const auto& [key, value] : map) {
		os << ' ' << key << " = ";

		std::visit([&os](const auto& v) {
			os << v;
		}, value);

		os << '\n';
	}

	return os << '}';
}

int main(int argc, char* argv[]) {
	po::options_description options("Options");
	options.add_options()
	("help,h", "show help")
	("config,c", po::value<std::string>()->default_value("config.ini"), "path to config file")
	("set,s", po::value<std::vector<std::string>>()->composing(), "override config value: key=value");

	po::variables_map args;
	try {
		po::store(po::parse_command_line(argc, argv, options), args);
		po::notify(args);
	} catch (const po::error& e) {
		std::cerr << e.what() << '\n';
		return 1;
	}

	if (args.contains("help")) {
		std::cout << options << '\n';
		return 0;
	}

	config::Config cfg;
	cfg.add<std::string>("host", "127.0.0.1");
	cfg.add<int>("port", 3000);
	cfg.add<bool>("debug", false);

	try {
		const auto path = args["config"].as<std::string>();
		try {
			cfg.load(path);
		} catch (const config::OpenError&) {
			std::cerr << "Warning: config file not found: " << path << " - using defaults\n";
		}

		if (args.contains("set")) {
			const std::regex pattern(R"(^\s*([^=\s]+)\s*=\s*(.*?)\s*$)");

			for (const auto& assignment : args["set"].as<std::vector<std::string>>()) {
				std::smatch match;
				if (!std::regex_match(assignment, match, pattern)) throw std::runtime_error("Invalid --set value: " + assignment + " (expected key=value)");

				cfg.set(match[1], match[2]);
			}
		}

		std::cout << cfg.get<std::string>("host") << '\n';
		std::cout << cfg.get<int>("port") << '\n';
		std::cout << cfg.get<bool>("debug") << '\n';
		std::cout << cfg.values() << '\n';
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << '\n';
		return 1;
	}
}

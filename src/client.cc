#include <iostream>
#include <optional>

#include <asio.hpp>

std::optional<std::pair<std::string, std::string>> input_credentials() {
	std::string username;
	std::string password;

	std::cout << "Username : ";
	
	if(!(std::cin >> username)) {
		std::cerr << "Input failed\n";
		return {};
	}

	std::cout << "Password : ";

	if(!(std::cin >> password)) {
		std::cerr << "Input failed\n";
		return {};
	}

	return std::make_pair(username, password);
}

int main(int argc, char ** argv) {

	if(argc != 2) {
		std::cerr << "Usage: " << argv[0] << " port_number\n";
		return 1;
	}

	const auto port_num = [argv]() {
		try {
			return std::stoi(argv[1]);
		} catch(...) {
			return -1;
		}
	}();

	if(port_num == -1) {
		std::cerr << "Invalid port number!\n";
		return 1;
	}

	asio::io_context ctx;
	asio::ip::tcp::socket sock(ctx);

	try {
		sock.connect(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port_num));
	} catch(const std::exception & _) {
		std::cerr << "Server is not reachable. Exiting...\n";
		return 1;
	}

	for(std::string cmd; std::cout << "> ", std::flush(std::cout), std::cin >> cmd; ) {

		if(cmd == "register") {
			auto registration_details = input_credentials();

			if(!registration_details) {
				continue;
			}

			auto & [username, password] = *registration_details;

			const std::string command = "REGISTER " + username + ':' + password;
			std::cout << "Sending register command to server... : " << command << '\n';
			sock.send(asio::buffer(command));

			continue;
		}

		if(cmd == "login") {
			auto login_details = input_credentials();

			if(!login_details) {
				continue;
			}

			auto & [username, password] = *login_details;

			const std::string command = "login " + username + ':' + password;
			std::cout << "Sending login command to server... : " << command << '\n';
			sock.send(asio::buffer(command));

			continue;
		}

		std::cout << "Available commands:\n - register\n - login\n - download\n - upload\n";
	}
}

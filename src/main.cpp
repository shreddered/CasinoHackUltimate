#include "calc.hpp"
#include <iostream>
#include "image.hpp"
#include "vk_api.hpp"
#include <thread>
#include <chrono>
//#include "process.hpp"
#include <utility>

#define COIN_MATH "-181671440"
#define sleep(n) std::this_thread::sleep_for(std::chrono::seconds(n))
#define LEVEL "Ур. 12"


std::string casino_hack(const VK::Client& client);
std::string fractionalHack(const VK::Client& client); 

int main(int argc, char** argv) {
	setlocale(LC_ALL, "");
	srand(time(0));
    if (argc < 2) {
        std::cerr << "Invalid usage." << std::endl << "Usage: " << argv[0] << " <token>" << std::endl;
        exit(1);
    }
	auto client = VK::Client(std::string(argv[1]));
	while(true) {
		try {
			client.send_message(LEVEL, COIN_MATH);
			sleep(5);
			auto resp = fractionalHack(client);
			if (resp == "Введите целое число, или нажмите на одну из кнопок, чтобы вернуться назад или узнать ответ...") {
				client.send_message("Показать ответ", COIN_MATH);
			}
			else if (resp == "Ответ неверный, попробуйте ещё раз...") {
				//cout << resp << endl;
				client.send_message("Показать ответ", COIN_MATH);
			}
			sleep(10);
		}
		catch(const calculator::Error& err) {
            std::cout << err.what() << std::endl;
		}
		catch(const VK::error& err) {
            std::cout << err.what() << std::endl;
			return 1;
		}
		catch(const http::error& err) {
            std::cout << err.what() << std::endl;
			return 1;
		}
		catch(const std::runtime_error& err) {
            std::cout << err.what() << std::endl;
		}
	}
	//cout << calculator::eval<int, wstring>(L"7*12*57/3") << L'\n';
	
}

std::string casino_hack(const VK::Client& client) {
	auto msg = client.get_last_message(COIN_MATH);
    //std::cout << msg.dump() << endl;
	if(!msg["url"].is_null()) {
		image::download(msg["url"].get<std::string>());
        std::wstring expr = image::parse("image.jpg", "file");
        std::wcout << expr << std::endl;
		//here comes the trick
		auto answer = calculator::eval<int, std::wstring>(expr);
		sleep(15);
		client.send_message(std::to_string(answer), COIN_MATH);
		sleep(5);
		msg = client.get_last_message(COIN_MATH);
	}
	if (!msg["text"].is_null())
		return msg["text"].get<std::string>();
	else
		throw std::runtime_error("Казино чудит");
}

std::string fractionalHack(const VK::Client& client) {
    auto msg = client.get_last_message(COIN_MATH);
	//cout << msg.dump() << endl;
	if(!msg["url"].is_null()) {
        image::download(msg["url"].get<std::string>());
        auto tmp = image::parse12lvl("image.jpg");
        auto result = solve12lvl(tmp);
        sleep(10);
        client.send_message(result.toString(), COIN_MATH);
        sleep(5);
        msg = client.get_last_message(COIN_MATH);
    }
    if (!msg["text"].is_null())
		return msg["text"].get<std::string>();
	else
		throw std::runtime_error("Казино чудит");
}

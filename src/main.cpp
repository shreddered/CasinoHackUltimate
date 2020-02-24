#include "calc.hpp"
#include <iostream>
#include "image.hpp"
#include "vk_api.hpp"
#include <thread>
#include <chrono>
#include "process.hpp"
#include <utility>

#define COIN_MATH "-181671440"
#define sleep(n) this_thread::sleep_for(chrono::seconds(n))
#define LEVEL "Ур. 6"

using namespace std;


string casino_hack(const VK::Client& client);

int main(int argc, char** argv) {
	setlocale(LC_ALL, "");
	srand(time(0));
    if (argc < 2) {
        cerr << "Invalid usage." << endl << "Usage: " << argv[0] << " <token>" << endl;
        exit(1);
    }
	auto client = VK::Client(string(argv[1]));
	while(true) {
		try {
			client.send_message(LEVEL, COIN_MATH);
			sleep(5);
			auto resp = casino_hack(client);
			if (resp == "Введите целое число, или нажмите на одну из кнопок, чтобы вернуться назад или узнать ответ...") {
				client.send_message("Показать ответ", COIN_MATH);
			}
			else if (resp == "Ответ неверный, попробуйте ещё раз...") {
				//cout << resp << endl;
				client.send_message("Показать ответ", COIN_MATH);
			}
			sleep(15);
		}
		catch(const calculator::Error& err) {
			cout << err.what() << endl;
		}
		catch(const VK::error& err) {
			cout << err.what() << endl;
			return 1;
		}
		catch(const http::error& err) {
			cout << err.what() << endl;
			return 1;
		}
		catch(const runtime_error& err) {
			cout << err.what() << endl;
		}
	}
	//cout << calculator::eval<int, wstring>(L"7*12*57/3") << L'\n';
	
}

string casino_hack(const VK::Client& client) {
	auto msg = client.get_last_message(COIN_MATH);
	cout << msg.dump() << endl;
	if(!msg["url"].is_null()) {
		image::download(msg["url"].get<string>());
		wstring expr = image::parse("image.jpg", "file");
		wcout << expr << endl;
		//here comes the trick
		auto answer = calculator::eval<int, wstring>(expr);
		sleep(15);
		client.send_message(to_string(answer), COIN_MATH);
		sleep(5);
		msg = client.get_last_message(COIN_MATH);
	}
	if (!msg["text"].is_null())
		return msg["text"].get<string>();
	else
		throw runtime_error("Казино чудит");
}

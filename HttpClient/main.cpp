#include <iostream>
#include <cstdio>

#include "http_client.h"


int main()
{
	auto client = create_http_client();
	auto results = client->commitGet("https://www.baidu.com/");
	printf("====================2=======================\n");
	auto results2 = client->commitGet("https://www.baidu.com/");
	printf("====================2=======================\n");
	std::cout << results.get()._handle_str << std::endl;
	std::cout << std::this_thread::get_id() << std::endl;
	
    return 0;
}
// MutiTask.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>



#include <concurrent_queue.h>
#include <thread>
#include <future>
#include <chrono>


class task {
public:
	task() {

	}
	~task() {

	}

	bool DoSomething() {
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

		for (int index = 0; index < 20000000; index++);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		return true;
	}

};


int main()
{

	unsigned int n = std::thread::hardware_concurrency();
	std::cout << n << " concurrent threads are supported.\n";

	unsigned int optimized_max_thread_count = (n - 3)*2;
	unsigned int optimized_basic_thread_count = (n - 3) -5;

	std::cout << n << "optimized_max_thread_count = " << optimized_max_thread_count << std::endl;
	std::cout << n << "optimized_basic_thread_count = " << optimized_basic_thread_count << std::endl;

	concurrency::concurrent_queue<std::shared_future<bool>> task_queue;

	std::thread maker([&] {
		while (true) {
			std::shared_future<bool> task_future = std::async(std::launch::async, [&] {

				task _task;
				return _task.DoSomething();

			});

			task_queue.push(task_future);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});
	maker.detach();



	std::thread manager([&] {
		for (int index = 0; index < optimized_basic_thread_count; index++) {
			std::thread consumer([&] {
				while (true) {
					std::shared_future<bool> task;
					if (task_queue.try_pop(task)) {
						auto result = task.get();
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
			});
			consumer.detach();
		}


		while (true) {
			int task_count = task_queue.unsafe_size();
			std::cout << "task count = " << task_count << std::endl;
			if (task_count > 8) {
				std::cout << "need optimization !!" << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});
	manager.detach();




	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

}





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

		for (int index = 0; index < 100000000; index++);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		return true;
	}

};


int main()
{


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


	std::thread consumer([&] {
		while (true) {
			std::cout << "task count = " << task_queue.unsafe_size() << std::endl;
			std::shared_future<bool> task;

			if (task_queue.try_pop(task)) {
				auto result = task.get();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});
	consumer.detach();

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

}





#pragma once

#include "../relacy/relacy_std.hpp"

// main RRD namespace is 'rl', also note required instrumentation in the form of '($)'

struct mutexmy {
	std::atomic<int> A;
	std::atomic<int> B;
	mutexmy() {
		A($) = 0;
		B($) = 0;
	}
	int thread1() {
		A.exchange(1, std::memory_order_acq_rel);
		return B.exchange(1, std::memory_order_acq_rel);
	}

	int thread2() {
		B.exchange(1, std::memory_order_acq_rel);
		return A.load(std::memory_order_acquire);
	}
};

// unit-test ('2' means number of threads)
struct mutex_test : rl::test_suite<mutex_test, 2> {
	mutexmy mtx;
	rl::var<int> x;
	rl::var<int> y;
	// executed in single thread before main thread function
	void before() {
	}
	// main thread function
	void thread(unsigned inx) {
		if (inx == 0 ){
			x($) = mtx.thread1();
		}
		else {
			y($) = mtx.thread2();
		}
	}
	// executed in single thread after main thread function
	void after() {
		RL_ASSERT(x($) != 0 || y($) != 0);
	}
};
#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"
#include "../h/riscv.hpp"
#include "printing.hpp"
#include "modification.h"

class ThreadX : public Thread {
public:
	ThreadX() = default;
protected:
	void run() override {
		for (int i = 0; i < 5; i++) {
		
		}
	}
};

class ThreadY : public Thread {
public:
	ThreadY() = default;
protected:
	void run() override {
		for (int i = 0; i < 5; i++) {
			
		}
	}
};

void modification() {
	ThreadX t1;
	ThreadY t2;

	t1.start();
	t2.start();
	t1.join();
	t2.join();
}

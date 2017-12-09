#include<iostream>
#include<deque>
struct testStr {
	std::deque<int> queue;
};

testStr arr[1] = {
	testStr()
};

testStr t = testStr();

void func() {
	testStr &s = arr[0];
	s.queue.push_back(5);
	t.queue.push_back(6);
}

void func2() {
	std::cout << arr[0].queue.empty() << '\n';
	std::cout << t.queue.empty() << '\n';
}

int main() {
	func();
	func2();
	return 0;
}
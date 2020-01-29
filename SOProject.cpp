#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

#include "so.h"
#include "sisprog.h"
SO so;

void test1() {
	so.JobArrival("programs/schopp/monitor.txt", 1);
}

void testMulti() {
	so.JobArrival("programs/schopp/work500m.txt", 0);
	so.JobArrival("programs/schopp/work500m.txt", 0);
}

void testPrioridade() {
	so.JobArrival("programs/schopp/n2.txt", 1);
	so.JobArrival("programs/schopp/work500m.txt", 0);
	so.JobArrival("programs/schopp/n2.txt", 2);
	so.JobArrival("programs/schopp/work500m.txt", 0);
}

void testIO() {
	so.JobArrival("programs/schopp/monitor.txt", 1);
	so.JobArrival("programs/schopp/monitor.txt", 1);
}

int main()
{
	test1();
	testMulti();
	testPrioridade();
	testIO();
	so.stop();
	return 0;
}




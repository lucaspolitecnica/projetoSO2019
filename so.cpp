#include "stdafx.h"
#include "so.h"



std::thread SOThread;
std::chrono::steady_clock::time_point t0;
std::mutex g_vm_mutex;
std::mutex g_file_mutex;
std::ofstream myfile;
SO::SO()
{
	myfile.open("SOLog.txt");
	t0 = std::chrono::steady_clock::now();
	idx = 1;
	running = true;
	SOThread=std::thread(&SO::run, this);
	
}

void SO::JobArrival(const std::string &file, int prioridade)
{
	ready=false;
	std::ifstream n2File(file);
	std::string exe = "";
	n2File >> exe;
	Process* p=new Process(idx, exe, prioridade);
	p->startTime= std::chrono::steady_clock::now() - t0;
	p->executionTime = rand() % 100 + 1;
	processes.push_back(*p);
	g_file_mutex.lock();
	myfile << "< " << p->startTime.count() << "><Job Arrival>< " << idx << "><Added to WaitingList>" << std::endl;
	g_file_mutex.unlock();
	idx++;
	waitMemlist.push_back(p);

}


void SO::run()
{
	
	int id=0;
	std::chrono::duration<double> dTime;
	
	while (running || !ready)
	{
		
		if (waitMemlist.size() > 0) {
			Process *p = waitMemlist.front();
			waitMemlist.pop_front();
			loadProcessesToReadyList(p);
			g_file_mutex.lock();
			myfile << "< " << (std::chrono::steady_clock::now() - t0).count() << "><Job Ready>< " << p->id << "><Added to ReadyList>" << std::endl;
			g_file_mutex.unlock();
		}
		if (readylist.size() > 0) {
			g_vm_mutex.lock();
			orderList();
			switch (vm.eval())
			{
			case VM::CycleResult::finished:
				finishCurrentProcess();
				if (waitIODevlist.size() + readylist.size() == 0)ready = true;
				if (waitIODevlist.size() > 0) {
					std::list<Process*>::iterator it = waitIODevlist.begin();
					while (it != waitIODevlist.end()) {
						if (blockIO(*it)) {
							waitIOReslist.push_back(*it);
							g_file_mutex.lock();
							myfile << "< " << (std::chrono::steady_clock::now() - t0).count() << "><Process requested IO>< " << std::to_string((*it)->id) << "><Using IO ID>" << std::to_string((*it)->requestedResource) << std::endl;
							g_file_mutex.unlock();
							std::thread *ioThread = new std::thread(&SO::useIO, this, (*it));
							waitIODevlist.remove(*it);
							break;
						}
						it++;
					}
				}
				break;
			case VM::CycleResult::IORequest:
				IOHandling();
				break;
			default:
				break;
			}
			g_vm_mutex.unlock();
		}
	}
	
}

void SO::loadProcessesToReadyList(Process* proc) {

	if (readylist.size() == 0) {
		readylist.push_back(proc);
	}
	else {
		std::list<Process*>::iterator it = readylist.begin();
		while (it != readylist.end()) {
			if ((*it)->prioridade > proc->prioridade) {
				readylist.insert(it, proc);
				break;
			}
			it++;
		}
		if (it == readylist.end())
			readylist.push_back(proc);
	}
	vm.restoreContext(*readylist.front());
	
}

void SO::deleteProcessFromReadyList(Process* proc) {
	readylist.pop_front();
	vm.saveContext(*proc);
}

void SO::finishCurrentProcess() {
	currentActiveCicles = 0;
	Process* p = readylist.front();
	p->endProcess(vm);
	for each (int res in p->resources)
	{
		unavailibleIO.remove(res);
	}
	readylist.pop_front();
	if (!readylist.empty())
		vm.restoreContext(*readylist.front());
	g_file_mutex.lock();
	myfile << "< " << (std::chrono::steady_clock::now() - t0).count() << "><Job Ended>< " << std::to_string(p->id) << "><Deleted from ReadyList>" << std::endl;
	g_file_mutex.unlock();
}

void SO::orderList() {
	if (currentActiveCicles < maxActiveCicles)
	{
		currentActiveCicles++;
	}
	else
	{
		currentActiveCicles = 0;
		Process *proc = readylist.front();
		deleteProcessFromReadyList(proc);
		loadProcessesToReadyList(proc);
		g_file_mutex.lock();
		myfile << "< " << (std::chrono::steady_clock::now() - t0).count() << "><Context switched>< " << std::to_string(proc->id) << "><New Process>" << std::to_string(readylist.front()->id) << std::endl;
		g_file_mutex.unlock();
	}
	Process *p = (readylist.front());
	p->loadProcess(vm);
}

void SO::IOHandling() {
	Process* p = readylist.front();
	p->requestedResource = vm.operand;
	deleteProcessFromReadyList(p);
	if (blockIO(p)) {
		waitIOReslist.push_back(p);
		g_file_mutex.lock();
		myfile << "< " << (std::chrono::steady_clock::now() - t0).count() << "><Process requested IO>< " << std::to_string(p->id) << "><Using IO ID>" << std::to_string(p->requestedResource) << std::endl;
		g_file_mutex.unlock();
		std::thread *ioThread = new std::thread(&SO::useIO, this,p);
	}	
	else {
		waitIODevlist.push_back(p);
		g_file_mutex.lock();
		myfile << "< " << (std::chrono::steady_clock::now() - t0).count() << "><Process requested IO>< " << std::to_string(p->id) << "><IO used by other Process>" << std::to_string(p->requestedResource) << std::endl;
		g_file_mutex.unlock();
	}
		
}

bool SO::blockIO(Process *p) {
	//Check if process already blocked resource for itself
	if (std::find(p->resources.begin(), p->resources.end(), p->requestedResource) != p->resources.end())return true;
	//Check if other process already blocked resource
	if (std::find(unavailibleIO.begin(), unavailibleIO.end(), p->requestedResource) != unavailibleIO.end())return false;
	p->resources.push_back(p->requestedResource);
	unavailibleIO.push_back(p->requestedResource);
	return true;
}

void SO::useIO(Process* p)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	g_file_mutex.lock();
	myfile << "< " << (std::chrono::steady_clock::now() - t0).count() << "><Process used IO>< " << std::to_string(p->id) << "><Inserted into Readylist>"  << std::endl;
	g_file_mutex.unlock();
	g_vm_mutex.lock();
	waitIOReslist.pop_front();
	loadProcessesToReadyList(p);
	g_vm_mutex.unlock();
}
void SO::stop() {
	running = false;
	SOThread.join();
	myfile.close();
}
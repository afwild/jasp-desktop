#include "ipcchannel.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::interprocess;
using namespace boost::posix_time;

IPCChannel::IPCChannel(std::string name, int channelNumber, bool isSlave)
{
	_name = name;
	_channelNumber = channelNumber;
	_isSlave = isSlave;

	_memory = new managed_shared_memory(boost::interprocess::open_or_create, name.c_str(), 1024*1024);

	stringstream mutexInName;
	stringstream mutexOutName;
	stringstream dataInName;
	stringstream dataOutName;
	stringstream semaphoreInName;
	stringstream semaphoreOutName;

	if (isSlave)
	{
		mutexInName << "sm";
		mutexOutName << "mm";
		dataInName << "sd";
		dataOutName << "md";
		semaphoreInName << "JASP-" << name << "-ss";
		semaphoreOutName << "JASP-" << name << "-ms";
	}
	else
	{
		mutexInName << "mm";
		mutexOutName << "sm";
		dataInName << "md";
		dataOutName << "sd";
		semaphoreInName << "JASP-" << name << "-ms";
		semaphoreOutName << "JASP-" << name << "-ss";
	}

	mutexInName << channelNumber;
	mutexOutName << channelNumber;
	dataInName << channelNumber;
	dataOutName << channelNumber;
	semaphoreInName << channelNumber;
	semaphoreOutName << channelNumber;

	_mutexIn  = _memory->find_or_construct<interprocess_mutex>(mutexInName.str().c_str())();
	_mutexOut = _memory->find_or_construct<interprocess_mutex>(mutexOutName.str().c_str())();
	_dataIn   = _memory->find_or_construct<String>(dataInName.str().c_str())(_memory->get_segment_manager());
	_dataOut  = _memory->find_or_construct<String>(dataOutName.str().c_str())(_memory->get_segment_manager());

#ifdef __APPLE__
	_semaphoreIn  = sem_open(mutexInName.str().c_str(), O_CREAT, S_IWUSR | S_IRGRP | S_IROTH, 0);
	_semaphoreOut = sem_open(mutexOutName.str().c_str(), O_CREAT, S_IWUSR | S_IRGRP | S_IROTH, 0);

	if (isSlave == false)
	{
		// cleanse the semaphores; they don't seem to reliably initalise to zero.

		while (sem_trywait(_semaphoreIn) == 0)
			; // do nothing
		while (sem_trywait(_semaphoreOut) == 0)
			; // do nothing

	}
#else

    if (_isSlave == false)
    {
        named_semaphore::remove(mutexInName.str().c_str());
        named_semaphore::remove(mutexOutName.str().c_str());

        _semaphoreIn  = new named_semaphore(create_only, mutexInName.str().c_str(), 0);
        _semaphoreOut = new named_semaphore(create_only, mutexOutName.str().c_str(), 0);
    }
    else
    {
        _semaphoreIn  = new named_semaphore(open_only, mutexInName.str().c_str());
        _semaphoreOut = new named_semaphore(open_only, mutexOutName.str().c_str());
    }


#endif
}

void IPCChannel::send(string &data)
{
	_mutexOut->lock();
	_dataOut->assign(data.begin(), data.end());

#ifdef __APPLE__
	sem_post(_semaphoreOut);
#else
    _semaphoreOut->post();
#endif


    _mutexOut->unlock();
}

bool IPCChannel::receive(string &data, int timeout)
{
	if (tryWait(timeout))
	{
		_mutexIn->lock();
		while (tryWait())
			; // clear it completely

		data.assign(_dataIn->c_str(), _dataIn->size());

		_mutexIn->unlock();

		return true;
	}

	return false;
}


bool IPCChannel::tryWait(int timeout)
{
	bool messageWaiting;

#ifdef __APPLE__

	messageWaiting = sem_trywait(_semaphoreIn) == 0;

	if (timeout > 0 && messageWaiting == false)
	{
		usleep(1000 * timeout);
		messageWaiting = sem_trywait(_semaphoreIn) == 0;
	}

#else

	if (timeout > 0)
	{
		ptime now(microsec_clock::universal_time());
		ptime then = now + microseconds(1000 * timeout);

        messageWaiting = _semaphoreIn->timed_wait(then);
	}
	else
	{
		messageWaiting = _semaphoreIn->try_wait();
	}
#endif

	return messageWaiting;

}


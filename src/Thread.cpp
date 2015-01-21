#include "Thread.hpp"

std::map< std::thread::id, std::pair<std::mutex, std::queue<ThreadMessage> > > Thread::_messageQueues;

Thread::Thread(std::thread::id master)
{
	_isRunning = false;
	_masterThreadID = master;
}

Thread::~Thread()
{
	
}


void Thread::sendMessage(std::thread::id threadID, ThreadMessage msg)
{
	_messageQueues[threadID].first.lock();
	_messageQueues[threadID].second.push(msg);
	_messageQueues[threadID].first.unlock();
}

void Thread::sendThreadMessage(ThreadMessage msg)
{
	sendMessage(_threadID, msg);
}

void Thread::kill()
{
	sendThreadMessage({ MessageType::TERMINATE, nullptr });
	_thread.join();
}

std::thread::id Thread::run(void* arguments)
{
	if (!_isRunning)
	{
		_thread = std::thread(&threadMain, arguments);
		_isRunning = true;
		_threadID = _thread.get_id();

		//Create queue
		_messageQueues[_threadID].first.unlock();

	}
	else
		return _threadID;
}

ThreadMessage Thread::getMessage()
{
	ThreadMessage t{ MessageType::NO_MESSAGE, nullptr };

	_messageQueues[_threadID].first.lock();
	if (!_messageQueues[_threadID].second.empty())
	{
		t = _messageQueues[_threadID].second.front();
		_messageQueues[_threadID].second.pop();
	}

	_messageQueues[_threadID].first.unlock();

	return t;
}

ThreadMessage Thread::peekMessage()
{
	ThreadMessage t{ MessageType::NO_MESSAGE, nullptr };

	_messageQueues[_threadID].first.lock();
	if (!_messageQueues[_threadID].second.empty())
	{
		t = _messageQueues[_threadID].second.front();
	}

	_messageQueues[_threadID].first.unlock();

	return t;
}
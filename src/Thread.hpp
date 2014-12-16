#pragma once

#include <mutex>
#include <thread>
#include <queue>
#include <map>
#include <chrono>

//Message Type
enum class MessageType
{
	NO_MESSAGE,
	TERMINATE,
	INIT_FAILED,
	INIT_OK,
	DATA_READY,
	THREAD_ID,
};

//Thread Message
typedef struct _ThreadMessage
{
	MessageType	messageType;
	void*		messageData;
} ThreadMessage;

//Threading class
//Implemented over std::thread
class Thread
{
public:
	Thread();

	//Pushes a message to threads queue
	//Static function
	static void sendMessage(std::thread::id threadID, ThreadMessage msg);
	
	//Blocking call
	//Sends terminate message
	//Waints in std::thread::join() to finish
	void kill();

	//Makes the thread run
	//Returns thread ID
	//Non-blocking (obviously)
	std::thread::id run(void* arguments);

	//Sends message to this thread
	void sendThreadMessage(ThreadMessage msg);

protected:

	//Checks whether there is a message waiting in the queue
	//Does not remove it from queue
	ThreadMessage peekMessage();

	//Checks whether there is a message waiting in the queue
	//Removes it from message queue
	ThreadMessage getMessage();

	//Thread implementation
	virtual void threadMain(void* arguments) = 0;

	//Sleeps for T miliseconds
	void sleep(std::chrono::milliseconds T);

private:
	//Thread identifier
	std::thread			_thread;
	std::thread::id		_threadID;
	bool				_isRunning;

	//Global message queues for each constructed thread
	//Each thread registers itself in this queue
	static std::map< std::thread::id, std::pair<std::mutex, std::queue<ThreadMessage> > > _messageQueues;
};
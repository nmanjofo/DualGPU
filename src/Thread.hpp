#pragma once

#include <mutex>
#include <thread>
#include <queue>
#include <map>
#include <chrono>
#include <functional>

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
	void*		messageData; //TODO je to takto treba? nestaci stack?//Allocated by senedr, deleted by receiver
} ThreadMessage;

//Threading class
//Implemented over std::thread
//Object inherited from this type internally spawns a thread
//and communication is handled by sending messages
class Thread
{
public:
	Thread();
	~Thread();

	//Pushes a message to threads queue
	//Static function
	static void sendMessage(std::thread::id threadID, ThreadMessage msg);

	//Sends message to this thread
	void sendThreadMessage(ThreadMessage msg);

	//gets ID of this thread
	std::thread::id getThreadID();

protected:

	//Checks whether there is a message waiting in the queue
	//Does not remove it from queue
	ThreadMessage peekMessage();

	//Checks whether there is a message waiting in the queue
	//Removes it from message queue
	ThreadMessage getMessage();

	//Thread implementation
	virtual void threadMain(void* arguments) = 0;

	//Blocking call
	//Sends terminate message
	//Waints in std::thread::join() to finish
	void kill();

	//Makes the thread run
	//Returns thread ID
	//Non-blocking (obviously)
	//TODO - isto vracat IDcko?
	std::thread::id run(void* arguments);

	bool isTimeToDie() { return _isTimeToDie; }

protected:
	//Thread identifier
	std::thread			_thread;
	std::thread::id		_threadID;
	bool				_isRunning;
	
	bool				_isTimeToDie;

	//Global message queues for each constructed thread
	//Each thread registers itself in this queue
	//Isto bude?
	static std::map< std::thread::id, std::pair<std::mutex, std::queue<ThreadMessage> > > _messageQueues;
};
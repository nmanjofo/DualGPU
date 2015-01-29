#pragma once

#include <functional>

class Callback
{
public:
	virtual void call(void* arg) = 0;
};

template <class T> class CallbackFromInstance : public Callback
{
public:
	void bind(void(T::*function)(void*), T* instance)
	{
		func = std::bind(function, instance, std::placeholders::_1);
	}

	void call(void* arg) override
	{
		func(arg);
	}

protected:
	std::function<void(void*)> func;
};

class CallbackFromFunction : public Callback
{
public:
	void bind(std::function<void(void*)> &f)
	{
		func = std::bind(f, std::placeholders::_1);
	}

	void call(void* arg) override
	{
		func(arg);
	}

protected:
	std::function<void(void*)> func;
};

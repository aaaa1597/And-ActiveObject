#ifndef ACTIVEOBJECT_H
#define ACTIVEOBJECT_H

#include <queue>
#include <functional>

#define D_THREADEND_MESSAGE 0x7777

class Priority {
public:
	bool operator<(Priority const& rhs) const {
		if(this->priority == rhs.priority)
			return this->creationtime < rhs.creationtime;
		return this->priority < rhs.priority;
	}

private:
	int priority;
	int64_t  creationtime;
	std::function<int()> func;
};

class ActionScheduler {
private:
	std::queue<std::function<int()>> mRunnableQueue;
	std::priority_queue<Priority> mQueue2;
	bool isInitializing = true;

public:	/* 公開用 */
	static bool isInitializeing(int SurfaceID);
	static void sendMessage(int SurfaceID, int MsgID);
	static void sendMessage(int SurfaceID, std::function<int()> &func);
	static void sendMessage(int SurfaceID, std::function<int()> &&func);
	static void ThreadLoop(int SurfaceID);

public:	/* for System */
	ActionScheduler(){}
	ActionScheduler(const ActionScheduler &obj) { mRunnableQueue = obj.mRunnableQueue; mQueue2 = obj.mQueue2; }
	ActionScheduler(const ActionScheduler&& obj) { mRunnableQueue = std::move(obj.mRunnableQueue); mQueue2 = std::move(obj.mQueue2); }
};

#endif //ACTIVEOBJECT_H

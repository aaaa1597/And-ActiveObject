#include <map>
#include <thread>
#include <mutex>
#include <android/log.h>
#include "ActiveObject.h"

static std::map<int, ActionScheduler> mActionSchedulerList;
static std::mutex mMutexforAOQueue;
static std::mutex mMutexforAS;

void ActionScheduler::sendMessage(int SurfaceID, int MsgID) {
	mMutexforAS.lock();
	std::map<int, ActionScheduler>::iterator itr = mActionSchedulerList.find(SurfaceID);
	mMutexforAS.unlock();
	if(itr == mActionSchedulerList.end())
		__android_log_assert("itr == mActionSchedulerList.end()", "aaaaa", "itr == mActionSchedulerList.end() SurfaceID=%d line(%d)", SurfaceID, __LINE__);

	mMutexforAOQueue.lock();
	itr->second.mRunnableQueue.push([MsgID](){return MsgID;});
	mMutexforAOQueue.unlock();
}

void ActionScheduler::sendMessage(int SurfaceID, std::function<int()> &func) {
	mMutexforAS.lock();
	std::map<int, ActionScheduler>::iterator itr = mActionSchedulerList.find(SurfaceID);
	mMutexforAS.unlock();
	if(itr == mActionSchedulerList.end())
		__android_log_assert("itr == mActionSchedulerList.end()", "aaaaa", "itr == mActionSchedulerList.end() SurfaceID=%d line(%d)", SurfaceID, __LINE__);

	mMutexforAOQueue.lock();
	itr->second.mRunnableQueue.push(func);
	mMutexforAOQueue.unlock();
	return;
}

void ActionScheduler::sendMessage(int SurfaceID, std::function<int()> &&func) {
	mMutexforAS.lock();
	std::map<int, ActionScheduler>::iterator itr = mActionSchedulerList.find(SurfaceID);
	mMutexforAS.unlock();
	if(itr == mActionSchedulerList.end())
		__android_log_assert("itr == mActionSchedulerList.end()", "aaaaa", "itr == mActionSchedulerList.end() SurfaceID=%d line(%d)", SurfaceID, __LINE__);

	mMutexforAOQueue.lock();
	itr->second.mRunnableQueue.push(func);
	mMutexforAOQueue.unlock();
	return;
}

void ActionScheduler::ThreadLoop(int SurfaceId) {
	mMutexforAS.lock();
	mActionSchedulerList.insert(std::make_pair(SurfaceId, ActionScheduler()));
	ActionScheduler &as = mActionSchedulerList[SurfaceId];
	mMutexforAS.unlock();
	as.isInitializing = false;
	mMutexforAOQueue.lock();
	std::queue<std::function<int()>> &queue = as.mRunnableQueue;
	mMutexforAOQueue.unlock();

	while(true) {
		/* 受信待ち */
		while(queue.empty())
			std::this_thread::sleep_for(std::chrono::milliseconds(1));    // 10 ミリ秒

		mMutexforAOQueue.lock();
		/* アイテム実行 */
		std::function<int()> func = queue.front();
		queue.pop();
		mMutexforAOQueue.unlock();
		int ret = func();

		/* スレッド終了判定 */
		if(ret == D_THREADEND_MESSAGE)
			break;
	}

	mMutexforAS.lock();
	mActionSchedulerList.erase(SurfaceId);
	mMutexforAS.unlock();
}

bool ActionScheduler::isInitializeing(int SurfaceID) {
	mMutexforAS.lock();
	std::map<int, ActionScheduler>::iterator itr = mActionSchedulerList.find(SurfaceID);
	mMutexforAS.unlock();

	if(itr == mActionSchedulerList.end())
		return true;

	return itr->second.isInitializing;
}

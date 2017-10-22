#include <android/log.h>
#include <map>
#include "ActiveObject.h"

static std::map<int, ActionScheduler> gActionSchedulerList;
void ActionScheduler::sendMessage(int SurfaceID, std::function<int()> &&func) {
	gActionSchedulerList[SurfaceID].mRunnableQueue.push(func);
	return;
}

void ActionScheduler::ThreadLoop(int SurfaceId) {
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa ThreadLoop() s SurfaceId=%d", SurfaceId);
	gActionSchedulerList.insert(std::make_pair(SurfaceId, ActionScheduler()));
	ActionScheduler &as = gActionSchedulerList[SurfaceId];
	as.isInitializing = false;
	std::queue<std::function<int()>> &queue = as.mRunnableQueue;

	while(true) {
		/* 受信待ち */
		while(queue.empty())
			std::this_thread::sleep_for(std::chrono::milliseconds(10));    // 10 ミリ秒

		/* アイテム実行 */
		std::function<int()> func = queue.front();
		queue.pop();
		int ret = func();

		/* スレッド終了判定 */
		if(ret == D_THREADEND_MESSAGE)
			break;
	}

	gActionSchedulerList.erase(SurfaceId);
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa ThreadLoop() e SurfaceId=%d", SurfaceId);
}

bool ActionScheduler::isInitializeing(int SurfaceID) {
	return gActionSchedulerList.find(SurfaceID)->second.isInitializing;
}

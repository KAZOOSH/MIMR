#pragma once
#include "ofMain.h"
class Timer
{
public:
	Timer();
	~Timer();

	void update();
	void start();
	void stop();
	void setCountdown(long time);
	void setCountdown(long time,int round);
	string getRemainingTimeString();
	float getPercentage();
	bool isRunning();
	void setRounds(int n);
	int getRound();

	ofEvent<void> timerFinishedEvent;

private:
	bool isRunning_ = false;
	long tStart = 0;
	vector<long> runTime = { 3000 };
	int nRounds = 1;
	int currentRound = 0;
};


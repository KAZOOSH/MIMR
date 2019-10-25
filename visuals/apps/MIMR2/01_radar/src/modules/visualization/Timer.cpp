#include "Timer.h"



Timer::Timer()
{
}


Timer::~Timer()
{
}

void Timer::update()
{
	if (isRunning_ && ofGetElapsedTimeMillis() - tStart > runTime[currentRound]) {
		currentRound++;
		if (currentRound >= nRounds) {
			isRunning_ = false;
			ofNotifyEvent(timerFinishedEvent);
		}else {
			
			tStart = ofGetElapsedTimeMillis();
		}
		
	}
}

void Timer::start()
{
	isRunning_ = true;
	currentRound = 0;
	tStart = ofGetElapsedTimeMillis();
}

void Timer::stop()
{
	isRunning_ = false;
	
}

void Timer::setCountdown(long time)
{
	for (auto& t : runTime) {
		t = time;
	}
}

void Timer::setCountdown(long time, int round)
{
	if (runTime.size() > round) runTime[round] = time;
}

string Timer::getRemainingTimeString()
{
	/*long tTemp = (runTime - (ofGetElapsedTimeMillis() - tStart)) / 1000;
	string out = ofToString(tTemp / 60);
	out += ":";
	string sec = ofToString(tTemp % 60);
	if (sec.length() < 2) sec = "0" + sec;
	out += sec;*/
	return "";
}

float Timer::getPercentage()
{
	return ofMap(ofGetElapsedTimeMillis() - tStart, 0, runTime[currentRound], 0, 1);
}

bool Timer::isRunning()
{
	return isRunning_;
}

void Timer::setRounds(int n)
{
	nRounds = n;
	while (runTime.size() < nRounds) {
		runTime.push_back(runTime[0]);
	}
}

int Timer::getRound()
{
	return currentRound;
}

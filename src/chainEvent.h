//
//  chainEvent.h
//  summerparty
//
//  Created by Karina Korsgaard Jensen on 29/05/2018.
//

#ifndef chainEvent_h
#define chainEvent_h

typedef enum {
    BEGIN_LEARNING,
    LEARNING,
    THINKING,
    TRAINING,
    PLAYING, // these can have any name you want, eg STATE_CLAP etc
    POSE1,
    POSE2,
    POSE3,
	POUR,
    TRYAGAIN,
	PREPARE,
    PAUSE,
    RESET,
    TOOMANY,
    NOONE,
	CELEBRATE,
    EMPTY
} State;


class ChainEvent {
public:
    vector<double> timers;
    vector<double> durations;
    vector<State> eventName;
    vector<bool> looping;
    bool done = true;
    int eventNumber = 0;
    bool isfirstframe;
    void addEvent(double duration, State name, bool loop = false) {
        timers.push_back(0.0);
        durations.push_back(duration);
        eventName.push_back(name);
        looping.push_back(loop);
    }
    
    void update() {
        if (done) return;
        
        if (shouldGoToNextEvent()) {
            timers[eventNumber] = 0.0;
            eventNumber++;
            isfirstframe = true;
            if (eventNumber > timers.size() - 1) {
                setToEmpty();
            }
        }
        
        timers[eventNumber] += ofGetLastFrameTime();
    }
    
    bool shouldGoToNextEvent() {
        if (looping[eventNumber]) return false;
        if (timers[eventNumber] < durations[eventNumber] + .3) return false;
        return true;
    }
    
    void beginEvents() {
        cout << "begin events" << endl;
        done = false;
        eventNumber = 0;
        isfirstframe = true;
		std::fill(timers.begin(), timers.end(), 0.0);
    }
    
    void setTo(State state) {
        if (getName() == state) return;
        std::vector<State>::iterator it = std::find(eventName.begin(), eventName.end(), state);
        
        if (it != eventName.end()) {
            eventNumber = std::distance(eventName.begin(), it);
            setToEmpty(eventNumber);
        }
    }
    
    void setToEmpty(int i = 0) {
        isfirstframe = true;
        eventNumber = i;
        done = eventNumber == 0 ? true : false;
    }
    
    int getEvent() {
        return eventNumber;
    }
    double getTime() {
        return timers[eventNumber];
    }
    double getDuration() {
        return durations[eventNumber];
    }
    State getName() {
        if (done)return EMPTY;
        else return eventName[eventNumber];
    }
	
    void next() {
        isfirstframe = true;
        eventNumber++;
        eventNumber = eventNumber%timers.size();
    }
    void back() {
        isfirstframe = true;
        timers[eventNumber] = 0.0;
        eventNumber--;
        eventNumber = eventNumber%timers.size();
        timers[eventNumber] = 0.0;
    }
    
};

#endif /* chainEvent_h */

#ifndef CALLBACK_INTERFACE_H
#define CALLBACK_INTERFACE_H


#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <vector>

#include <unordered_map>

class CallbackInterface {
    public:
        virtual void onEvent(int eventId, std::string eventData) = 0;
        virtual ~CallbackInterface() = default;
};

class EventTrigger {
    public:
    
        ~EventTrigger() {
            for (auto callback : callbacks) {
                delete callback;
            }
        }
        void addCallback(CallbackInterface* callback) {
            callbacks.push_back(callback);
        }
    
        
        void triggerEvent(int eventId, std::string eventData) {
            for (const auto& callback : callbacks) {
                if (callback) {
                    callback->onEvent(eventId, eventData);
                }
            }
        }
    
    private:
        std::vector<CallbackInterface*> callbacks;
};
#endif
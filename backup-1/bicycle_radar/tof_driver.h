#ifndef TOF_DRIVER_H
#define TOF_DRIVER_H
#include <string>

#include "CallbackInterface.h"

class ToFSensor {
public:
    bool running = true;
    struct StructLidar {
        float Temperature;
        float Dist;
        int Strength;
    };
    void writeCSV(const std::string& filename, const std::vector<StructLidar>& data);
    static std::string serialize(const StructLidar& data);
    static StructLidar deserialize(const std::string& data);
    ToFSensor(std::string port, EventTrigger* trigger);
    void SetRunningFlag(bool signum) {
        std::cout << "\nCaught signal " << signum << ", exiting gracefully...\n";
        running = signum;
    }
    int Init();
    int Run();

private:
    std::string portName;
    EventTrigger* eventTrigger;
    int serialPort = -1;
};
#endif //TOF_DRIVER_H

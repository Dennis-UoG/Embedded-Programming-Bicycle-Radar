#ifndef IMU_DRIVER_H
#define IMU_DRIVER_H
#include "CallbackInterface.h"

class IMUSensor {
public:
    bool running = true;
    struct StructImu {
        float acc_X;
        float acc_Y;
        float acc_Z;
    };
    static void writeCSV(const std::string& filename, const std::vector<StructImu>& data);
    static std::string serialize(const StructImu& data);
    static StructImu deserialize(const std::string& data);
    IMUSensor(std::string port, EventTrigger* trigger);
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

#endif // IMU_DRIVER_H

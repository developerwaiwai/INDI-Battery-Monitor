

#pragma once

#include "libindi/defaultdevice.h"

class BatteryMonitor : public INDI::DefaultDevice
{
private:
    IText CapacityT[1] {};
    ITextVectorProperty CapacityTP;
    char capacity_text[100] = {0};

private:
    void get_pc_battery_capacity();

protected:
     virtual void TimerHit() override;

public:
    BatteryMonitor();
    virtual ~BatteryMonitor() = default;

    virtual const char *getDefaultName() override;

    virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n) override;

    virtual bool initProperties() override;

    virtual bool updateProperties() override;

    bool Connect();
    
    bool Disconnect();
};
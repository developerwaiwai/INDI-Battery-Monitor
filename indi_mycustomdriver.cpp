#include "config.h"
#include "indi_mycustomdriver.h"
#include <stdio.h>

// We declare an auto pointer to BatteryMonitor.
static std::unique_ptr<BatteryMonitor> mydriver(new BatteryMonitor());

BatteryMonitor::BatteryMonitor()
{
    setVersion(CDRIVER_VERSION_MAJOR, CDRIVER_VERSION_MINOR);
}

const char *BatteryMonitor::getDefaultName()
{
    return "My Custom Driver";
}


bool BatteryMonitor::initProperties()
{
    INDI::DefaultDevice::initProperties();

    get_pc_battery_capacity();

    IUFillText(&CapacityT[0], "PC_BATTERY", "Battery Capacity", (const char*)capacity_text);
    IUFillTextVector(&CapacityTP, CapacityT, 1, getDeviceName(), "PC_BATTERY", "Battery Capacity", MAIN_CONTROL_TAB, IP_RO, 60, IPS_IDLE);
    defineProperty(&CapacityTP);

    setDriverInterface(AUX_INTERFACE);
    addAuxControls();

    return true;
}

bool BatteryMonitor::updateProperties()
{
    INDI::DefaultDevice::updateProperties();

    if (isConnected())
    {
        this->TimerHit();

        // get_pc_battery_capacity();
        // IUSaveText(&CapacityT[0], capacity_text);

        // SetTimer(getCurrentPollingPeriod());
    }

    return true;
}


bool BatteryMonitor::Connect()
{
    LOG_INFO("BatteryMonitor is online.");
    return true;
}


bool BatteryMonitor::Disconnect()
{
    LOG_INFO("BatteryMonitor is offline.");
    return true;
}


void BatteryMonitor::TimerHit()
{
    get_pc_battery_capacity();
    IUSaveText(&CapacityT[0], capacity_text);
    IDSetText(&CapacityTP, nullptr);

    SetTimer(getCurrentPollingPeriod());
}


bool BatteryMonitor::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    // Make sure it is for us.
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (strcmp(name, CapacityTP.name) == 0)
        {
            CapacityTP.s = IPS_IDLE;
            // This is a helper method to just update the values
            // on the property.
            if (!IUUpdateText(&CapacityTP, texts, names, n))
            {
                return false;
            }
            IDSetText(&CapacityTP, nullptr);
            return true;
        }
    }

    // Nobody has claimed this, so let the parent handle it
    return INDI::DefaultDevice::ISNewText(dev, name, texts, names, n);
}


void BatteryMonitor::get_pc_battery_capacity()
{
    FILE* fd = NULL;
    fd = fopen("/sys/class/power_supply/BAT0/capacity", "r");
    if(fd == NULL) {
        fd = fopen("/sys/class/power_supply/BAT1/capacity", "r");
    }

    if (fd != NULL) {
        unsigned char buffer[4];
        fread(buffer, sizeof(buffer), sizeof(buffer), fd);
        fclose(fd);

        snprintf(capacity_text, sizeof(capacity_text), "%s %%", buffer);
    }
    else {
        snprintf(capacity_text, sizeof(capacity_text), "Unknown");
    }
}
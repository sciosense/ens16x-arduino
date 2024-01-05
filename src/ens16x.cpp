#include <Arduino.h>
#include <Wire.h>

#include "ens16x.h"
#include "utils.h"

using namespace ScioSense::Utils;

namespace ScioSense
{
    ENS16x::~ENS16x() { }

    ENS16x::ENS16x()
    {
        partId          = 0;
        operatingMode   = OperatingMode::DeepSleep;
        configuration   = Configuration::Default;
        interruptPin    = NOT_A_PIN;

        debugStream     = nullptr;
        ioInterface     = nullptr;
    }

    bool ENS16x::begin(IoInterface<RegisterAddress, Result>* ioInterface)
    {
        this->ioInterface = ioInterface;

        reset();

        read(RegisterAddress::PART_ID, partId);
        debug("PART_ID  : ", partId);

        if (isConnected())
        {
            setOperatingMode(OperatingMode::Idle);
            readFirmwareVersion();
        }

        return isConnected();
    }

    ENS16x::Result ENS16x::reset()
    {
        Result result;

        write(RegisterAddress::OPMODE, OperatingMode::Reset);
        delay(SystemTiming::Reset);
        result = write(RegisterAddress::OPMODE, OperatingMode::Idle);

        debug(__func__, result);
        return result;
    }

    ENS16x::Result ENS16x::readFirmwareVersion()
    {
        //wait for long running op-modes
        {
            for (int i = 0; i < 30; i++)
            {
                invoke(Command::ClearGeneralPourposeRegisters, SystemTiming::ClearGeneralPurposeRegisters);
                read(RegisterAddress::GPR_READ_4, firmwareVersion);
                if
                (       firmwareVersion[0] == 0
                    &&  firmwareVersion[1] == 0
                    &&  firmwareVersion[2] == 0
                )
                {
                    break;
                }

                delay(SystemTiming::StandardMeasure);
            }
        }

        Result result = invoke(Command::GetFirmwareVersion);

        if (result == Result::Ok)
        {
            result = read(RegisterAddress::GPR_READ_4, firmwareVersion);

            if (this->debugStream)
            {
                this->debugStream->print("Firmware: ");
                this->debugStream->print(firmwareVersion[0]);
                this->debugStream->print(".");
                this->debugStream->print(firmwareVersion[1]);
                this->debugStream->print(".");
                this->debugStream->println(firmwareVersion[2]);
            }
        }

        return result;
    }

    ENS16x::Result ENS16x::setOperatingMode(const OperatingMode& mode)
    {
        Result result = write(RegisterAddress::OPMODE, mode);

        if (result == Result::Ok)
        {
            operatingMode = mode;
        }

        return result;
    }

    ENS16x::Result ENS16x::invoke(const Command& command, unsigned long ms)
    {
        return invoke((uint8_t)command, ms);
    }

    ENS16x::Result ENS16x::invoke(uint8_t command, unsigned long ms)
    {
        Result result = Result::Ok;

        if (operatingMode != OperatingMode::Idle)
        {
            result = setOperatingMode(OperatingMode::Idle);
        }

        if (result == Result::Ok)
        {
            write(RegisterAddress::COMMAND, Command::Nop);
            write(RegisterAddress::COMMAND, Command::ClearGeneralPourposeRegisters);
            delay(SystemTiming::ClearGeneralPurposeRegisters);

            result = write(RegisterAddress::COMMAND, command);
            delay(ms);
        }

        return result;
    }

    ENS16x::Result ENS16x::startStandardMeasure()
    {
        return setOperatingMode(OperatingMode::Standard);
    }

    ENS16x::Result ENS16x::writeCompensation(const uint16_t& temperature, const uint16_t& relativeHumidity)
    {
        uint8_t compensation[4];
        compensation[0] = temperature & 0xff;
        compensation[1] = (temperature >> 8) & 0xff;

        compensation[2] = relativeHumidity & 0xff;
        compensation[3] = (relativeHumidity >> 8) & 0xff;

        return write(RegisterAddress::TEMP_IN, compensation);
    }

    ENS16x::Result ENS16x::writeConfiguration(const Configuration& configuration)
    {
        this->configuration = configuration;
        return write(RegisterAddress::CONFIG, configuration);
    }

    void ENS16x::waitPolled()
    {
        delay(SystemTiming::StandardMeasure);
    }

    void ENS16x::waitInterrupt()
    {
        if (hasFlag(configuration, Configuration::InterruptPolarity))
        {
            while (digitalRead(interruptPin) != HIGH); // wait until high
        }
        else
        {
            while (digitalRead(interruptPin) != LOW); // wait until low
        }
    }

    ENS16x::Result ENS16x::update()
    {
        Result result;

        result = read(RegisterAddress::DEVICE_STATUS, deviceStatus);
        if (result == Result::Ok)
        {
            if (hasAnyFlag(deviceStatus, DeviceStatus::NewData | DeviceStatus::NewGeneralPurposeData))
            {
                if (hasFlag(deviceStatus, DeviceStatus::NewData))
                {
                    result = read(RegisterAddress::DATA_AQI, dataBuffer);
                }

                if (result == Result::Ok && hasFlag(deviceStatus, DeviceStatus::NewGeneralPurposeData))
                {
                    result = read(RegisterAddress::GPR_READ, resistanceBuffer);
                }
            }
            else
            {
                result = Result::Invalid;
            }
        }

        return result;
    }

    void ENS16x::enableDebugging(Stream& debugStream)
    {
        this->debugStream = &debugStream;
    }

    void ENS16x::disableDebugging()
    {
        debugStream = nullptr;
    }

    template<class T>
    ENS16x::Result ENS16x::read(RegisterAddress address, T& data)
    {
        return read(address, (uint8_t*)&data, sizeof(data));
    }

    template<class T>
    ENS16x::Result ENS16x::write(RegisterAddress address, const T& data)
    {
        return write(address, (uint8_t*)&data, sizeof(data));
    }
}
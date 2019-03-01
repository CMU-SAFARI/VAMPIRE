/*

STATISTICS.CPP

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) 2018 SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#include "statistics.h"
#include "vampire.h"

/***************************************/
/* Defining ScalarStat Class Functions */
/***************************************/

/* Constructor */
template<typename T>
ScalarStat<T>::ScalarStat(T value, std::string name, std::string unit, std::string description)
        : value(value), name(name), unit(unit), description(description) {}


template <typename T>
std::string ScalarStat<T>::toString() const {
    std::stringstream ss;
    ss << std::setw(40) << this->name << ": "
       << std::setw(20) << this->value
       << std::setw(5) << this->unit;
    if (this->description != "")
        ss << "  # " << this->description;
    ss << std::endl;
    return ss.str();
};

/* Generates a string representation of the object for writing to a csv file */
template<typename T>
std::string ScalarStat<T>::toCsvString() const {
    std::stringstream ss;
    ss << this->name << ","
       << this->value << ","
       << this->unit << ","
       << this->description << std::endl;
    return ss.str();
}

/* Getters and setters */
template<typename T>
void ScalarStat<T>::setValue(T value) {
    this->value = value;
}
template<typename T>
const std::string &ScalarStat<T>::getName() const {
    return name;
}
template<typename T>
void ScalarStat<T>::setName(const std::string &name) {
    this->name = name;
}
template<typename T>
const std::string &ScalarStat<T>::getUnit() const {
    return unit;
}
template<typename T>
void ScalarStat<T>::setUnit(const std::string &unit) {
    this->unit = unit;
}
template<typename T>
const std::string &ScalarStat<T>::getDescription() const {
    return description;
}
template<typename T>
void ScalarStat<T>::setDescription(const std::string &description) {
    this->description = description;
}

/***************************************/
/* Defining VectorStat Class Functions */
/***************************************/
template <typename T>
VectorStat<T>::VectorStat(uint64_t memberCount, T initialValue, std::string name, std::string unit, std::string description) {
    members = new std::vector<T>(memberCount, initialValue);
    this->name = name;
}

template <typename T>
VectorStat<T>::~VectorStat() {
    delete members;
}

template<typename T>
std::string VectorStat<T>::toCsvString() const {
    std::stringstream ss;
    for (int i = 0; i < members->size(); i++) {
        ss << this->name
           << "[" << i << "]: "
           << this->members->operator[](i) << ","
           << this->unit << ","
           << this->description
           << std::endl;
    }
    return ss.str();
};

template<typename T>
std::string VectorStat<T>::toCsvString(std::function<std::string(uint64_t)> mappingFun) const {
    std::stringstream ss;
    for (uint64_t i = 0; i < members->size(); i++) {
        ss << mappingFun(i) << ","
           << this->members->operator[](i) << ","
           << this->unit << ","
           << std::endl;
    }
    return ss.str();
}

/***************************************/
/* Defining Statistics Class Functions */
/***************************************/
void Statistics::print_stats() const {
    msg::info("Printing stats...");
    std::function<std::string(uint64_t)> cmdToString=[&] (uint64_t index) -> std::string {return commandString[index] + " count";};
    std::function<std::string(uint64_t)> cmdToStringCycles=[&] (uint64_t index) -> std::string {return commandString[index] + " cycles";};
    std::cout
            << cmdCount->toString(cmdToString) << std::endl
            << cmdCycles->toString(cmdToStringCycles) << std::endl
            << totalCycleCount->toString() << std::endl
            << totalActStandbyCycles->toString()
            << totalPreStandbyCycles->toString() << std::endl
            << totalReadEnergy->toString()
            << totalWriteEnergy->toString()
            << totalPreCmdEnergy->toString()
            << totalActCmdEnergy->toString()
            << totalPrechargeStandbyEnergy->toString()
            << totalActiveStandbyEnergy->toString()
            << totalEnergy->toString()
            << avgPower->toString()
            << avgCurrent->toString();
}

void Statistics::write_csv(std::string *csvFilename) const {
    msg::error(csvFilename == nullptr, DBG_STR + "nullptr passed for csv file.");

    std::ofstream csvFs(*csvFilename);

    std::function<std::string(uint64_t)> cmdToString =
            [&] (uint64_t index) -> std::string {
                return commandString[index] + " count";
            };

    std::function<std::string(uint64_t)> cmdToStringCycles =
            [&] (uint64_t index) -> std::string {
                return commandString[index] + " cycles";
            };

    csvFs << "stat,value,unit,description" << std::endl
          << cmdCount->toCsvString(cmdToString)
          << cmdCycles->toCsvString(cmdToStringCycles)
          << totalCycleCount->toCsvString()
          << totalActStandbyCycles->toCsvString()
          << totalPreStandbyCycles->toCsvString()
          << totalReadEnergy->toCsvString()
          << totalWriteEnergy->toCsvString()
          << totalPreCmdEnergy->toCsvString()
          << totalActCmdEnergy->toCsvString()
          << totalPrechargeStandbyEnergy->toCsvString()
          << totalActiveStandbyEnergy->toCsvString()
          << totalEnergy->toCsvString()
          << avgPower->toCsvString()
          << avgCurrent->toCsvString();
    msg::info("Stats written as csv to `" + *csvFilename + "'.");
}

Statistics::Statistics(uint64_t (&structCount)[int(Level::MAX)], std::string *csvFilename) {
    this->csvFilename = csvFilename;
    this->structCount = structCount;

    totalEnergy.reset(new ScalarStat<double_t>(
            0,
            "total energy",
            "pJ",
            ""
    ));
    totalReadEnergy.reset(new ScalarStat<double_t>(
            0ul,
            "total read energy",
            "pJ",
            ""
    ));
    totalWriteEnergy.reset(new ScalarStat<double_t>(
            0ul,
            "total write energy",
            "pJ",
            ""
    ));
    totalActiveStandbyEnergy.reset(new ScalarStat<double_t>(
            0ul,
            "total active standby energy",
            "pJ",
            ""
    ));
    totalPrechargeStandbyEnergy.reset(new ScalarStat<double_t>(
            0ul,
            "total precharge standby energy",
            "pJ",
            ""
    ));
    cmdCount.reset(new VectorStat<uint64_t>(
            (uint64_t) (CommandType::MAX),
            0ul,
            "Count of each command type",
            "",
            ""
    ));
    cmdCycles.reset(new VectorStat<uint64_t>(
            (uint64_t) (CommandType::MAX),
            0ul,
            "Cycle count of each command type",
            "",
            "Number of memory cycles spent on a command"
    ));
    totalActStandbyCycles.reset(new ScalarStat<uint64_t>(
            0ul,
            "totalActStandbyCycles",
            "",
            "Total number of cycles for which the rank was in active standby mode"
    ));
    totalPreStandbyCycles.reset(new ScalarStat<uint64_t>(
            0ul,
            "totalPreStandbyCycles",
            "",
            "Total number of cycles for which the rank was in precharge standby mode"
    ));
    totalActCmdEnergy.reset(new ScalarStat<double_t>(
            0ul,
            "totalActCmdEnergy",
            "pJ",
            ""
    ));
    totalPreCmdEnergy.reset(new ScalarStat<double_t>(
            0ul,
            "totalPreCmdEnergy",
            "pJ",
            ""
    ));
    totalCycleCount.reset(new ScalarStat<uint64_t>(
            0ul,
            "totalCycleCount",
            "",
            ""
    ));
    avgPower.reset(new ScalarStat<double_t>(
            0.0,
            "avgPower",
            "mW",
            ""
    ));
    avgCurrent.reset(new ScalarStat<double_t>(
            0.0,
            "avgCurrent",
            "mA",
            ""
    ));
}


void Statistics::calculateTotal(DramSpec &dramSpec, uint64_t endTime) {
    *this->totalEnergy = *this->totalActCmdEnergy
                         + *this->totalPreCmdEnergy
                         + *this->totalActiveStandbyEnergy
                         + *this->totalPrechargeStandbyEnergy
                         + *this->totalReadEnergy
                         + *this->totalWriteEnergy;
    this->totalEnergy->setName("total energy");

    this->totalCycleCount->setValue(endTime);
    this->totalCycleCount->setName("totalCycleCount");
    this->avgPower->setValue(totalEnergy->getValue()/(2.5*this->totalCycleCount->getValue()));
    this->avgCurrent->setValue(this->avgPower->getValue()/dramSpec.vdd);
}

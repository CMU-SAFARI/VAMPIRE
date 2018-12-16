/*

STATISTICS.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#ifndef VAMPIRE_STATISTICS_H
#define VAMPIRE_STATISTICS_H


#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <functional>
#include "consts.h"
#include "dramStruct.h"
#include "helper.h"
#include "dramSpec.h"

template <typename T>
class ScalarStat {
private:
    T value;
    std::string name;
    std::string unit;
    std::string description;
public:
    ScalarStat(T value, std::string name, std::string unit, std::string description);

    std::string toString() const;
    std::string toCsvString() const;

    /* Operators */
    operator T()   {
        return value;
    }

    T &operator++() {
        return value++;
    }

    T &operator--() {
        return value--;
    }

    ScalarStat<T> operator+(ScalarStat<T> &obj) const {
        return ScalarStat<T>(this->value + obj.getValue(), this->name, this->unit, this->description);
    };

    template <typename ARG>
    ScalarStat<T> &operator+=(ARG value) {
        this->value += value;
        return *this;
    }

    template <typename ARG>
    ScalarStat<T> &operator*=(ARG value) {
        this->value *= value;
        return *this;
    }

    /* Getters and setters */
    T getValue() const {return value;};

    void setValue(T value);
    const std::string &getName() const;

    void setName(const std::string &name);
    const std::string &getUnit() const;

    void setUnit(const std::string &unit);
    const std::string &getDescription() const;

    void setDescription(const std::string &description);
};

template <typename T>
class VectorStat {
private:
    std::vector<T> *members;
public:
    std::string name;
    std::string unit;
    std::string description;
    VectorStat(uint64_t memberCount, T initialValue, std::string name, std::string unit, std::string description);
    ~VectorStat();

    T &operator[](uint64_t index) {return members->operator[](index);};

    std::string toString() const {
        std::stringstream ss;
        for (int i = 0; i < members->size(); i++) {
            ss << std::setw(36) << this->name
               << "[" << std::setw(2) << std::setfill('0') << i << std::setfill(' ') << "]: "
               << std::setw(20)
               << this->members->operator[](i)
               << std::setw(5) << this->unit
               << "  # " << this->description
               << std::endl;
        }
        return ss.str();
    };

    std::string toString(std::function<std::string(uint64_t)> mappingFun) const {
        std::stringstream ss;
        for (uint64_t i = 0; i < members->size(); i++) {
            ss << std::setw(40) << mappingFun(i) << ": "
               << std::setw(20)
               << this->members->operator[](i)
               << std::endl;
        }
        return ss.str();
    }

    std::string toCsvString() const;
    std::string toCsvString(std::function<std::string(uint64_t)> mappingFun) const;

    /* For C++11 range based loops */
    typename std::vector<T>::iterator begin() {return this->members->begin();};
    typename std::vector<T>::iterator end() {return this->members->end();};
};



/* Stream operators */
template <typename T>
std::ostream& operator<<(std::ostream& ostr, ScalarStat<T>& stat) {
    ostr << stat.toString();
}

template <typename T>
std::ostream& operator<<(std::ostream& ostr, VectorStat<T>& stat) {
    ostr << stat.toString();
}



class Vampire;
class Statistics {
private:
    uint64_t *structCount = nullptr;
    std::string *csvFilename = nullptr;
protected:
public:
    std::shared_ptr<ScalarStat<uint64_t>>    totalActStandbyCycles;
    std::shared_ptr<ScalarStat<uint64_t>>    totalPreStandbyCycles;
    
    std::shared_ptr<ScalarStat<double_t>>    totalEnergy;
    std::shared_ptr<ScalarStat<double_t>>    totalReadEnergy;
    std::shared_ptr<ScalarStat<double_t>>    totalWriteEnergy;
    std::shared_ptr<ScalarStat<double_t>>    totalActCmdEnergy;
    std::shared_ptr<ScalarStat<double_t>>    totalPreCmdEnergy;
    std::shared_ptr<ScalarStat<double_t>>    totalActiveStandbyEnergy;
    std::shared_ptr<ScalarStat<double_t>>    totalPrechargeStandbyEnergy;
    
    std::shared_ptr<VectorStat<uint64_t>>    cmdCount;              // One for each commandType
    std::shared_ptr<VectorStat<uint64_t>>    cmdCycles;             // One for each commandType
    
    std::shared_ptr<ScalarStat<uint64_t>>    totalCycleCount;
    
    std::shared_ptr<ScalarStat<double_t>>    avgPower;
    std::shared_ptr<ScalarStat<double_t>>    avgCurrent;

    explicit Statistics(Statistics &statistics, std::string *csvFilename) : csvFilename(csvFilename) {}
    explicit Statistics(uint64_t (&structCount)[int(Level::MAX)], std::string *csvFilename);
    ~Statistics() = default;
    
    void print_stats() const;
    void write_csv(std::string *csvFilename) const;
    void calculateTotal(DramSpec &dramSpec, uint64_t endTime);
};

#endif //VAMPIRE_STATISTICS_H

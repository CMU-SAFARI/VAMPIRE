//
// Created by suyash on 7/16/18.
//

#ifndef VAMPIRE_COMMAND_H
#define VAMPIRE_COMMAND_H


#include <sstream>
#include "address.h"
#include "helper.h"

class Command {
private:
public:
    CommandType type;
    MappedAdd add;
    uint64_t issueTime = 0ul;
    uint64_t finishTime = 0ul;
    unsigned int data[16];
    Command() = default;

    Command(CommandType cmdType, const MappedAdd add, uint64_t issueTime, uint64_t finishTime, unsigned int data[16]);

    /* Getters and Setters */
    uint64_t getIssueTime() const;
    void setIssueTime(uint64_t issueTime);

    uint64_t getFinishTime() const;
    void setFinishTime(uint64_t finishTime);

    const MappedAdd &getAdd() const;
    void setAdd(const MappedAdd &add);

    CommandType getType() const;
    void setType(CommandType type);
};

std::ostream& operator<<(std::ostream& ostr, CommandType& cmdType);
std::ostream& operator<<(std::ostream& ostr, Command& cmd);


#endif //VAMPIRE_COMMAND_H

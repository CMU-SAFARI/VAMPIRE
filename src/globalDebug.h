//
// Created by suyash on 12/16/18.
//

#ifndef VAMPIRE_GLOBALDEBUG_H
#define VAMPIRE_GLOBALDEBUG_H

//#define GLOBAL_DEBUG

#ifdef GLOBAL_DEBUG
#define dbgstream std::cout
#else
#define dbgstream 0 && std::cout
#endif

#endif //VAMPIRE_GLOBALDEBUG_H

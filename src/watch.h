//
// Created by Pedro Tôrres on 2018-12-01.
//

#ifndef IPMT_WATCH_H
#define IPMT_WATCH_H


#define __WATCH_BEGIN_IDENTIFIER(i) __CONCAT(__watch_begin, i)
#define __WATCH_END_IDENTIFIER(i) __CONCAT(__watch_end, i)
#define __WATCH_DEFINITION(e, i) clock_t __WATCH_BEGIN_IDENTIFIER(i) = clock(); e; clock_t __WATCH_END_IDENTIFIER(i) = clock(); std::cerr << "[WATCH#" << i << "@" << __FILE__ << ":" << __LINE__ << "] " << static_cast<long double>(__WATCH_END_IDENTIFIER(i) - __WATCH_BEGIN_IDENTIFIER(i)) / CLOCKS_PER_SEC << 's' << std::endl;
#define WATCH(e) __WATCH_DEFINITION(e, __COUNTER__)


#endif //IPMT_WATCH_H

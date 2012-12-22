#ifndef PACKETS_H
#define PACKETS_H


#define DELIM "|"
//#define DELIM 0x10
////////////////////////////////////////////////////////////////////////////////

typedef struct _P_HEADER {
    unsigned int len; // msg 길이. 즉, 전체길이는 msg + len
} P_HEADER;

#endif // PACKETS_H

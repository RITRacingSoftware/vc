#ifndef CAN_H
#define CAN_H

typedef struct
{
    int id;
    int dlc;
    uint64_t data;
} can_message_s;



#endif // CAN_H
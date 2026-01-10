#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#pragma pack(push, 1)

struct employee {
    int num;              
    char name[10];          
    double hours;          
};

#pragma pack(pop)


enum Operation {
    OP_READ = 1,
    OP_WRITE = 2,
    OP_MODIFY = 3,
    OP_EXIT = 4
};


struct Request {
    Operation op;          
    int employee_id;       
};

struct Response {
    int success;            
    employee emp;           
    char error_msg[100];    
};

#endif // EMPLOYEE_H
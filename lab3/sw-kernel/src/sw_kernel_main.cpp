#include <stdlib.h>
#include <ctime>
#include "lnh64.hxx"
#include "gpc_io_swk.h"
#include "gpc_handlers.h"
#include "iterators.h"
#include "common_struct.h"
#include "compose_keys.hxx"

#define __fast_recall__

extern lnh lnh_core;
volatile unsigned int event_source;

int main(void) {
    /////////////////////////////////////////////////////////
    //                  Main Event Loop
    /////////////////////////////////////////////////////////
    //Leonhard driver structure should be initialised
    lnh_init();
    for (;;) {
        //Wait for event
        event_source = wait_event();
        switch(event_source) {
            /////////////////////////////////////////////
            //  Measure GPN operation frequency
            /////////////////////////////////////////////
            case __event__(create_students) : create_students(); break;
            case __event__(create_housing_applications) : create_housing_applications(); break;
            case __event__(select) : select(); break;
        }
        set_gpc_state(READY);
    }
}
    
//-------------------------------------------------------------
//      Вставка ключа и значения в структуру
//-------------------------------------------------------------
 
void create_students() {
    while(1) {
        auto key=students::key::from_int(mq_receive());
        if (key==-1ull) break;
        auto val=students::val::from_int(mq_receive());
        STUDENTS.ins_async(key, val); //Вставка в таблицу с типизацией uint64_t
    }
}

void create_housing_applications() {
    while(1) {
        auto key = housing_applications::key::from_int(mq_receive());
        if (key==-1ull) break;
        auto val = housing_applications::val::from_int(mq_receive());
        HOUSING_APPLICATIONS.ins_async(key, val); //Вставка в таблицу с типизацией uint64_t
    }
}


void select() {
    while(1){
        uint32_t student = mq_receive();
        if (student == -1) {
            break;
        }

        auto housing_application = HOUSING_APPLICATIONS.nsm(housing_applications::key{.idx=housing_applications::idx_min,.housing_application_id=housing_applications::idx_min});
        while (housing_application) {
                if (housing_application.value().student_id == student) {
                    mq_send(housing_application.key().housing_application_id);
                    mq_send(housing_application.value().status);
                }
                housing_application = HOUSING_APPLICATIONS.nsm(housing_application.key());
        }
        mq_send(-1ull);
        mq_send(-1ull); // 2х null так как плохо обрабатывается выход при получении на хосте
    }
}


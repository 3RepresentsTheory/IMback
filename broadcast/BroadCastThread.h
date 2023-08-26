//
// Created by no3core on 2023/8/26.
//


#ifndef DEMO02_BROADCASTTHREAD_H
#define DEMO02_BROADCASTTHREAD_H

#include <QThread>
#include <iostream>

class BroadCastThread: public QThread{
    Q_OBJECT
public:
    void run() override{
        std::cout<<"Broadcast thread start!"<<std::endl;

        exec();
    }
};

#endif //DEMO02_BROADCASTTHREAD_H
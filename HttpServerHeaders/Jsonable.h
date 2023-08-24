//
// Created by 28443 on 2023/8/24.
//

#ifndef DEMO02_JSONABLE_H
#define DEMO02_JSONABLE_H
#include "types.h"
#include "utils.h"
#include <QtConcurrent/qtconcurrentrun.h>
#include <QtHttpServer/QHttpServer>

class jsonable{
public:
    virtual QJsonObject toJsonObject() = 0;
    virtual bool formJsonObject(QJsonObject& json) = 0;
};

#endif //DEMO02_JSONABLE_H

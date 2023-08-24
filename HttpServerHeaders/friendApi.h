//
// Created by 28443 on 2023/8/24.
//

#ifndef DEMO02_FRIENDAPI_H
#define DEMO02_FRIENDAPI_H
#include "../service/friendService.h"
#include "types.h"
#include "utils.h"
#include <QtConcurrent/qtconcurrentrun.h>

#include <QtHttpServer/QHttpServer>

class FriendApi {
private:
    FriendService* friendService;
public:
    FriendApi();
    ~FriendApi();
    QHttpServerResponse request(const QHttpServerRequest &request);
};


#endif //DEMO02_FRIENDAPI_H

//
// Created by 28443 on 2023/8/24.
//

#include "friendApi.h"

FriendApi::FriendApi() {
    friendService = new FriendService();
}

FriendApi::~FriendApi() {
    if(!friendService){
        delete(friendService);
    }
}

QHttpServerResponse FriendApi::request(const QHttpServerRequest &request) {
    return QHttpServerResponse("OK");
}

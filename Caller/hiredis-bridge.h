//
//  hiredis-bridge.h
//  Caller
//
//  Created by 刘闽晟 on 7/1/14.
//  Copyright (c) 2014 Selveskii. All rights reserved.
//

#import "../hiredis/hiredis.h"
#import "../hiredis/async.h"
#include "../hiredis/libdispatch.h"

int getErrRedisContext (redisContext *context) {
    return context->err;
}

int getErrRedisAsyncContext (redisAsyncContext *context) {
    return context->err;
}

const char* getErrStrRedisContext (redisContext *context) {
    return context->errstr;
}

const char* getErrStrRedisAsyncContext (redisAsyncContext *context) {
    return context->errstr;
}

redisReply* redisCommandWrapper (redisContext *ctx, const char *cmd) {
    return redisCommand(ctx, cmd);
}

void redisAppendCommandWrapper (redisContext *ctx, const char *cmd) {
    redisAppendCommand(ctx, cmd);
}

redisReply* redisGetReplyWrapper (redisContext *ctx) {
    redisReply *ptr;
    if (redisGetReply(ctx, (void **)&ptr) == REDIS_OK) {
        return ptr;
    }
    else {
        return NULL;
    }
}

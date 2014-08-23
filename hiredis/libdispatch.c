/*
 Copyright (c) 2014 Minsheng Liu
 I have no idea of how to write a proper copyright declartion here.
 
 Anyway, I copy the code of this "hiredis async adapter to integrate
 with GCD" and modify it somehow.
 
 The original copyright and permission notice is included hereby.
 
 Copyright (c) 2012 Sébastien Martini <seb@dbzteam.org>
 
 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>

#include "libdispatch.h"

void redisLibdispatchAddRead(void *privdata) {
    redisLibdispatchEvents *e = (redisLibdispatchEvents*)privdata;
    if (!e->reading) {
        e->reading = 1;
        dispatch_resume(e->rev);
    }
}

void redisLibdispatchDelRead(void *privdata) {
    redisLibdispatchEvents *e = (redisLibdispatchEvents*)privdata;
    if (!e->reading) {
        e->reading = 0;
        dispatch_suspend(e->rev);
    }
}

void redisLibdispatchAddWrite(void *privdata) {
    redisLibdispatchEvents *e = (redisLibdispatchEvents*)privdata;
    if (!e->writing) {
        e->writing = 1;
        dispatch_resume(e->wev);
    }
}

void redisLibdispatchDelWrite(void *privdata) {
    redisLibdispatchEvents *e = (redisLibdispatchEvents*)privdata;
    if (e->writing) {
        e->writing = 0;
        dispatch_suspend(e->wev);
    }
}

void redisLibdispatchCleanup(void *privdata) {
    redisLibdispatchEvents *e = (redisLibdispatchEvents*)privdata;
    
    if (e->rev != NULL && dispatch_source_testcancel(e->rev) == 0) {
        redisLibdispatchAddRead(privdata);
        dispatch_source_cancel(e->rev);
        dispatch_release(e->rev);
    }
    
    if (e->wev != NULL && dispatch_source_testcancel(e->wev) == 0) {
        redisLibdispatchAddWrite(privdata);
        dispatch_source_cancel(e->wev);
        dispatch_release(e->wev);
    }
    
    free(e);
}

int redisLibdispatchAttach(redisAsyncContext *ac, dispatch_queue_t queue) {
    redisContext *c = &(ac->c);
    redisLibdispatchEvents *e;
    
    // Nothing should be attached when something is already attached
    if (ac->ev.data != NULL)
        return REDIS_ERR;
    
    // Initialize and install read/write events
    dispatch_source_t rev = dispatch_source_create(DISPATCH_SOURCE_TYPE_READ,
                                                   c->fd, 0, queue);
    if (rev == NULL)
        return REDIS_ERR_IO;
    dispatch_source_set_event_handler(rev, ^{
        redisAsyncHandleRead(ac);
    });
    
    dispatch_source_t wev = dispatch_source_create(DISPATCH_SOURCE_TYPE_WRITE,
                                                   c->fd, 0, queue);
    if (wev == NULL)
        return REDIS_ERR_IO;
    dispatch_source_set_event_handler(wev, ^{
        redisAsyncHandleWrite(ac);
    });
    
    // Create container for context and r/w events
    e = (redisLibdispatchEvents*)malloc(sizeof(*e));
    e->context = ac;
    e->reading = e->writing = 0;
    
    // Register functions to start/stop listening for events
    ac->ev.addRead = redisLibdispatchAddRead;
    ac->ev.delRead = redisLibdispatchDelRead;
    ac->ev.addWrite = redisLibdispatchAddWrite;
    ac->ev.delWrite = redisLibdispatchDelWrite;
    ac->ev.cleanup = redisLibdispatchCleanup;
    ac->ev.data = e;
    e->rev = rev;
    e->wev = wev;
    
    return REDIS_OK;
}
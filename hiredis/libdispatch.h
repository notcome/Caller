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

#ifndef __HIREDIS_LIBDISPATCH_H__
#define __HIREDIS_LIBDISPATCH_H__

#include <dispatch/dispatch.h>

#include "hiredis.h"
#include "async.h"

typedef struct redisLibdispatchEvents {
    redisAsyncContext *context;
    int reading, writing;
    __unsafe_unretained dispatch_source_t rev, wev;
} redisLibdispatchEvents;

void redisLibdispatchAddRead(void *privdata);
void redisLibdispatchDelRead(void *privdata);
void redisLibdispatchAddWrite(void *privdata);
void redisLibdispatchDelWrite(void *privdata);
void redisLibdispatchCleanup(void *privdata);
int redisLibdispatchAttach(redisAsyncContext *ac, dispatch_queue_t queue);

#endif  // __HIREDIS_LIBDISPATCH_H__

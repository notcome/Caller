//
//  RedisAsyncClient.swift
//  Caller
//
//  Created by 刘闽晟 on 7/2/14.
//  Copyright (c) 2014 Selveskii. All rights reserved.
//

import Foundation

class RedisAsyncClient: RedisClientProtocol {
    let connection: UnsafePointer<redisAsyncContext>
    
    init (ip: String = "127.0.0.1", port: Int = 6379) {
        connection = redisAsyncConnect(ip.bridgeToObjectiveC().UTF8String, CInt(port))
        if self.getError() == nil {
            redisLibdispatchAttach(connection, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0))
        }
    }
    
    deinit {
        redisAsyncFree(connection)
    }

    func getError () -> (type: RedisContextError, message: String)? {
        let type = Int(getErrRedisAsyncContext(connection))
        
        //if type is neither 0 nor a memeber of RedisError's raw values,
        //then something must be wrong.
        if type != 0 {
            let msg = String.fromCString(getErrStrRedisAsyncContext(connection))
            return (RedisContextError.fromRaw(type)!, msg)
        } else {
            return nil
        }
    }
}

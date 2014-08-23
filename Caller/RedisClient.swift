//
//  SyncClient.swift
//  Caller
//
//  Created by 刘闽晟 on 7/1/14.
//  Copyright (c) 2014 Selveskii. All rights reserved.
//

import Foundation

class RedisClient: RedisClientProtocol {
    let connection: UnsafePointer<redisContext>
    
    init (ip: String = "127.0.0.1", port: Int = 6379) {
        connection = redisConnect(ip.bridgeToObjectiveC().UTF8String, CInt(port))
    }
    
    deinit {
        redisFree(connection)
    }
    
    func getError () -> (type: RedisContextError, message: String)? {
        let type = Int(getErrRedisContext(connection))
        
        //if type is neither 0 nor a memeber of RedisError's raw values,
        //then something must be wrong.
        if type != 0 {
            let msg = String.fromCString(getErrStrRedisContext(connection))
            return (RedisContextError.fromRaw(type)!, msg)
        } else {
            return nil
        }
    }
    
    func sendCommand (command: String) -> RedisReply {
        let reply = redisCommandWrapper(connection, command.bridgeToObjectiveC().UTF8String)
        return handleReply(self, reply)
    }
}

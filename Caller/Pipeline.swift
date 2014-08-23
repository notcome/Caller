//
//  Pipeline.swift
//  Caller
//
//  Created by 刘闽晟 on 7/1/14.
//  Copyright (c) 2014 Selveskii. All rights reserved.
//

import Foundation

class Pipeline {
    var commandPile: Array<String>
    var replyPointer: UnsafePointer<redisReply>
    let client: RedisClient
    let connection: UnsafePointer<redisContext>
    
    init (client: RedisClient) {
        commandPile = []
        replyPointer = UnsafePointer<redisReply>()
        self.client = client
        connection = self.client.connection
    }
    
    deinit {
        if replyPointer != UnsafePointer<RedisReply>.null() {
            freeReplyObject(replyPointer)
        }
    }
    
    func add (command: String) -> Pipeline {
        commandPile.append(command)
        return self
    }
    
    func done () -> RedisReply {
        for command in commandPile {
            redisAppendCommandWrapper(connection, command.bridgeToObjectiveC().UTF8String)
        }
        
        replyPointer = redisGetReplyWrapper(connection)
        return client.handleReply(replyPointer)
    }
}

extension RedisClient {
    func pipeline () -> Pipeline {
        return Pipeline(client: self)
    }
}
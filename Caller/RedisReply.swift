//
//  RedisReply.swift
//  Caller
//
//  Created by 刘闽晟 on 7/1/14.
//  Copyright (c) 2014 Selveskii. All rights reserved.
//

import Foundation

enum RedisContextError: Int {
    case IO = 1, Other, EOF, Protocol, OOM
}

enum RedisReply {
    case Text(String)
    case Error(String)
    case Status(String)
    case Integer(Int)
    case Nil
    case Array(RedisReply[])
    case ConnError(RedisContextError, String)
}

func destructureRedisReply (inout reply: redisReply) -> RedisReply {
    var destructured = RedisReply.Nil
    
    switch reply.type {
    case REDIS_REPLY_STRING:
        destructured = .Text(String.fromCString(reply.str))
    case REDIS_REPLY_ERROR:
        destructured = .Error(String.fromCString(reply.str))
    case REDIS_REPLY_STATUS:
        destructured = .Status(String.fromCString(reply.str))
    case REDIS_REPLY_INTEGER:
        destructured = .Integer(Int(reply.type))
    case REDIS_REPLY_ARRAY:
        let size = Int(reply.elements)
        var replies = RedisReply[](count: size, repeatedValue: RedisReply.Nil)
        for i in 0..size {
            replies[i] = destructureRedisReply(&reply.element[i].memory)
        }
        destructured = .Array(replies)
    default:
        break
    }
    
    return destructured
}

protocol RedisClientProtocol {
    func getError () -> (type: RedisContextError, message: String)?
}

func handleReply<ClientType: RedisClientProtocol> (client: ClientType, reply: UnsafePointer<redisReply>) -> RedisReply {
    if reply != UnsafePointer<redisReply>.null() {
        let result = destructureRedisReply(&reply.memory)
        freeReplyObject(reply)
        return result
    } else {
        let (type, msg) = client.getError()!
        return RedisReply.ConnError(type, msg)
    }
}

//
//  main.swift
//  Caller
//
//  Created by 刘闽晟 on 7/1/14.
//  Copyright (c) 2014 Selveskii. All rights reserved.
//

import Foundation

println("Hello, World!")

let client = RedisClient()

let reply = client.pipeline().add("SET A 1").add("SET B 2").add("SET C 3").done()

switch reply {
case .Status (let status):
    println(status)
case .Error (let error):
    println(error)
case .Text (let text):
    println(text)
case .Integer (let integer):
    println(integer)
case .Nil:
    println("nil")
case .ConnError (let type, let error):
    println(error)
default:
    println("bulk of data")
}

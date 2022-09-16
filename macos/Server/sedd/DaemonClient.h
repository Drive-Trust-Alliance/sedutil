//
//  DaemonClient.h
//  SED
//
//  A DaemonClient is attached to each Connection created by a network server.
//  It interprets commands and produces responses.
//
//  Created by Scott Marks on 07/05/2017.
//
//

#import <Foundation/Foundation.h>
#import "SEDClientServerCommunication.h"
#import "DeviceChangeDelegate.h"

@class DaemonClientController;
@interface DaemonClient: NSObject<DeviceChangeDelegate>
+(instancetype) clientWithInputStream:(NSInputStream *) i outputStream:(NSOutputStream *) o
                           netService:(NSNetService *) n
                         loopbackPort:(NSUInteger) loopbackPort
                           isLoopback:(bool) isLoopback
                           controller:(DaemonClientController *) controller;
@property (nonatomic,readonly) Connection * connection;
@end

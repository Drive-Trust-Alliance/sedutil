//
//  TPerUserClient.h
//  seddaemonandclient
//
//  Created by Scott Marks on 04/23/2016.
//  Copyright © 2016 MagnoliaHeights. All rights reserved.
//

#import <sedutil/sedutil.h>
#import "IOObject.h"

@interface TPerUserClient: IOObject
+(instancetype) connectionToDriver:(IORegistryEntry *)driver;
@end

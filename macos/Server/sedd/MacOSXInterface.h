//
//  MacOSXInterface.h
//  sedagent
//
//  Created by Jackie Marks on 04/2/2016.
//  Copyright © 2016 Bright Plaza Inc. All rights reserved.
//

#import "OSInterface.h"
#import "MacFileSystemInterface.h"

@interface MacOSXInterface: OSInterface
@property (nonatomic, strong, readwrite) MacFileSystemInterface * fileSystemInterface;
- (TCGSTATUSCODE) unlockDrive:(TPer *)tPer hostChallenge:(NSData *)hostChallenge;  // for automatic unlocking
@end

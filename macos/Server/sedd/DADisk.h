//
//  DADisk.h
//  seddaemonandclient
//
//  Created by Scott Marks on 05/13/2016.
//  Copyright © 2016 MagnoliaHeights. All rights reserved.
//

#include <DiskArbitration/DiskArbitration.h>
//#import "ContinuationDefs.h"
#import "IOObject.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef void (* callbackfn)(_Nonnull DADiskRef disk, _Nullable DADissenterRef dissenter,  void * _Nullable  interface );

@interface DADisk: NSObject
+(nullable DADisk *) diskFromDiskRef:(nonnull DADiskRef)diskRef;
+(nullable DADisk *) diskFromBSDName:(nonnull NSString  *)bsdName session:(nonnull DASessionRef)session;
-(void) claimWithCallback:(nullable callbackfn)callback userInfo:(nullable void *)interface;
-(void) unclaim;
-(void) unmountWithCallback:(nullable callbackfn)callback userInfo:(nullable void *)interface;
-(void) mountWithCallback:(nullable callbackfn)callback whole:(bool)whole userInfo:(nullable void *)interface;

-(nullable NSString *)bsdName;
-(nullable IORegistryEntry *)media;
@property (nonatomic,readonly)  _Nullable DADiskRef disk;
@end

#if defined(__cplusplus)
}
#endif

//
//  SEDNotifications.h
//  SED
//
//  Created by Scott Marks on 09/27/2017.
//
//
#import <Availability.h>

#if __MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_11_0

#import <Foundation/Foundation.h>
#define _NOTIFICATION_DELEGATE_PROTOCOL NSUserNotificationCenterDelegate

#else

#import <UserNotifications/UserNotifications.h>
#define _NOTIFICATION_DELEGATE_PROTOCOL UNUserNotificationCenterDelegate

#endif


@interface SEDUserNotificationCenterDelegate: NSObject<_NOTIFICATION_DELEGATE_PROTOCOL>
- (void) becomeNotificationDelegate;
@end

#undef _NOTIFICATION_DELEGATE_PROTOCOL


extern
void showNotificationAutomaticallyUnlockingTPer(NSString * WWN, NSString * deviceName);

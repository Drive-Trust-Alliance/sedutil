//
//  SEDNotifications.mm
//  SED
//
//  Created by Scott Marks on 09/27/2017.
//
//


#import "SEDNotifications.h"
@implementation SEDUserNotificationCenterDelegate
#if __MAC_OS_X_VERSION_MIN_REQUIRED && __MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_11_0
- (void) becomeNotificationDelegate {
   [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:self];
}
- (BOOL)userNotificationCenter:(__unused NSUserNotificationCenter *)center
     shouldPresentNotification:(__unused NSUserNotification *) notification {
    return YES;
}
#else
- (void) becomeNotificationDelegate {
    [[UNUserNotificationCenter currentNotificationCenter] setDelegate:self];
}
#endif

//@protocol UNUserNotificationCenterDelegate <NSObject>
//
//@optional
//
//// The method will be called on the delegate only if the application is in the foreground. If the method is not implemented or the handler is not called in a timely manner then the notification will not be presented. The application can choose to have the notification presented as a sound, badge, alert and/or in the notification list. This decision should be based on whether the information in the notification is otherwise visible to the user.
//- (void)userNotificationCenter:(UNUserNotificationCenter *)center willPresentNotification:(UNNotification *)notification withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler __API_AVAILABLE(macos(10.14), ios(10.0), watchos(3.0), tvos(10.0));
//
//// The method will be called on the delegate when the user responded to the notification by opening the application, dismissing the notification or choosing a UNNotificationAction. The delegate must be set before the application returns from application:didFinishLaunchingWithOptions:.
//- (void)userNotificationCenter:(UNUserNotificationCenter *)center didReceiveNotificationResponse:(UNNotificationResponse *)response withCompletionHandler:(void(^)(void))completionHandler __API_AVAILABLE(macos(10.14), ios(10.0), watchos(3.0)) __API_UNAVAILABLE(tvos);
//
//// The method will be called on the delegate when the application is launched in response to the user's request to view in-app notification settings. Add UNAuthorizationOptionProvidesAppNotificationSettings as an option in requestAuthorizationWithOptions:completionHandler: to add a button to inline notification settings view and the notification settings view in Settings. The notification will be nil when opened from Settings.
//- (void)userNotificationCenter:(UNUserNotificationCenter *)center openSettingsForNotification:(nullable UNNotification *)notification __API_AVAILABLE(macos(10.14), ios(12.0)) __API_UNAVAILABLE(watchos, tvos);
//
//@end
@end

//void showNotificationAutomaticallyUnlockingTPer(NSString * WWN, NSString * deviceName) {
//    NSUserNotification *notification = [[NSUserNotification alloc] init];
//    notification.title = [NSString stringWithFormat:@"%@ unlocked", WWN];
//    notification.informativeText = [NSString stringWithFormat:@"%@ automatically unlocked as %@", WWN, deviceName];
//    notification.soundName = NSUserNotificationDefaultSoundName;
//    
//    [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
//}

void showNotificationAutomaticallyUnlockingTPer(NSString * WWN, NSString * deviceName) {
    NSString * title = [NSString stringWithFormat:@"%@ unlocked", WWN];
    NSString * informativeText = [NSString stringWithFormat:@"%@ automatically unlocked as %@", WWN, deviceName];
    CFTimeInterval timeout = 15.0; // seconds
    CFOptionFlags flags = kCFUserNotificationPlainAlertLevel;
    CFURLRef iconURL = NULL;
    CFURLRef soundURL = NULL;
    CFURLRef localizationURL = NULL ;
    CFStringRef alertHeader = (__bridge CFStringRef)title;
    CFStringRef alertMessage = (__bridge CFStringRef)informativeText;
    CFStringRef defaultButtonTitle = NULL;
    SInt32 result = CFUserNotificationDisplayNotice(timeout, flags, iconURL, soundURL, localizationURL,
                                                    alertHeader, alertMessage, defaultButtonTitle);
    (void)(result);
}

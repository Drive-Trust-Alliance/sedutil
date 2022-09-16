//
//  main.m
//  CBServer
//
//  Created by Scott Marks on 12/19/2017.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

@interface CBServer:NSObject<CBPeripheralManagerDelegate>
- (void) start;
- (void) stop;
@end
@interface CBServer()
@property (nonatomic, readwrite, strong) CBPeripheralManager * manager;
@end

@implementation CBServer
@synthesize manager;

- (instancetype) init {
    if ((self = [ super init ])) {
        self.manager = [[CBPeripheralManager alloc] initWithDelegate:self queue:nil options:nil ];
    }
    return self;
}

- (void) start {
    
}
- (void) stop {
    
}

#pragma mark - CBPeripheralManagerDelegate

// @required

/*!
 *  @method peripheralManagerDidUpdateState:
 *
 *  @param peripheral   The peripheral manager whose state has changed.
 *
 *  @discussion         Invoked whenever the peripheral manager's state has been updated. Commands should only be issued when the state is
 *                      <code>CBPeripheralManagerStatePoweredOn</code>. A state below <code>CBPeripheralManagerStatePoweredOn</code>
 *                      implies that advertisement has paused and any connected centrals have been disconnected. If the state moves below
 *                      <code>CBPeripheralManagerStatePoweredOff</code>, advertisement is stopped and must be explicitly restarted, and the
 *                      local database is cleared and all services must be re-added.
 *
 *  @see                state
 *
 */
/*!
 *  @enum CBPeripheralManagerState
 *
 *  @discussion Represents the current state of a CBPeripheralManager.
 *
 *  @constant CBPeripheralManagerStateUnknown       State unknown, update imminent.
 *  @constant CBPeripheralManagerStateResetting     The connection with the system service was momentarily lost, update imminent.
 *  @constant CBPeripheralManagerStateUnsupported   The platform doesn't support the Bluetooth Low Energy Peripheral/Server role.
 *  @constant CBPeripheralManagerStateUnauthorized  The application is not authorized to use the Bluetooth Low Energy Peripheral/Server role.
 *  @constant CBPeripheralManagerStatePoweredOff    Bluetooth is currently powered off.
 *  @constant CBPeripheralManagerStatePoweredOn     Bluetooth is currently powered on and available to use.
 *
 */
 (void)peripheralManagerDidUpdateState:(CBPeripheralManager *)peripheral {
    
}

// @optional

/*!
 *  @method peripheralManager:willRestoreState:
 *
 *  @param peripheral    The peripheral manager providing this information.
 *  @param dict            A dictionary containing information about <i>peripheral</i> that was preserved by the system at the time the app was terminated.
 *
 *  @discussion            For apps that opt-in to state preservation and restoration, this is the first method invoked when your app is relaunched into
 *                        the background to complete some Bluetooth-related task. Use this method to synchronize your app's state with the state of the
 *                        Bluetooth system.
 *
 *  @seealso            CBPeripheralManagerRestoredStateServicesKey;
 *  @seealso            CBPeripheralManagerRestoredStateAdvertisementDataKey;
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral willRestoreState:(NSDictionary<NSString *, id> *)dict {
    
}
/*!
 *  @method peripheralManagerDidStartAdvertising:error:
 *
 *  @param peripheral   The peripheral manager providing this information.
 *  @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion         This method returns the result of a @link startAdvertising: @/link call. If advertisement could
 *                      not be started, the cause will be detailed in the <i>error</i> parameter.
 *
 */
- (void)peripheralManagerDidStartAdvertising:(CBPeripheralManager *)peripheral error:(nullable NSError *)error {
    
}
/*!
 *  @method peripheralManager:didAddService:error:
 *
 *  @param peripheral   The peripheral manager providing this information.
 *  @param service      The service that was added to the local database.
 *  @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion         This method returns the result of an @link addService: @/link call. If the service could
 *                      not be published to the local database, the cause will be detailed in the <i>error</i> parameter.
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral didAddService:(CBService *)service error:(nullable NSError *)error {
    
}

/*!
 *  @method peripheralManager:central:didSubscribeToCharacteristic:
 *
 *  @param peripheral       The peripheral manager providing this update.
 *  @param central          The central that issued the command.
 *  @param characteristic   The characteristic on which notifications or indications were enabled.
 *
 *  @discussion             This method is invoked when a central configures <i>characteristic</i> to notify or indicate.
 *                          It should be used as a cue to start sending updates as the characteristic value changes.
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral central:(CBCentral *)central didSubscribeToCharacteristic:(CBCharacteristic *)characteristic {
    
}

/*!
 *  @method peripheralManager:central:didUnsubscribeFromCharacteristic:
 *
 *  @param peripheral       The peripheral manager providing this update.
 *  @param central          The central that issued the command.
 *  @param characteristic   The characteristic on which notifications or indications were disabled.
 *
 *  @discussion             This method is invoked when a central removes notifications/indications from <i>characteristic</i>.
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral central:(CBCentral *)central didUnsubscribeFromCharacteristic:(CBCharacteristic *)characteristic {
    
}

/*!
 *  @method peripheralManager:didReceiveReadRequest:
 *
 *  @param peripheral   The peripheral manager requesting this information.
 *  @param request      A <code>CBATTRequest</code> object.
 *
 *  @discussion         This method is invoked when <i>peripheral</i> receives an ATT request for a characteristic with a dynamic value.
 *                      For every invocation of this method, @link respondToRequest:withResult: @/link must be called.
 *
 *  @see                CBATTRequest
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral didReceiveReadRequest:(CBATTRequest *)request {
    
}

/*!
 *  @method peripheralManager:didReceiveWriteRequests:
 *
 *  @param peripheral   The peripheral manager requesting this information.
 *  @param requests     A list of one or more <code>CBATTRequest</code> objects.
 *
 *  @discussion         This method is invoked when <i>peripheral</i> receives an ATT request or command for one or more characteristics with a dynamic value.
 *                      For every invocation of this method, @link respondToRequest:withResult: @/link should be called exactly once. If <i>requests</i> contains
 *                      multiple requests, they must be treated as an atomic unit. If the execution of one of the requests would cause a failure, the request
 *                      and error reason should be provided to <code>respondToRequest:withResult:</code> and none of the requests should be executed.
 *
 *  @see                CBATTRequest
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral didReceiveWriteRequests:(NSArray<CBATTRequest *> *)requests {
    
}

/*!
 *  @method peripheralManagerIsReadyToUpdateSubscribers:
 *
 *  @param peripheral   The peripheral manager providing this update.
 *
 *  @discussion         This method is invoked after a failed call to @link updateValue:forCharacteristic:onSubscribedCentrals: @/link, when <i>peripheral</i> is again
 *                      ready to send characteristic value updates.
 *
 */
- (void)peripheralManagerIsReadyToUpdateSubscribers:(CBPeripheralManager *)peripheral {
    
}

/*!
 *  @method peripheralManager:didPublishL2CAPChannel:error:
 *
 *  @param peripheral   The peripheral manager requesting this information.
 *  @param PSM            The PSM of the channel that was published.
 *  @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion         This method is the response to a  @link publishL2CAPChannel: @/link call.  The PSM will contain the PSM that was assigned for the published
 *                        channel
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral didPublishL2CAPChannel:(CBL2CAPPSM)PSM error:(nullable NSError *)error {
    
}

/*!
 *  @method peripheralManager:didUnublishL2CAPChannel:error:
 *
 *  @param peripheral   The peripheral manager requesting this information.
 *  @param PSM            The PSM of the channel that was published.
 *  @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion         This method is the response to a  @link unpublishL2CAPChannel: @/link call.
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral didUnpublishL2CAPChannel:(CBL2CAPPSM)PSM error:(nullable NSError *)error {
    
}

/*!
 *  @method peripheralManager:didOpenL2CAPChannel:error:
 *
 *  @param peripheral   The peripheral manager requesting this information.
 *  @param requests     A list of one or more <code>CBATTRequest</code> objects.
 *
 *  @discussion         This method is invoked when <i>peripheral</i> receives an ATT request or command for one or more characteristics with a dynamic value.
 *                      For every invocation of this method, @link respondToRequest:withResult: @/link should be called exactly once. If <i>requests</i> contains
 *                      multiple requests, they must be treated as an atomic unit. If the execution of one of the requests would cause a failure, the request
 *                      and error reason should be provided to <code>respondToRequest:withResult:</code> and none of the requests should be executed.
 *
 *  @see                CBATTRequest
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral didOpenL2CAPChannel:(nullable CBL2CAPChannel *)channel error:(nullable NSError *)error {
    
}
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSLog(@"%d command line argument%s:", argc, 1==argc ? "" : "s");
        for (const char * arg=argv[0], *argend=arg+argc; arg<argend; arg++) {
            NSLog(@"    %s", arg);
        }
        CBServer * server = [CBServer new];
        [server start];
        [[NSRunLoop currentRunLoop] run];  // go!
        [server stop];
    }
    return 0;
}

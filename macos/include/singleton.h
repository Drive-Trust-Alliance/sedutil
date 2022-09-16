//
//  singleton.h
//  SED
//
//  Created by Scott Marks on 07/05/2017.
//
//

#ifndef singleton_h
#define singleton_h

#define declareSingleton(name) +(instancetype) name

#define implementSingleton(name)                               \
declareSingleton(name) {                                       \
    static dispatch_once_t onceToken;                          \
    static id singleton = nil;                                 \
    dispatch_once(&onceToken, ^{singleton = [self new];});     \
    return singleton;                                          \
}

#endif /* singleton_h */

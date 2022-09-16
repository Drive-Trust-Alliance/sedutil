//
//  debug.h
//  SedUserClient
//
//  Created by Jackie Marks on 2/23/16.
//
//
#ifndef debug_h
#define debug_h

#if defined(DEBUG)
#define NSLOG_DEBUG NSLog

#include "Apple Cross-platform.h"
#include <objc/runtime.h>
static inline void
showMethods(NSObject * o) {
    for (Class cls=[o class]; cls!=Nil; cls=class_getSuperclass(cls)) {
        NSLOG_DEBUG(@"Methods for class %s", class_getName(cls));
        unsigned int nMethods;
        Method * methods=class_copyMethodList(cls, &nMethods);
        for (unsigned int i=0; i<nMethods ; i++ ) {
            NSLOG_DEBUG(@"%s", sel_getName(method_getName(methods[i])));
        }
        NSLOG_DEBUG(@"---------------------------\n");
    }
}
static inline void
showClasses(NSObject * o) {
    for (Class cls=[o class]; cls!=Nil; cls=class_getSuperclass(cls)) {
        if ([NSObject class]==cls || [AppleView class]==cls || [AppleWindow class]==cls || [AppleResponder class]==cls) {
            NSLOG_DEBUG(@"---------------------------\n");
            NSLOG_DEBUG(@"---stopping at class %s----\n", class_getName(cls));
            NSLOG_DEBUG(@"---------------------------\n");
            break;
        }
        NSLOG_DEBUG(@"----- Ivars for class %s", class_getName(cls));
        unsigned int nIvars;
        Ivar * ivars=class_copyIvarList(cls, &nIvars);
        for (unsigned int i=0; i<nIvars; i++ ) {
            Ivar v=ivars[i];
            const char * enc=ivar_getTypeEncoding(v);
            if ('@'==enc[0]) {
                NSLOG_DEBUG(@"%s %s %@", enc, ivar_getName(v), object_getIvar(o,v));
            } else {
                NSLOG_DEBUG(@"%s %s", enc, ivar_getName(v));
            }
        }
        NSLOG_DEBUG(@"----- Methods for class %s", class_getName(cls));
        unsigned int nMethods;
        Method * methods=class_copyMethodList(cls, &nMethods);
        for (unsigned int i=0; i<nMethods ; i++ ) {
            NSLOG_DEBUG(@"%s", sel_getName(method_getName(methods[i])));
        }
        NSLOG_DEBUG(@"---------------------------\n");
    }
}
static void
showViewRecursively(AppleView * v, NSString * indentation) {
    NSLOG_DEBUG(@"%@%@", indentation, v);
    for (AppleView * sv in v.subviews) showViewRecursively(sv, [indentation stringByAppendingString:@"  "]);
}
static inline void
showView(AppleView * v) {
    if ([v isKindOfClass:[AppleView class]])
        showViewRecursively(v,@"");
    else
        NSLOG_DEBUG(@"%@ is not an NSView", v);
}
static inline void
showWindow(AppleWindow * w) {
    if ([w isKindOfClass:[AppleWindow class]]) {
        NSLOG_DEBUG(@"%@",w);
        showClasses(w);
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        AppleViewController * cvc=w.rootViewController;
        NSLOG_DEBUG(@"---Root View Controller---");
        NSLOG_DEBUG(@"%@",cvc);
        showClasses(cvc);
#else // TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        AppleViewController * cvc=w.contentViewController;
        NSLOG_DEBUG(@"---Content View Controller---");
        NSLOG_DEBUG(@"%@",cvc);
        showClasses(cvc);
        AppleView * cv=w.contentView;
        NSLOG_DEBUG(@"---Content View---");
        showView(cv);
#endif // TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
    } else {
        NSLOG_DEBUG(@"%@ is not an NSWindow", w);
    }
}


#else


#define NSLOG_DEBUG(...) do ; while(0)

#endif


#endif /* debug_h */

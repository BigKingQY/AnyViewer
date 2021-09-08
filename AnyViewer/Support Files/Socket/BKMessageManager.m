//
//  BKMessageManager.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/7.
//

#import "BKMessageManager.h"

@implementation BKMessageManager

static BKMessageManager *_manager;
+ (BKMessageManager *)shared {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        if (!_manager) {
            _manager = [[self alloc] init];
        }
    });
    return _manager;
}

- (void)setDeletegateForTarget:(id)target {
    if (self->_delegate) {
        self->_delegate = nil;
    }
    
    self->_delegate = target;
}

@end

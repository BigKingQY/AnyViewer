//
//  NSObject+BKAdd.m
//  sz
//
//  Created by BigKing on 2021/2/23.
//  Copyright © 2021 hqht. All rights reserved.
//

#import "NSObject+BKAdd.h"

@implementation NSObject (BKAdd)

+ (void)load {
    [NSNotificationCenter.defaultCenter addObserver:self selector:@selector(appDidLaunch) name:UIApplicationDidFinishLaunchingNotification object:nil];
}

- (void)appDidLaunch {
    [NSObject isIphoneX];
    [NSNotificationCenter.defaultCenter removeObserver:self];
}

static BOOL _isPhoneX = NO;
static CGFloat _statusBarH  = 0;
static CGFloat _naviBarH    = 0;
static CGFloat _homeH       = 0;
static CGFloat _tabBarH     = 0;

+ (BOOL)isIphoneX {
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        // 利用safeAreaInsets.bottom > 0.0来判断是否是iPhone X以上设备。
        UINavigationBar *bar = [UINavigationBar new];
        [bar sizeToFit];
        _naviBarH = bar.bounds.size.height;
        
        UITabBar *tabBar = [UITabBar new];
        [tabBar sizeToFit];
        _tabBarH = tabBar.bounds.size.height;
        
        _statusBarH = 20;
        if (@available(iOS 11.0, *)) {
            UIWindow * window = [[[UIApplication sharedApplication] delegate] window];
            if (window.safeAreaInsets.bottom > 0.0) {
                _isPhoneX = YES;
                _statusBarH = window.safeAreaInsets.top;
                _homeH = window.safeAreaInsets.bottom;
                _tabBarH = _tabBarH + _homeH;
            }
        }
    });
    
    return _isPhoneX;
}

+ (CGFloat)statusBarHeight {
    if (_statusBarH == 0) {
        [self isIphoneX];
    }
    return _statusBarH;
}

+ (CGFloat)currentStatusBarHeight {
    if (@available(iOS 13, *)) {
        return UIApplication.sharedApplication.windows.firstObject.windowScene.statusBarManager.statusBarFrame.size.height;
    }
    
    return [[UIApplication sharedApplication] statusBarFrame].size.height;
}

+ (CGFloat)naviStatusBarHeight {
    return self.statusBarHeight+self.naviBarHeight;
}

+ (CGFloat)naviBarHeight {
    if (_naviBarH == 0) {
        [self isIphoneX];
    }
    return _naviBarH;
}

+ (CGFloat)homeIndecatorHeight {
    if (_statusBarH == 0) {
        [self isIphoneX];
    }
    return _homeH;
}

+ (CGFloat)tabBarHeight {
    if (_tabBarH == 0) {
        [self isIphoneX];
    }
    return _tabBarH;
}


@end

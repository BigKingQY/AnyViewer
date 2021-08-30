//
//  AppDelegate+BKAdd.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/24.
//

#import "AppDelegate+BKAdd.h"
#import "AMTabBarViewController.h"
#import <AFNetworking.h>
#import <IQKeyboardManager.h>

@implementation AppDelegate (BKAdd)

- (void)config {
    
    //配置主控制器
    [self configRootController];
    
    //配置键盘输入
    [self configIQKeboard];
    
    //配置网络监听通知
    [self configNetworkStatusNotification];
}

- (void)configRootController {
    
    AMTabBarViewController *controller = [AMTabBarViewController new];
    
    self.window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
    self.window.rootViewController = controller;
    [self.window makeKeyAndVisible];
    
}

- (void)configIQKeboard {
    
    IQKeyboardManager.sharedManager.enable = YES;
    IQKeyboardManager.sharedManager.shouldResignOnTouchOutside = YES;
    IQKeyboardManager.sharedManager.toolbarTintColor = UIColor.whiteColor;
    
}

- (void)configNetworkStatusNotification {
    
    [[AFNetworkReachabilityManager sharedManager] setReachabilityStatusChangeBlock:^(AFNetworkReachabilityStatus status) {
        
        [NSNotificationCenter.defaultCenter postNotificationName:kAFNetworkReachabilityNotification object:nil userInfo:@{@"status":@(status)}];
    }];
    
    [[AFNetworkReachabilityManager sharedManager] startMonitoring];
}

@end

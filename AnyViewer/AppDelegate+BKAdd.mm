//
//  AppDelegate+BKAdd.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/24.
//

#import "AppDelegate+BKAdd.h"
#import "AMTabBarViewController.h"
#import "AMSocketHeader.h"
#import <AFNetworking.h>
#import <IQKeyboardManager.h>
#include "CNetTransactionEngine.h"

@interface AppDelegate ()

@end

@implementation AppDelegate (BKAdd)

- (void)config {
    
    //配置主控制器
    [self configRootController];
    
    //配置socket连接
    [self configSocketConnect];
    
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

- (void)configSocketConnect {
    
    
    NSError *error;
    NSString *html = [NSString stringWithContentsOfURL:[NSURL URLWithString:@"https://ip138.com/iplookup.asp?ip=118.113.100.39&action=2"] encoding:NSUnicodeStringEncoding error:&error];
    
    NSData *data = [NSData dataWithContentsOfURL:[NSURL URLWithString:@"https://ip138.com/iplookup.asp?ip=118.113.100.39&action=2"]];
    

    //初始化日志
    CNetTransactionEngine::Instance()->InitLogWithPath(kLogPath);

    //初始化IP查询文件下载路径
    CNetTransactionEngine::Instance()->SetIPRegionPath(kIPRegionPath);
    
    //连接控制服务器
    CNetTransactionEngine::Instance()->StartConnect();
}

@end

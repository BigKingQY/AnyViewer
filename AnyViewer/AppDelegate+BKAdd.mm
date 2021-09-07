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
#import <AdSupport/AdSupport.h>
#import <SAMKeychain.h>
#import "LocalMessageBus.h"

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
    
    //初始化日志的存储路径
    GetTransactionInstance()->InitLogFilePath(kLogPath);
    
    //初始化设置的存储路径
    GetTransactionInstance()->InitSettingFilePath(kPrjSettingPath);
    
    //初始化APP版本
    GetTransactionInstance()->SetAppVersion([kAppVersion doubleValue]);
    
    //设置设备唯一ID
    GetTransactionInstance()->SetDeviceUUID([self getDeviceUUID].UTF8String);
    
    //调用消息注册函数，用来和OC数据交互
    GetLocalMessageBusInstance()->RegistMessageBus();
    
    //连接控制服务器
    GetTransactionInstance()->StartConnect();
    
    [self getDeviceUUID];
    
}


- (NSString *)getDeviceUUID {
        
    //从keychain中获取设备ID
    NSString *deviceId = [SAMKeychain passwordForService:kServiceName account:kAccountName];
    
    if (!deviceId || deviceId.length == 0) {
        //生成唯一的标识符，并存储到Keychain中
        NSString *uuid = (NSString*)CFBridgingRelease(CFUUIDCreateString(kCFAllocatorDefault, CFUUIDCreate(kCFAllocatorDefault)));
        //去掉中横线
        uuid = [uuid stringByReplacingOccurrencesOfString:@"-" withString:@""];
        //将生成的ID保存到keychain中
        [SAMKeychain setPassword:uuid forService:kServiceName account:kAccountName];
        
        //赋值
        deviceId = uuid;
    }
    
    NSLog(@"设备ID：%@", deviceId);
    
    return deviceId;
}

@end

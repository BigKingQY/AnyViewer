//
//  ConstHeader.h
//  AnyViewer
//
//  Created by BigKing on 2021/8/23.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

#ifndef ConstHeader_h
#define ConstHeader_h

//字体、图片、颜色设置
#define BKFont(font)        [UIFont systemFontOfSize:font]
#define BKImage(image)      [UIImage imageNamed:image]
#define BKColor(rgb)        [UIColor colorWithRGB:rgb]
#define BKAColor(rgb, a)    [UIColor colorWithRGB:rgb alpha:a]

// 判断是否为iPhone X 系列
#define KIS_IPHONEX          NSObject.isIphoneX
#define KHOME_INDICATOR_H    NSObject.homeIndecatorHeight
#define KSTATUS_BAR_H        NSObject.statusBarHeight // 状态栏 高度
#define KNAVIGATION_BAR_H    NSObject.naviBarHeight // 导航栏 高度
#define KSTATUS_NAVIGATION_H NSObject.naviStatusBarHeight // 状态栏＋导航栏 高度
#define KTAB_BAR_H           NSObject.tabBarHeight // 标签栏 高度

// 配置类宏定义
#define GKConfigure          [GKNavigationBarConfigure sharedInstance]

#define NSLog(format, ...) printf("[%s] %s [第%d行] %s\n", __TIME__, __FUNCTION__, __LINE__, [[NSString stringWithFormat:format, ## __VA_ARGS__] UTF8String]);

#define kLogPath [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).lastObject stringByAppendingPathComponent:@"Log"].UTF8String

#define kIPRegionPath [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).lastObject stringByAppendingPathComponent:@"TmpIPRegion.htm"].UTF8String


#define kAppVersion [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"]

#define kServiceName @"com.aomei.anyviewer.service"
#define kAccountName @"com.aomei.anyviewer.account"

//网络变化的监听通知
static NSString *kAFNetworkReachabilityNotification = @"kAFNetworkReachabilityNotification";

#endif /* ConstHeader_h */

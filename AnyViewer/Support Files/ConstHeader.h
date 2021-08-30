//
//  ConstHeader.h
//  AnyViewer
//
//  Created by BigKing on 2021/8/23.
//

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


//网络变化的监听通知
static NSString *kAFNetworkReachabilityNotification = @"kAFNetworkReachabilityNotification";

#endif /* ConstHeader_h */

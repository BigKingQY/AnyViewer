//
//  NSObject+BKAdd.h
//  sz
//
//  Created by BigKing on 2021/2/23.
//  Copyright © 2021 hqht. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSObject (BKAdd)

/// 判断是否为iPhone X
@property (class, nonatomic, assign, readonly) BOOL isIphoneX;
/// 获取状态栏的高度
@property (class, nonatomic, assign, readonly) CGFloat statusBarHeight;
/// 获取实时状态栏的高度，必须在主线程中使用
@property (class, nonatomic, assign, readonly) CGFloat currentStatusBarHeight;
/// 获取导航栏+状态栏高度
@property (class, nonatomic, assign, readonly) CGFloat naviStatusBarHeight;
/// 获取导航栏高度
@property (class, nonatomic, assign, readonly) CGFloat naviBarHeight;
/// 获取homeIndecator高度
@property (class, nonatomic, assign, readonly) CGFloat homeIndecatorHeight;
/// 获取tabbar高度
@property (class, nonatomic, assign, readonly) CGFloat tabBarHeight;

@end

NS_ASSUME_NONNULL_END

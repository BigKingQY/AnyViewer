//
//  UIViewController+Presentation.h
//  
//
//  Created by BigKing on 2019/8/26.
//  Copyright © 2019 BigKing. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class BKPresentationController;

@interface UIViewController (Presentation)

/// 增加一个属性，持有处理动画的对象
@property (nullable, nonatomic, strong) BKPresentationController *bk_presentationController;

/// 自定义从下往上弹出视图控制器
/// @param controller 控制器
/// @param animated 是否动画
- (void)bk_customDirectionalPresentViewController:(__kindof UIViewController *)controller animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss;


/// 自定义带圆角从下往上弹出视图控制器
/// @param controller 控制器
/// @param cornerRadius 圆角半径
/// @param animated 是否动画
- (void)bk_customDirectionalPresentViewController:(__kindof UIViewController *)controller cornerRadius:(CGFloat)cornerRadius animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss;


/// 自定义从屏幕中央弹出视图控制器
/// @param controller 控制器
/// @param animated 是否动画
- (void)bk_customFadedPresentViewController:(__kindof UIViewController *)controller animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss;


/// 自定义带圆角的从屏幕中央弹出视图控制器
/// @param controller 控制器
/// @param cornerRadius 圆角半径
/// @param animated 是否动画
- (void)bk_customFadedPresentViewController:(__kindof UIViewController *)controller cornerRadius:(CGFloat)cornerRadius animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss;


/// 自定义从屏幕顶部弹出视图控制器
/// @param controller 控制器
/// @param animated 是否动画
- (void)bk_customTopPresentViewController:(__kindof UIViewController *)controller animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss;


/// 自定义带圆角的从屏幕顶部弹出视图控制器
/// @param controller 控制器
/// @param cornerRadius 圆角半径
/// @param animated 是否动画
- (void)bk_customTopPresentViewController:(__kindof UIViewController *)controller cornerRadius:(CGFloat)cornerRadius animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss;


@end

NS_ASSUME_NONNULL_END

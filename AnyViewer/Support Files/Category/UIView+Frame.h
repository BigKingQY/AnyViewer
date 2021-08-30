//
//  UIView+Frame.h
//  MGWidget
//
//  Created by acmeway on 2018/3/28.
//  Copyright © 2018年 acmeway. All rights reserved.
//

#import <UIKit/UIKit.h>

//主线程函数
UIKIT_EXTERN void run_main_queue(void (^block)(void));
//全局异步线程函数
UIKIT_EXTERN void run_global_queue(void (^block)(void));
//主线程延时函数
UIKIT_EXTERN void run_main_after(float sec, void(^block)(void));

@interface UIView (Frame)

/// 圆角半径，不需要先设置Frame
@property (nonatomic, assign) CGFloat bk_cornerRadius;
/// 圆角类型
@property (nonatomic, assign) UIRectCorner bk_corner;
/// 边框颜色
@property (nonatomic, strong) UIColor *bk_borderColor;
/// 边框宽度
@property (nonatomic, assign) CGFloat bk_borderWidth;

/// 给视图添加全部圆角，需要设置视图大小
/// @param radius 圆角半径
- (void)bk_cornerRadius:(CGFloat)radius;

/// 给视图添加指定圆角，需要设置视图大小
/// @param radius 圆角半径
/// @param corner 指定圆角
- (void)bk_cornerRadius:(CGFloat)radius corner:(UIRectCorner)corner;

/// 给视图添加圆角已经边框，需要设置视图大小
/// @param radius 圆角半径
/// @param corner 指定圆角
/// @param borderRgb 边框颜色
/// @param borderWidth 边框宽度
- (void)bk_cornerRadius:(CGFloat)radius corner:(UIRectCorner)corner borderRgb:(uint32_t)borderRgb borderWidth:(CGFloat)borderWidth;

- (void)bk_cornerRadius:(CGFloat)radius corner:(UIRectCorner)corner borderColor:(UIColor *)borderColor borderWidth:(CGFloat)borderWidth;


/// 设置图片，无占位图
/// @param url 图片链接
- (void)bk_setImgaeWithUrl:(NSString *)url;


/// 设置图片，带占位图
/// @param url 图片链接
/// @param placeHolder 占位图
- (void)bk_setImgaeWithUrl:(NSString *)url placeHolder:(nullable NSString *)placeHolder;


/**
 *  起点x坐标
 */
@property (nonatomic, assign) CGFloat x;
/**
 *  起点y坐标
 */
@property (nonatomic, assign) CGFloat y;
/**
 *  中心点x坐标
 */
@property (nonatomic, assign) CGFloat centerX;
/**
 *  中心点y坐标
 */
@property (nonatomic, assign) CGFloat centerY;
/**
 *  宽度
 */
@property (nonatomic, assign) CGFloat width;
/**
 *  高度
 */
@property (nonatomic, assign) CGFloat height;
/**
 *  顶部
 */
@property (nonatomic, assign) CGFloat top;
/**
 *  底部
 */
@property (nonatomic, assign) CGFloat bottom;
/**
 *  左边
 */
@property (nonatomic, assign) CGFloat left;
/**
 *  右边
 */
@property (nonatomic, assign) CGFloat right;
/**
 *  size
 */
@property (nonatomic, assign) CGSize size;
/**
 *  起点坐标
 */
@property (nonatomic, assign) CGPoint origin;

@end

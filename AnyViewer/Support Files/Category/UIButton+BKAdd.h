//
//  UIButton+BKAdd.h
//  StandardProject
//
//  Created by BigKing on 2020/3/11.
//  Copyright © 2020 BigKing. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, BKButtonEdgeInsetsStyle) {
    BKButtonEdgeInsetsStyleTop, // image在上，label在下
    BKButtonEdgeInsetsStyleLeft, // image在左，label在右
    BKButtonEdgeInsetsStyleBottom, // image在下，label在上
    BKButtonEdgeInsetsStyleRight // image在右，label在左
};

@interface UIButton (BKAdd)

/// 设置button的titleLabel和imageView的布局样式，及间距
/// @param style titleLabel和imageView的布局样式
/// @param space titleLabel和imageView的间距
- (void)layoutButtonWithEdgeInsetsStyle:(BKButtonEdgeInsetsStyle)style
                        imageTitleSpace:(CGFloat)space;


/*
 按钮增加小菊花
 使用说明：
 1.创建indicatorView，可设置color属性调整颜色
 2.lastTitle用来设置菊花结束时的标题，lastImage同理
*/

@property (nullable, nonatomic, strong) UIActivityIndicatorView *indicatorView;
@property (nullable, nonatomic, copy) NSString *lastTitle;
@property (nullable, nonatomic, copy) UIImage *lastImage;
- (void)showIndicatorWithStyle:(UIActivityIndicatorViewStyle)style;
- (void)showIndicator;
- (void)hideIndicator;


@end

NS_ASSUME_NONNULL_END

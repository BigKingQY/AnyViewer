//
//  BKPresentationController.h
//  sz
//
//  Created by BigKing on 2021/1/11.
//  Copyright © 2021 hqht. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, BKPresentationControllerAnimationStyle) {
    BKPresentationControllerAnimationStyleFaded = 0,//渐变
    BKPresentationControllerAnimationStyleDirection,//弹出
    BKPresentationControllerAnimationStyleFromTop,  //从顶部弹出
};

@interface BKPresentationController : UIPresentationController
<UIViewControllerTransitioningDelegate>

/// 动画风格，默认Faded
@property (nonatomic, assign) BKPresentationControllerAnimationStyle animationStyle;

/// 动画时间 默认0.35s
@property (nonatomic, assign) NSTimeInterval animationDuration;

/// 是否启用毛玻璃效果，默认为NO
@property (nonatomic, assign) BOOL userBlurEffect;

/// 是否可以点击背景消失
@property (nonatomic, assign) BOOL autoDismiss;

/// 圆角
@property (nonatomic, assign) CGFloat roundCornerRadius;

@end

NS_ASSUME_NONNULL_END

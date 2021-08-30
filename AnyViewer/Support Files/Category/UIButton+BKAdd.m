//
//  UIButton+BKAdd.m
//  StandardProject
//
//  Created by BigKing on 2020/3/11.
//  Copyright © 2020 BigKing. All rights reserved.
//

#import "UIButton+BKAdd.h"
#import <objc/runtime.h>

static char *const indicatorKey = "BK_Button_Add_indicator";
static char *const lastTitleKey = "BK_Button_Add_lastTitle";
static char *const lastImageKey = "BK_Button_Add_lastImage";

@implementation UIButton (BKAdd)

- (void)layoutButtonWithEdgeInsetsStyle:(BKButtonEdgeInsetsStyle)style
                        imageTitleSpace:(CGFloat)space{
    
    
    CGFloat imageWith = self.imageView.image.size.width;
        CGFloat imageHeight = self.imageView.image.size.height;
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
        CGFloat labelWidth = [self.titleLabel.text sizeWithFont:self.titleLabel.font].width;
        CGFloat labelHeight = [self.titleLabel.text sizeWithFont:self.titleLabel.font].height;
    #pragma clang diagnostic pop
        
        CGFloat imageOffsetX = (imageWith + labelWidth) / 2 - imageWith / 2;//image中心移动的x距离
        CGFloat imageOffsetY = imageHeight / 2 + space / 2;//image中心移动的y距离
        CGFloat labelOffsetX = (imageWith + labelWidth / 2) - (imageWith + labelWidth) / 2;//label中心移动的x距离
        CGFloat labelOffsetY = labelHeight / 2 + space / 2;//label中心移动的y距离
        
        switch (style) {
            case BKButtonEdgeInsetsStyleLeft:
                self.imageEdgeInsets = UIEdgeInsetsMake(0, -space/2, 0, space/2);
                self.titleEdgeInsets = UIEdgeInsetsMake(0, space/2, 0, -space/2);
                break;
                
            case BKButtonEdgeInsetsStyleRight:
                self.imageEdgeInsets = UIEdgeInsetsMake(0, labelWidth + space/2, 0, -(labelWidth + space/2));
                self.titleEdgeInsets = UIEdgeInsetsMake(0, -(imageHeight + space/2), 0, imageHeight + space/2);
                break;
                
            case BKButtonEdgeInsetsStyleTop:
                self.imageEdgeInsets = UIEdgeInsetsMake(-imageOffsetY, imageOffsetX, imageOffsetY, -imageOffsetX);
                self.titleEdgeInsets = UIEdgeInsetsMake(labelOffsetY, -labelOffsetX, -labelOffsetY, labelOffsetX);
                break;
                
            case BKButtonEdgeInsetsStyleBottom:
                self.imageEdgeInsets = UIEdgeInsetsMake(imageOffsetY, imageOffsetX, -imageOffsetY, -imageOffsetX);
                self.titleEdgeInsets = UIEdgeInsetsMake(-labelOffsetY, -labelOffsetX, labelOffsetY, labelOffsetX);
                break;
                
            default:
                break;
        }
    
    
    
    
    
    
    // 1. 得到imageView和titleLabel的宽、高
//    CGFloat imageWith = self.imageView.frame.size.width;
//    CGFloat imageHeight = self.imageView.frame.size.height;
//
//    CGFloat labelWidth = 0.0;
//    CGFloat labelHeight = 0.0;
//    if ([UIDevice currentDevice].systemVersion.floatValue >= 8.0) {
//        // 由于iOS8中titleLabel的size为0，用下面的这种设置
//        labelWidth = self.titleLabel.intrinsicContentSize.width;
//        labelHeight = self.titleLabel.intrinsicContentSize.height;
//    } else {
//        labelWidth = self.titleLabel.frame.size.width;
//        labelHeight = self.titleLabel.frame.size.height;
//    }
//
//    // 2. 声明全局的imageEdgeInsets和labelEdgeInsets
//    UIEdgeInsets imageEdgeInsets = UIEdgeInsetsZero;
//    UIEdgeInsets labelEdgeInsets = UIEdgeInsetsZero;
//
//    // 3. 根据style和space得到imageEdgeInsets和labelEdgeInsets的值
//    switch (style) {
//        case BKButtonEdgeInsetsStyleTop:
//        {
//            imageEdgeInsets = UIEdgeInsetsMake(-labelHeight-space/2.0, 0, 0, -labelWidth);
//            labelEdgeInsets = UIEdgeInsetsMake(0, -imageWith, -imageHeight-space/2.0, 0);
//        }
//            break;
//        case BKButtonEdgeInsetsStyleLeft:
//        {
//            imageEdgeInsets = UIEdgeInsetsMake(0, -space/2.0, 0, space/2.0);
//            labelEdgeInsets = UIEdgeInsetsMake(0, space/2.0, 0, -space/2.0);
//        }
//            break;
//        case BKButtonEdgeInsetsStyleBottom:
//        {
//            imageEdgeInsets = UIEdgeInsetsMake(0, 0, -labelHeight-space/2.0, -labelWidth);
//            labelEdgeInsets = UIEdgeInsetsMake(-imageHeight-space/2.0, -imageWith, 0, 0);
//        }
//            break;
//        case BKButtonEdgeInsetsStyleRight:
//        {
//            imageEdgeInsets = UIEdgeInsetsMake(0, labelWidth+space/2.0, 0, -labelWidth-space/2.0);
//            labelEdgeInsets = UIEdgeInsetsMake(0, -imageWith-space/2.0, 0, imageWith+space/2.0);
//        }
//            break;
//        default:
//            break;
//    }
//    // 4. 赋值
//    self.titleEdgeInsets = labelEdgeInsets;
//    self.imageEdgeInsets = imageEdgeInsets;
//    [self bringSubviewToFront:self.titleLabel];
//    [self bringSubviewToFront:self.imageView];
}


- (void)setIndicatorView:(UIActivityIndicatorView *)indicatorView{
    objc_setAssociatedObject(self, &indicatorKey, indicatorView, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

- (UIActivityIndicatorView *)indicatorView{
    return (UIActivityIndicatorView *)objc_getAssociatedObject(self, &indicatorKey);
}

- (void)setLastTitle:(NSString *)lastTitle{
    objc_setAssociatedObject(self, &lastTitleKey, lastTitle, OBJC_ASSOCIATION_COPY);
}

- (NSString *)lastTitle{
    return (NSString *)objc_getAssociatedObject(self, &lastTitleKey);
}

- (void)setLastImage:(UIImage *)lastImage{
    objc_setAssociatedObject(self, &lastImageKey, lastImage, OBJC_ASSOCIATION_COPY);
}

- (UIImage *)lastImage{
    return (UIImage *)objc_getAssociatedObject(self, &lastImageKey);
}

- (void)showIndicatorWithStyle:(UIActivityIndicatorViewStyle)style{
    
    //这里判断下按钮的大小
    if (self.frame.size.width < 20.f || self.frame.size.height < 20.f) {
        if (style == UIActivityIndicatorViewStyleWhiteLarge) {
            style = UIActivityIndicatorViewStyleWhite;
        }
    }
    
    if (!self.indicatorView) {
        self.indicatorView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:style];
    }
    self.indicatorView.center = CGPointMake(self.frame.size.width/2.f, self.frame.size.height/2.f);
    self.lastTitle = self.currentTitle;
    self.lastImage = self.currentImage;
    [self setTitle:nil forState:UIControlStateNormal];
    [self setImage:nil forState:UIControlStateNormal];
    [self addSubview:self.indicatorView];
    [self.indicatorView startAnimating];
}

- (void)showIndicator{
    [self showIndicatorWithStyle:UIActivityIndicatorViewStyleGray];
}

- (void)hideIndicator{
    [self.indicatorView stopAnimating];
    [self.indicatorView removeFromSuperview];
    [self setTitle:self.lastTitle forState:UIControlStateNormal];
    [self setImage:self.lastImage forState:UIControlStateNormal];
}


- (void)setImageName:(NSString *)imageName withAnimated:(BOOL)animated {
    
    UIImage *icon = [UIImage imageNamed:imageName];
    [self setImage:icon forState:UIControlStateNormal];
    
    if (animated) {
        
        [UIView animateWithDuration:0.2f delay:0.f usingSpringWithDamping:0.9f initialSpringVelocity:10.f options:UIViewAnimationOptionCurveEaseInOut | UIViewAnimationOptionBeginFromCurrentState animations:^{
            
            self.transform = CGAffineTransformMakeScale(1.2f, 1.2f);
            
        } completion:^(BOOL finished) {

            [UIView animateWithDuration:0.2f delay:0.f usingSpringWithDamping:0.9f initialSpringVelocity:10.f options:UIViewAnimationOptionCurveEaseInOut | UIViewAnimationOptionBeginFromCurrentState animations:^{
                
                self.transform = CGAffineTransformIdentity;
                
            } completion:nil];
            
        }];
    }
}


@end


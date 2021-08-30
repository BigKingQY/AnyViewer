//
//  UIView+ViewController.h
//  yougc
//
//  Created by apple on 2017/5/11.
//  Copyright © 2017年 wendy. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIView (ViewController)

/**
 找到view的根控制器

 @return 返回的是view所在的根控制器
 */
- (UIViewController *)viewController;

@end

//
//  UIView+ViewController.m
//  yougc
//
//  Created by apple on 2017/5/11.
//  Copyright © 2017年 wendy. All rights reserved.
//


#import "UIView+ViewController.h"

@implementation UIView (ViewController)

- (UIViewController *)viewController
{
    UIResponder *next = self.nextResponder;
    
    do {
        if ([next isKindOfClass:[UIViewController class]]) {
            return (UIViewController *)next;
        }
        
        next = next.nextResponder;
        
    } while (next);
    
    return nil;
    
}


@end

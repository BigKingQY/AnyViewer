//
//  BaseNavigationViewController.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import "BaseNavigationViewController.h"

@interface BaseNavigationViewController ()

@end

@implementation BaseNavigationViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.navigationItem.backBarButtonItem = [[UIBarButtonItem alloc] initWithImage:nil style:UIBarButtonItemStylePlain target:nil action:nil];

    [self.navigationBar setTitleTextAttributes:[NSDictionary regularFontSize:17 color:0x000000]];
}

#pragma make ----------  重写push方法
- (void)pushViewController:(UIViewController *)viewController animated:(BOOL)animated{
    if (self.viewControllers.count > 0) {
            //只有第一个viewController push的时候设置隐藏
            if (self.viewControllers.count == 1) {
                viewController.hidesBottomBarWhenPushed = YES;
            }
        } else {
            viewController.hidesBottomBarWhenPushed = NO;
        }
    [super pushViewController:viewController animated:animated];
}


@end

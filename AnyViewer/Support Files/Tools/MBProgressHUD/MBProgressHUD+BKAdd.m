//
//  MBProgressHUD+BKAdd.m
//  BKStandProject
//
//  Created by BigKing on 2021/3/3.
//

#import "MBProgressHUD+BKAdd.h"

@implementation MBProgressHUD (BKAdd)

+ (void)showLoading:(NSString *)text onView:(UIView *)view {
    MBProgressHUD *hud = [self createMBProgressHUDviewWithMessage:text onView:view];
    if (!hud) {
        return;
    }
    hud.mode = MBProgressHUDModeIndeterminate;
    hud.square = YES;
    [hud showAnimated:YES];
}

+ (void)showSuccess:(NSString *)text onView:(UIView *)view {
    MBProgressHUD *hud = [self createMBProgressHUDviewWithMessage:text onView:view];
    if (!hud) {
        return;
    }
    hud.mode = MBProgressHUDModeCustomView;
    UIImageView *imageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"icon_mb_success"]];
    imageView.contentMode = UIViewContentModeScaleAspectFit;
    hud.square = YES;
    hud.customView = imageView;
    hud.minSize = CGSizeMake(80, 80.f);
    [hud showAnimated:YES];
    [hud hideAnimated:YES afterDelay:1.5f];
}

+ (void)showMessage:(NSString *)text onView:(UIView *)view {
    MBProgressHUD *hud = [self createMBProgressHUDviewWithMessage:text onView:view];
    if (!hud) {
        return;
    }
    hud.mode = MBProgressHUDModeText;
    hud.margin = 10.f;
    hud.square = NO;
    [hud showAnimated:YES];
    [hud hideAnimated:YES afterDelay:1.5f];
}

+ (void)hideForView:(UIView *)view {
    MBProgressHUD *viewHUD = [MBProgressHUD HUDForView:view];
    if (viewHUD) {
        [viewHUD hideAnimated:YES];
    }
}

+ (void)showLoadingOnWindow:(NSString *)text {
    [self showLoading:text onView:UIApplication.sharedApplication.keyWindow];
}

+ (void)showSuccessOnWindow:(NSString *)text {
    [self showSuccess:text onView:UIApplication.sharedApplication.keyWindow];
}

+ (void)showMessageOnWindow:(NSString *)text {
    [self showMessage:text onView:UIApplication.sharedApplication.keyWindow];
}

+ (void)hideForWindow {
    MBProgressHUD *windowHUD = [MBProgressHUD HUDForView:UIApplication.sharedApplication.keyWindow];
    if (windowHUD) {
        [windowHUD hideAnimated:YES];
    }
}

+ (void)hideForViewAndWindow {
    [self hideForView:[self rootView]];
    [self hideForWindow];
}

+ (UIView *)rootView {
    UIViewController *topController = [UIApplication sharedApplication].keyWindow.rootViewController;
    UIViewController *vc = [topController gk_visibleViewControllerIfExist];
    return vc.view;
}

+ (MBProgressHUD*)createMBProgressHUDviewWithMessage:(NSString*)message onView:(UIView *)view {
    
    if (!view) {
        return nil;
    }
    
    MBProgressHUD *hud = [MBProgressHUD HUDForView:view];
    if (!hud) {
        hud = [MBProgressHUD showHUDAddedTo:view animated:YES];
    }
    
    hud.animationType = MBProgressHUDAnimationFade;
    hud.bezelView.style = MBProgressHUDBackgroundStyleSolidColor;
    hud.bezelView.backgroundColor = [UIColor colorWithWhite:0 alpha:0.8];
    hud.label.text = message;
    hud.label.font = [UIFont systemFontOfSize:16];
    hud.label.textColor = UIColor.whiteColor;
    hud.label.numberOfLines = 0;
    hud.removeFromSuperViewOnHide = YES;
    hud.customView.hidden = NO;
    hud.contentColor = UIColor.whiteColor;
    
    return hud;
}

@end

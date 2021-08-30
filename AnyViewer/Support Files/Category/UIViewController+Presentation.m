//
//  UIViewController+Presentation.m
//
//
//  Created by BigKing on 2019/8/26.
//  Copyright © 2019 BigKing. All rights reserved.
//

#import "UIViewController+Presentation.h"
#import "BKPresentationController.h"
#import <objc/runtime.h>

@implementation UIViewController (Presentation)

@dynamic bk_presentationController;

- (void)bk_customDirectionalPresentViewController:(__kindof UIViewController *)controller animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss {
    [self bk_customDirectionalPresentViewController:controller cornerRadius:0.f animated:animated autoDismiss:autoDismiss];
}

- (void)bk_customDirectionalPresentViewController:(__kindof UIViewController *)controller cornerRadius:(CGFloat)cornerRadius animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss {
    
    BKPresentationController *presentationController = [[BKPresentationController alloc] initWithPresentedViewController:controller presentingViewController:self];
    
    presentationController.animationStyle = BKPresentationControllerAnimationStyleDirection;
    presentationController.userBlurEffect = NO;
    presentationController.roundCornerRadius = cornerRadius;
    presentationController.autoDismiss = autoDismiss;
    
    [self bk_customPresentViewWithPresentationController:presentationController presentedViewController:controller animated:animated];
}

- (void)bk_customFadedPresentViewController:(__kindof UIViewController *)controller animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss {
    
    BKPresentationController *presentationController = [[BKPresentationController alloc] initWithPresentedViewController:controller presentingViewController:self];
    
    presentationController.animationStyle = BKPresentationControllerAnimationStyleFaded;
    presentationController.userBlurEffect = NO;
    presentationController.autoDismiss = autoDismiss;
    
    [self bk_customPresentViewWithPresentationController:presentationController presentedViewController:controller animated:animated];
    
}

- (void)bk_customFadedPresentViewController:(__kindof UIViewController *)controller cornerRadius:(CGFloat)cornerRadius animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss {
    
    BKPresentationController *presentationController = [[BKPresentationController alloc] initWithPresentedViewController:controller presentingViewController:self];
    presentationController.animationStyle = BKPresentationControllerAnimationStyleFaded;
    presentationController.userBlurEffect = NO;
    presentationController.roundCornerRadius = cornerRadius;
    presentationController.autoDismiss = autoDismiss;
    
    [self bk_customPresentViewWithPresentationController:presentationController presentedViewController:controller animated:animated];
}

- (void)bk_customTopPresentViewController:(__kindof UIViewController *)controller animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss {
    
    BKPresentationController *presentationController = [[BKPresentationController alloc] initWithPresentedViewController:controller presentingViewController:self];
    
    presentationController.animationStyle = BKPresentationControllerAnimationStyleFromTop;
    presentationController.userBlurEffect = NO;
    presentationController.autoDismiss = autoDismiss;
    
    [self bk_customPresentViewWithPresentationController:presentationController presentedViewController:controller animated:animated];
}

- (void)bk_customTopPresentViewController:(__kindof UIViewController *)controller cornerRadius:(CGFloat)cornerRadius animated:(BOOL)animated autoDismiss:(BOOL)autoDismiss {
    
    BKPresentationController *presentationController = [[BKPresentationController alloc] initWithPresentedViewController:controller presentingViewController:self];
    presentationController.animationStyle = BKPresentationControllerAnimationStyleFromTop;
    presentationController.userBlurEffect = NO;
    presentationController.roundCornerRadius = cornerRadius;
    presentationController.autoDismiss = autoDismiss;
    
    [self bk_customPresentViewWithPresentationController:presentationController presentedViewController:controller animated:animated];
}

- (void)bk_customPresentViewWithPresentationController:(BKPresentationController *)presentationController presentedViewController:(__kindof UIViewController *)controller animated:(BOOL)animated{
    
    self.bk_presentationController = presentationController;
    controller.modalPresentationStyle = UIModalPresentationCustom;
    controller.transitioningDelegate = presentationController;
    [self presentViewController:controller animated:animated completion:^{
        ///动画执行完毕将对象置空
        self.bk_presentationController = nil;
    }];
}

- (UIPresentationController *)presentationController {
    return objc_getAssociatedObject(self, @"bk_presentationController");
}

- (void)setBk_presentationController:(BKPresentationController *)bk_presentationController {
    objc_setAssociatedObject(self, @"bk_presentationController", bk_presentationController, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

@end


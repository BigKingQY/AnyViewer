//
//  BKPresentationController.m
//  sz
//
//  Created by BigKing on 2021/1/11.
//  Copyright © 2021 hqht. All rights reserved.
//

#import "BKPresentationController.h"

//MARK: BlurView

//添加毛玻璃效果

@interface BlurView : UIVisualEffectView

@end

@implementation BlurView

- (instancetype)initWithFrame:(CGRect)frame{
    self = [super initWithFrame:frame];
    
    if (self) {
        
        self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        UIBlurEffectStyle style = UIBlurEffectStyleExtraLight;
        UIBlurEffect *eff = [UIBlurEffect effectWithStyle:style];
        self.effect = eff;
        
    }
    return self;
}

@end

//MARK: DimmingView

@interface DimmingView : UIControl

@end

@implementation DimmingView

- (instancetype)initWithFrame:(CGRect)frame{
    self = [super initWithFrame:frame];
    if (self) {
        self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        self.backgroundColor = [UIColor colorWithWhite:0 alpha:0.47];
    }
    return self;
}

@end

//MARK: WrapperView

@interface WrapperView : UIView

@end

@implementation WrapperView

- (instancetype)initWithFrame:(CGRect)frame{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor clearColor];
        self.layer.masksToBounds = YES;
    }
    return self;
}

@end

@interface BKPresentationController ()<UIViewControllerAnimatedTransitioning>

@property (nonatomic, strong) DimmingView *dimmingView;
@property (nonatomic, strong) WrapperView *wrapperView;
@property (nonatomic, strong) BlurView *blurView;

@end

@implementation BKPresentationController

- (instancetype)initWithPresentedViewController:(UIViewController *)presentedViewController presentingViewController:(UIViewController *)presentingViewController{
    self = [super initWithPresentedViewController:presentedViewController presentingViewController:presentingViewController];
    if (self) {
        presentedViewController.modalPresentationStyle = UIModalPresentationCustom;
        
        _animationStyle = BKPresentationControllerAnimationStyleFaded;
        _animationDuration = 0.35f;
        _userBlurEffect = NO;
    }
    return self;
}

- (void)setAnimationStyle:(BKPresentationControllerAnimationStyle)animationStyle{
    _animationStyle = animationStyle;
}

- (void)setAnimationDuration:(NSTimeInterval)animationDuration{
    _animationDuration = animationDuration;
}

- (void)setUserBlurEffect:(BOOL)userBlurEffect{
    _userBlurEffect = userBlurEffect;
}

- (UIView *)presentedView {
    /**
     这个东西很神奇~ 返回了被弹出来控制器的视图，如果重写了的话就是依照这个返回值为准
     所以在begin方法中调用了[super presentedView]，而不是[self presentedView];
     */
    return self.wrapperView;
}

- (void)presentationTransitionWillBegin {

    //presented view controller's view
    UIView *presentedView = super.presentedView;
    presentedView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    //dimming view

    DimmingView *dimmingView = [[DimmingView alloc] initWithFrame:self.containerView.bounds];
    dimmingView.alpha = 0.f;

    if (self.autoDismiss) {
        [dimmingView addTarget:self action:@selector(dismissController) forControlEvents:UIControlEventTouchUpInside];
    }

    [self.containerView addSubview:dimmingView];
    self.dimmingView = dimmingView;

    if (_userBlurEffect) {
        BlurView *blurView = [[BlurView alloc] initWithFrame:self.containerView.bounds];
        [self.containerView addSubview:blurView];
        self.blurView = blurView;
        dimmingView.backgroundColor = [UIColor clearColor];
    }

    WrapperView *wrapperView = [[WrapperView alloc] initWithFrame:self.frameOfPresentedViewInContainerView];
    presentedView.frame = wrapperView.bounds;
    [wrapperView addSubview:presentedView];
    wrapperView.layer.cornerRadius = self.roundCornerRadius;

    if (_userBlurEffect) {
        wrapperView.backgroundColor = [UIColor clearColor];
    }

    self.wrapperView = wrapperView;

    self.blurView.effect = nil;
    [self.presentedViewController.transitionCoordinator animateAlongsideTransition:^(id<UIViewControllerTransitionCoordinatorContext>  _Nonnull context) {

        self.blurView.effect = [UIBlurEffect effectWithStyle:UIBlurEffectStyleExtraLight];
        self.dimmingView.alpha = 1.f;

    } completion:NULL];

}

- (void)dismissController{
    [self.presentedViewController dismissViewControllerAnimated:YES completion:NULL];
}


- (void)presentationTransitionDidEnd:(BOOL)completed{

    if (completed == NO) {
        self.dimmingView = nil;
        self.wrapperView = nil;
        if (self.blurView) {
            self.blurView = nil;
        }
    }

}

- (void)dismissalTransitionWillBegin{

    [self.presentingViewController.transitionCoordinator animateAlongsideTransition:^(id<UIViewControllerTransitionCoordinatorContext>  _Nonnull context) {

        self.dimmingView.alpha = 0.f;
        self.blurView.effect = nil;

    } completion:NULL];
}



- (void)dismissalTransitionDidEnd:(BOOL)completed{

    if (completed) {

        self.dimmingView = nil;
        self.wrapperView = nil;
        if (self.blurView) {
            self.blurView = nil;
        }
    }
}

- (void)containerViewWillLayoutSubviews{
    [super containerViewWillLayoutSubviews];

    self.dimmingView.frame = self.containerView.bounds;
    self.wrapperView.frame = self.frameOfPresentedViewInContainerView;
}

- (void)preferredContentSizeDidChangeForChildContentContainer:(id<UIContentContainer>)container{
    [super preferredContentSizeDidChangeForChildContentContainer:container];

    if (container == self.presentedViewController) {
        [self.containerView setNeedsLayout];
    }
}

- (CGSize)sizeForChildContentContainer:(id<UIContentContainer>)container withParentContainerSize:(CGSize)parentSize{

    CGSize presentedViewPreferSize = ((UIViewController *)container).preferredContentSize;
    CGSize superPreferSize = [super sizeForChildContentContainer:container withParentContainerSize:parentSize];

    if (presentedViewPreferSize.width == 0) {
        presentedViewPreferSize.width = superPreferSize.width;
    }

    if (presentedViewPreferSize.height == 0) {
        presentedViewPreferSize.height = superPreferSize.height;
    }

    if (container == self.presentedViewController) {

        return presentedViewPreferSize;

    }else{
        return superPreferSize;
    }

}

- (CGRect)frameOfPresentedViewInContainerView{

    CGRect containerBounds = self.containerView.bounds;

    //这里在[self sizeForChild]方法里面对size做了大小的处理
    CGSize presentedViewSize = [self sizeForChildContentContainer:self.presentedViewController withParentContainerSize:containerBounds.size];

    CGRect presentedViewControllerFrame = CGRectMake(0, 0, presentedViewSize.width, presentedViewSize.height);

    presentedViewControllerFrame.origin.x = (containerBounds.size.width - presentedViewSize.width) / 2.f;
    
    if (_animationStyle == BKPresentationControllerAnimationStyleFaded) {

        presentedViewControllerFrame.origin.y = (containerBounds.size.height - presentedViewSize.height)/2;
        presentedViewControllerFrame.size.height = presentedViewSize.height;

    }else if (_animationStyle == BKPresentationControllerAnimationStyleDirection) {
        presentedViewControllerFrame.origin.y = containerBounds.size.height - presentedViewSize.height;
        presentedViewControllerFrame.size.height = presentedViewSize.height;
    }else {
        presentedViewControllerFrame.origin.y = 0;
        presentedViewControllerFrame.size.height = presentedViewSize.height;
    }

    return presentedViewControllerFrame;
}


#pragma mark UIViewControllerAnimatedTransitioning

- (NSTimeInterval)transitionDuration:(id<UIViewControllerContextTransitioning>)transitionContext{
    
    return [transitionContext isAnimated] ? self.animationDuration : 0;
}


- (void)animateTransition:(id<UIViewControllerContextTransitioning>)transitionContext{
    
    UIViewController *fromViewController = [transitionContext viewControllerForKey:UITransitionContextFromViewControllerKey];
    UIViewController *toViewController = [transitionContext viewControllerForKey:UITransitionContextToViewControllerKey];
    
    UIView *fromView = [transitionContext viewForKey:UITransitionContextFromViewKey];
    UIView *toView = [transitionContext viewForKey:UITransitionContextToViewKey];
    
    CGRect fromViewFinalFrame   = [transitionContext finalFrameForViewController:fromViewController];
    
    CGRect toViewInitialFrame   = [transitionContext initialFrameForViewController:toViewController];
    CGRect toViewFinalFrame     = [transitionContext finalFrameForViewController:toViewController];
    
    
    UIView *containerView = transitionContext.containerView;
    [containerView addSubview:toView];
    
    BOOL isPresenting = (fromViewController == self.presentingViewController);
    
    NSTimeInterval animationDuration = [self transitionDuration:transitionContext];
    
    [containerView addSubview:toView];
    
    if (self.animationStyle == BKPresentationControllerAnimationStyleFaded) {
        
        toView.alpha = 0.f;
        toView.frame = toViewFinalFrame;
        
        if (isPresenting) {
            
            [UIView animateWithDuration:animationDuration delay:0 usingSpringWithDamping:1.f initialSpringVelocity:2.f options:UIViewAnimationOptionCurveEaseInOut | UIViewAnimationOptionBeginFromCurrentState animations:^{
                
                toView.alpha = 1.f;
                
            } completion:^(BOOL finished) {
                
                BOOL wasCancelled = [transitionContext transitionWasCancelled];
                [transitionContext completeTransition:!wasCancelled];
                
            }];
            
        }else{
            
            [UIView animateWithDuration:animationDuration delay:0 usingSpringWithDamping:1.f initialSpringVelocity:2.f options:UIViewAnimationOptionCurveEaseInOut | UIViewAnimationOptionBeginFromCurrentState animations:^{
                
                fromView.alpha = 0.f;
                
            } completion:^(BOOL finished) {
                
                BOOL wasCancelled = [transitionContext transitionWasCancelled];
                [transitionContext completeTransition:!wasCancelled];
                
            }];
        }
    
    }else if (self.animationStyle == BKPresentationControllerAnimationStyleDirection) {
        
        if (isPresenting) {
            
            CGFloat pointY = CGRectGetMaxY(containerView.bounds);
            
            toViewInitialFrame.origin = CGPointMake(CGRectGetMinX(containerView.bounds), pointY);
            toViewInitialFrame.size = toViewFinalFrame.size;
            toView.frame = toViewInitialFrame;
            
        } else {
            
            CGFloat offsetY = CGRectGetHeight(fromView.frame);
            fromViewFinalFrame = CGRectOffset(fromView.frame, 0, offsetY);
        }
        
        NSTimeInterval transitionDuration = [self transitionDuration:transitionContext];
        
        [UIView animateWithDuration:transitionDuration animations:^{
            if (isPresenting)
                toView.frame = toViewFinalFrame;
            else
                fromView.frame = fromViewFinalFrame;
            
        } completion:^(BOOL finished) {
            
            BOOL wasCancelled = [transitionContext transitionWasCancelled];
            [transitionContext completeTransition:!wasCancelled];
        }];
        
    }else if (self.animationStyle == BKPresentationControllerAnimationStyleFromTop) {
        
        if (isPresenting) {

            toViewInitialFrame.origin = CGPointMake(CGRectGetMinX(containerView.bounds), 0);
            toViewInitialFrame.size = toViewFinalFrame.size;
            toView.frame = CGRectMake(0, -toViewFinalFrame.size.height, toViewFinalFrame.size.width, toViewFinalFrame.size.height);
            
        } else {
            
            CGFloat offsetY = CGRectGetHeight(fromView.frame);
            fromViewFinalFrame = CGRectOffset(fromView.frame, 0, -offsetY);
        }
        
        NSTimeInterval transitionDuration = [self transitionDuration:transitionContext];
        
        [UIView animateWithDuration:transitionDuration animations:^{
            if (isPresenting)
                toView.frame = toViewFinalFrame;
            else
                fromView.frame = fromViewFinalFrame;
            
        } completion:^(BOOL finished) {
            
            BOOL wasCancelled = [transitionContext transitionWasCancelled];
            [transitionContext completeTransition:!wasCancelled];
        }];
    }
    
}

- (UIPresentationController*)presentationControllerForPresentedViewController:(UIViewController *)presented presentingViewController:(UIViewController *)presenting sourceViewController:(UIViewController *)source{
    NSAssert(self.presentedViewController == presented, @"You didn't initialize %@ with the correct presentedViewController.  Expected %@, got %@.",
             self, presented, self.presentedViewController);
    
    return self;
}

- (id<UIViewControllerAnimatedTransitioning>)animationControllerForPresentedController:(UIViewController *)presented presentingController:(UIViewController *)presenting sourceController:(UIViewController *)source{
    return self;
}

- (id<UIViewControllerAnimatedTransitioning>)animationControllerForDismissedController:(UIViewController *)dismissed{
    return self;
}

@end

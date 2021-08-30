//
//  BKFingerWaveView.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/25.
//

#import "BKFingerWaveView.h"


@interface BKFingerWaveView () <CAAnimationDelegate>

{
    CGSize waveSize;            //波纹大小
    NSTimeInterval duration;    //动画时长
}

@end


@implementation BKFingerWaveView

- (instancetype)initWithFrame:(CGRect)frame {
    
    if (self = [super initWithFrame:frame]) {
        waveSize = CGSizeMake(100, 100);
        duration = 0.3;
    }
    return self;
}

+ (instancetype)showInView:(UIView *)view center:(CGPoint)center {
    
    BKFingerWaveView *waveView = [BKFingerWaveView new];
    [waveView setframeWithCenter:center];
    [view addSubview:waveView];
    return waveView;
}

- (void)didMoveToSuperview{
    
    CAShapeLayer *waveLayer = [CAShapeLayer new];
    waveLayer.backgroundColor = [UIColor clearColor].CGColor;
    waveLayer.opacity = 0.6;
    waveLayer.fillColor = [UIColor whiteColor].CGColor;
    [self.layer addSublayer:waveLayer];
    
    [self startAnimationInLayer:waveLayer];
}

- (void)startAnimationInLayer:(CALayer *)layer {
    
    UIBezierPath *beginPath = [UIBezierPath bezierPathWithArcCenter:[self pathCenter] radius:[self animationBeginRadius] startAngle:0 endAngle:M_PI*2 clockwise:YES];
    UIBezierPath *endPath = [UIBezierPath bezierPathWithArcCenter:[self pathCenter] radius:[self animationEndRadius] startAngle:0 endAngle:M_PI*2 clockwise:YES];
    
    CABasicAnimation *rippleAnimation = [CABasicAnimation animationWithKeyPath:@"path"];
    rippleAnimation.delegate = self;
    rippleAnimation.fromValue = (__bridge id _Nullable)(beginPath.CGPath);
    rippleAnimation.toValue = (__bridge id _Nullable)(endPath.CGPath);
    rippleAnimation.duration = duration;
    
    CABasicAnimation *opacityAnimation = [CABasicAnimation animationWithKeyPath:@"opacity"];
    opacityAnimation.delegate = self;
    opacityAnimation.fromValue = [NSNumber numberWithFloat:0.6];
    opacityAnimation.toValue = [NSNumber numberWithFloat:0.0];
    opacityAnimation.duration = duration;
    
    [layer addAnimation:rippleAnimation forKey:@"rippleAnimation"];
    [layer addAnimation:opacityAnimation forKey:@"opacityAnimation"];
}

- (void)setframeWithCenter:(CGPoint)center{
    CGRect frame = CGRectMake(center.x-waveSize.width*0.5, center.y-waveSize.height*0.5, waveSize.width, waveSize.height);
    self.frame = frame;;
}

- (CGFloat)animationBeginRadius{
    return waveSize.width*0.5*0.2;
}

- (CGFloat)animationEndRadius{
    return waveSize.width*0.5;
}

- (CGPoint)pathCenter{
    return CGPointMake(waveSize.width*0.5, waveSize.height*0.5);
}

#pragma mark - CAAnimationDelegate
- (void)animationDidStop:(CAAnimation *)anim finished:(BOOL)flag{
    if (flag) {
        [self removeFromSuperview];
    }
}

@end

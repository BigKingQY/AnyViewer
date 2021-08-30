//
//  UIView+Frame.m
//  MGWidget
//
//  Created by acmeway on 2018/3/28.
//  Copyright © 2018年 acmeway. All rights reserved.
//

#import "UIView+Frame.h"
#import <objc/runtime.h>

dispatch_queue_t get_global_queue(void) {
    return dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
}

void run_main_queue(void (^block)(void)) {
    dispatch_async(dispatch_get_main_queue(), block);
}

void run_global_queue(void (^block)(void)) {
    dispatch_async(get_global_queue(), block);
}

void run_main_after(float sec, void(^block)(void)) {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(sec * NSEC_PER_SEC)), dispatch_get_main_queue(), block);
}

@implementation UIView (Frame)

static const char* kBKCornerRadiusKey = "kBKCornerRadiusKey";
static const char* kBKCornerKey = "kBKCornerKey";
static const char* kBKBorderColorKey = "kBKBorderColorKey";
static const char* kBKBorderWidthKey = "kBKBorderWidthKey";

- (void)setBk_cornerRadius:(CGFloat)bk_cornerRadius {
    objc_setAssociatedObject(self, kBKCornerRadiusKey, @(bk_cornerRadius), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self setNeedsDisplay];
}

- (CGFloat)bk_cornerRadius {
    NSNumber *radius = objc_getAssociatedObject(self, kBKCornerRadiusKey);
    if (!radius) {
        radius = 0;
    }
    return [radius floatValue];
}

- (void)setBk_corner:(UIRectCorner)bk_corner {
    objc_setAssociatedObject(self, kBKCornerKey, @(bk_corner), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self setNeedsDisplay];
}

- (UIRectCorner)bk_corner {
    NSNumber *corner = objc_getAssociatedObject(self, kBKCornerKey);
    if (!corner) {
        return UIRectCornerAllCorners;
    }
    return [corner integerValue];
}

- (void)setBk_borderColor:(UIColor *)bk_borderColor {
    objc_setAssociatedObject(self, kBKBorderColorKey, bk_borderColor, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self setNeedsDisplay];
}

- (UIColor *)bk_borderColor {
    UIColor *color = objc_getAssociatedObject(self, kBKBorderColorKey);
    if (!color) {
        color = UIColor.clearColor;
    }
    return color;
}

- (void)setBk_borderWidth:(CGFloat)bk_borderWidth {
    objc_setAssociatedObject(self, kBKBorderWidthKey, @(bk_borderWidth), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self setNeedsDisplay];
}

- (CGFloat)bk_borderWidth {
    NSNumber *width = objc_getAssociatedObject(self, kBKBorderWidthKey);
    if (!width) {
        width = @0;
    }
    return [width floatValue];
}

- (void)bk_cornerRadius:(CGFloat)radius {
    [self bk_cornerRadius:radius corner:UIRectCornerAllCorners];
}

- (void)bk_cornerRadius:(CGFloat)radius corner:(UIRectCorner)corner {
    [self bk_cornerRadius:radius corner:corner borderColor:0x0 borderWidth:1];
}

- (void)bk_cornerRadius:(CGFloat)radius corner:(UIRectCorner)corner borderRgb:(uint32_t)borderRgb borderWidth:(CGFloat)borderWidth {
    [self bk_cornerRadius:radius corner:corner borderColor:BKColor(borderRgb) borderWidth:borderWidth];
}

- (void)bk_cornerRadius:(CGFloat)radius corner:(UIRectCorner)corner borderColor:(UIColor *)borderColor borderWidth:(CGFloat)borderWidth {
    NSAssert(!CGRectEqualToRect(self.bounds, CGRectZero), @"当前View未设置大小，切圆角失败");
    if (CGRectEqualToRect(self.bounds, CGRectZero) || radius == 0) {
        return;
    }
    
    for (int i = 0; i < self.layer.sublayers.count; i++) {
        CALayer *layer = self.layer.sublayers[i];
        if ([layer isKindOfClass:[CAShapeLayer class]]) {
            [layer removeFromSuperlayer];
        }
    }
    
    UIBezierPath *path = [UIBezierPath bezierPathWithRoundedRect:self.bounds byRoundingCorners:corner cornerRadii:CGSizeMake(radius, radius)];
    path.lineJoinStyle = kCGLineJoinRound;
    
    CAShapeLayer *maskLayer = [CAShapeLayer new];
    maskLayer.frame = self.bounds;
    maskLayer.path = path.CGPath;
    self.layer.mask = maskLayer;
    
    if (borderWidth > 0) {
        CAShapeLayer *borderLayer = [CAShapeLayer new];
        borderLayer.frame = self.bounds;
        borderLayer.path = path.CGPath;
        borderLayer.lineWidth = borderWidth;
        borderLayer.strokeColor = borderColor.CGColor;
        borderLayer.fillColor = UIColor.clearColor.CGColor;
        [self.layer addSublayer:borderLayer];        
    }
}


//- (void)bk_setImgaeWithUrl:(NSString *)url {
//    [self bk_setImgaeWithUrl:url placeHolder:nil];
//}
//
//- (void)bk_setImgaeWithUrl:(NSString *)url placeHolder:(nullable NSString *)placeHolder {
//    [self.layer setImageWithURL:[NSURL URLWithString:[self convertImageUrl:url]] placeholder:[UIImage imageNamed:placeHolder]];
//}
//
//- (NSString *)convertImageUrl:(NSString *)imageUrl {
//    if ([imageUrl hasPrefix:@"http"]) {
//        return imageUrl;
//    }
//    return [NSString stringWithFormat:@"%@%@", JZMUserManager.shared.baseImageUrl, imageUrl];
//}



- (void)setX:(CGFloat)x
{
    CGRect frame   = self.frame;
    
    frame.origin.x = x;
    
    self.frame     = frame;
}

- (void)setY:(CGFloat)y
{
    CGRect frame   = self.frame;
    if (isnan(y)) {
        y = 0;
    }
    frame.origin.y = y;
    
    self.frame     = frame;
}

- (CGFloat)x
{
    return self.frame.origin.x;
}

- (CGFloat)y
{
    return self.frame.origin.y;
}

- (void)setCenterX:(CGFloat)centerX
{
    CGPoint center = self.center;
    
    center.x       = centerX;
    
    self.center    = center;

}

- (CGFloat)centerX
{
    return self.center.x;
}

- (void)setCenterY:(CGFloat)centerY
{
    CGPoint center = self.center;

    center.y       = centerY;

    self.center    = center;
}

- (CGFloat)centerY
{
    return self.center.y;
}

- (void)setWidth:(CGFloat)width
{
    CGRect frame     = self.frame;
    
    frame.size.width = width;
    
    self.frame       = frame;
}

- (void)setHeight:(CGFloat)height
{
    CGRect frame      = self.frame;
    if (isnan(height)) {
        height = 0;
    }
    frame.size.height = height;
    
    self.frame        = frame;
}

- (CGFloat)height
{
    return self.frame.size.height;
}

- (CGFloat)width
{
    return self.frame.size.width;
}

- (void)setSize:(CGSize)size
{
    CGRect frame = self.frame;
    
    frame.size   = size;
    
    self.frame   = frame;
}

- (CGSize)size
{
    return self.frame.size;
}

- (void)setOrigin:(CGPoint)origin
{
    CGRect frame = self.frame;
    
    frame.origin = origin;
    
    self.frame   = frame;
}

- (CGPoint)origin
{
    return self.frame.origin;
}
- (CGFloat)top
{
    return self.frame.origin.y;
}

- (void)setTop:(CGFloat)top
{
    CGRect frame   = self.frame;
    
    frame.origin.y = top;
    
    self.frame     = frame;
}

- (CGFloat)left
{
    return self.frame.origin.x;
}

- (void)setLeft:(CGFloat)left
{
    CGRect frame   = self.frame;
    
    frame.origin.x = left;
    
    self.frame     = frame;
}


- (CGFloat)bottom
{
    return self.frame.size.height + self.frame.origin.y;
}

- (void)setBottom:(CGFloat)bottom
{
    CGRect frame   = self.frame;
    
    frame.origin.y = bottom - frame.size.height;
    
    self.frame     = frame;
}

- (CGFloat)right
{
    return self.frame.size.width + self.frame.origin.x;
}

- (void)setRight:(CGFloat)right
{
    CGRect frame   = self.frame;
    
    frame.origin.x = right - frame.size.width;
    
    self.frame     = frame;
}

@end

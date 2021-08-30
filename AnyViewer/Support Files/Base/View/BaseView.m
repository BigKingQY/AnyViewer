//
//  BaseView.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import "BaseView.h"

@implementation BaseView

- (void)drawRect:(CGRect)rect {
    [super drawRect:rect];
    
    [self bk_cornerRadius:self.bk_cornerRadius corner:self.bk_corner borderColor:self.bk_borderColor borderWidth:self.bk_borderWidth];
}

- (void)setImage:(UIImage *)image {
    _image = image;
    
    self.layer.contents = (__bridge id)image.CGImage;
}

@end

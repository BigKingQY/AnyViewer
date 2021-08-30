//
//  BaseTextField.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import "BaseTextField.h"

@implementation BaseTextField

- (CGRect)textRectForBounds:(CGRect)bounds {
    CGRect rect = [super textRectForBounds:bounds];
    rect.origin = CGPointMake(self.bk_inset.left, self.bk_inset.top);
    rect.size = CGSizeMake(rect.size.width-self.bk_inset.left-self.bk_inset.right, rect.size.height-self.bk_inset.top-self.bk_inset.bottom);
    return rect;
}

- (CGRect)editingRectForBounds:(CGRect)bounds {
    CGRect rect = [super editingRectForBounds:bounds];
    rect.origin = CGPointMake(self.bk_inset.left, self.bk_inset.top);
    rect.size = CGSizeMake(rect.size.width-self.bk_inset.left-self.bk_inset.right, rect.size.height-self.bk_inset.top-self.bk_inset.bottom);
    return rect;
}

- (void)drawRect:(CGRect)rect {
    [super drawRect:rect];
    
    [self bk_cornerRadius:self.bk_cornerRadius corner:self.bk_corner borderColor:self.bk_borderColor borderWidth:self.bk_borderWidth];
}

@end

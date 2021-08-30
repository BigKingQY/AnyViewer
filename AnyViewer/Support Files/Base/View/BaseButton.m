//
//  BaseButton.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import "BaseButton.h"

@implementation BaseButton

- (void)setHighlighted:(BOOL)highlighted {}

- (void)drawRect:(CGRect)rect {
    [super drawRect:rect];
    
    [self bk_cornerRadius:self.bk_cornerRadius corner:self.bk_corner borderColor:self.bk_borderColor borderWidth:self.bk_borderWidth];
}

@end

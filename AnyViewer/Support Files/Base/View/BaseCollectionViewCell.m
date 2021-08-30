//
//  BaseCollectionViewCell.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/8.
//

#import "BaseCollectionViewCell.h"

@implementation BaseCollectionViewCell

- (void)drawRect:(CGRect)rect {
    [super drawRect:rect];
    
    [self bk_cornerRadius:self.bk_cornerRadius corner:self.bk_corner borderColor:self.bk_borderColor borderWidth:self.bk_borderWidth];
}
@end

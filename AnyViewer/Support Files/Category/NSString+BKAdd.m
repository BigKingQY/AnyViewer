//
//  NSString+BKAdd.m
//  sz
//
//  Created by BigKing on 2020/12/3.
//  Copyright © 2020 hqht. All rights reserved.
//

#import "NSString+BKAdd.h"

@implementation NSString (BKAdd)

/// 计算常规字体文字大小
/// @param width 限制宽度
/// @param fontSize 字体大小
- (CGSize)sizeForWidth:(CGFloat)width fontSize:(CGFloat)fontSize {
    CGSize size = CGSizeMake(width, CGFLOAT_MAX);
    NSStringDrawingOptions options = NSStringDrawingUsesFontLeading | NSStringDrawingUsesLineFragmentOrigin;
    NSDictionary *att = @{NSFontAttributeName:[UIFont systemFontOfSize:fontSize]};
    CGSize textSize = [self boundingRectWithSize:size options:options attributes:att context:nil].size;
    
    return textSize;
}


/// 计算自定义字体文字大小
/// @param width 限制宽度
/// @param font 字体
- (CGSize)sizeForWidth:(CGFloat)width font:(UIFont *)font {
    CGSize size = CGSizeMake(width, CGFLOAT_MAX);
    NSStringDrawingOptions options = NSStringDrawingUsesFontLeading | NSStringDrawingUsesLineFragmentOrigin;
    NSDictionary *att = @{NSFontAttributeName:font};
    CGSize textSize = [self boundingRectWithSize:size options:options attributes:att context:nil].size;
    
    return textSize;
}


@end

//
//  NSString+BKAdd.h
//  sz
//
//  Created by BigKing on 2020/12/3.
//  Copyright © 2020 hqht. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSString (BKAdd)

//MARK: 文字大小计算

/// 计算常规字体文字大小
/// @param width 限制宽度
/// @param fontSize 字体大小
- (CGSize)sizeForWidth:(CGFloat)width fontSize:(CGFloat)fontSize;


/// 计算自定义字体文字大小
/// @param width 限制宽度
/// @param font 字体
- (CGSize)sizeForWidth:(CGFloat)width font:(UIFont *)font;


@end

NS_ASSUME_NONNULL_END

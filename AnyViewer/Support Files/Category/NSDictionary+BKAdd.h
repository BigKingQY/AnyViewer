//
//  NSDictionary+BKAdd.h
//  sz
//
//  Created by BigKing on 2020/12/30.
//  Copyright © 2020 hqht. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSDictionary (BKAdd)

///常规字体大小，颜色
+ (NSDictionary *)regularFontSize:(CGFloat)fontSize color:(uint32_t)color;
///中等字体大小，颜色
+ (NSDictionary *)mediumFontSize:(CGFloat)fontSize color:(uint32_t)color;
///粗体字体大小，颜色
+ (NSDictionary *)boldFontSize:(CGFloat)fontSize color:(uint32_t)color;
///字体，颜色
+ (NSDictionary *)textFont:(UIFont *)font color:(uint32_t)color;

@end

NS_ASSUME_NONNULL_END

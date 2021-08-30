//
//  UIImage+EXIF.h
//  EBOEboR
//
//  Created by Ebo on 17/3/21.
//  Copyright © 2017年 chuanbu. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (EXIF)

+ (UIImage *)imageWithView:(UIView *)view;

+ (NSDictionary *)getExifInfoWithImageData:(NSData *)imageData;

+ (UIImage *)addImage:(UIImage *)useImage useImageFrame:(CGRect)useImageFrame addMsakImage:(UIImage *)maskImage addMsakImageFrame:(CGRect)addMsakImageframe;

- (UIImage *)fixOrientation:(UIImage *)aImage;

-(UIImage*)imageChangeColor:(UIColor*)color;

+ (UIImage *)imageWithColor:(UIColor *)color;

- (UIImage *)ys_imageByRoundCornerRadius:(CGFloat)cornerRadius;

- (UIImage *)yy_imageByRoundCornerRadius:(CGFloat)radius
       corners:(UIRectCorner)corners
   borderWidth:(CGFloat)borderWidth
   borderColor:(UIColor *)borderColor
                          borderLineJoin:(CGLineJoin)borderLineJoin;

///图片压缩，指定最大长度
+ (UIImage *)compressImage:(UIImage *)image toByte:(NSUInteger)maxLength;

@end

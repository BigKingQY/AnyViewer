//
//  UIImageView+BKAdd.h
//  sz
//
//  Created by BigKing on 2020/10/20.
//  Copyright © 2020 hqht. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIImageView (BKAdd)

/// 设置用户头像，带默认头像占位图
/// @param url 图片链接
- (void)bk_setAvatrImageUrl:(NSString *)url;


/// 设置图片，无占位图
/// @param url 图片链接
- (void)bk_setImgaeWithUrl:(NSString *)url;


/// 设置图片，带占位图
/// @param url 图片链接
/// @param placeHolder 占位图
- (void)bk_setImgaeWithUrl:(NSString *)url placeHolder:(nullable NSString *)placeHolder;

@end

NS_ASSUME_NONNULL_END

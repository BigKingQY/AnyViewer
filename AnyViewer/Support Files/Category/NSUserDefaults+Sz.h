//
//  NSUserDefaults+Sz.h
//  sz
//
//  Created by mac on 2020/8/16.
//  Copyright © 2020 hqht. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSUserDefaults (Sz)

//+(void)saveToken:(NSString *)token;
//
//+(NSString *)getToken;

+ (void)saveSearchHistoryValue:(NSString *)value;
+ (NSArray *)getHistorySearchArray;
+ (void)saveSearchHistoryArray:(NSArray *)data;
+ (void)removeSaveSearchHistoryValue:(NSString *)value;
+ (void)removeSaveSearchHistoryAllValue;

+ (void)saveAdPictureUrl:(NSString *)adPictureUrl;
+ (NSString *)getAdPicture;

+ (void)saveUserNote:(NSString *)noteName userId:(NSString *)userId;
+(NSString *)getUserNoteName:(NSString *)userId;

+(void)saveFoundDynamicSearchArray:(NSArray *)data;
+(void)saveFoundDynamicSearchValue:(NSString *)value;
+ (NSArray *)getFoundDynamicSearchArray;
+ (void)removeFoundDynamicSearchArrayValue:(NSString *)value;
+ (void)removeFoundDynamicSearchArrayAllValue;

@end

NS_ASSUME_NONNULL_END

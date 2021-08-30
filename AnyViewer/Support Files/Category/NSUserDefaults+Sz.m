//
//  NSUserDefaults+Sz.m
//  sz
//
//  Created by mac on 2020/8/16.
//  Copyright © 2020 hqht. All rights reserved.
//

#import "NSUserDefaults+Sz.h"

@implementation NSUserDefaults (Sz)


//+(void)saveToken:(NSString *)token{
//    
//    [[NSUserDefaults standardUserDefaults] setObject:token forKey:@"token"];
//    [[NSUserDefaults standardUserDefaults] synchronize];
//}
//
//+(NSString *)getToken{
//    if (SzUserManager.shared.user) {
//        return SzUserManager.shared.user.token;
//    }
//    return @"";
//}

+ (void)saveSearchHistoryValue:(NSString *)value{
    NSMutableArray *data = [NSMutableArray arrayWithArray:[self getHistorySearchArray]];
    
    if ([data containsObject:value]) {
        [data removeObject:value];
        [data insertObject:value atIndex:0];
        [[NSUserDefaults standardUserDefaults] setObject:data forKey:@"historySearchArray"];
        [[NSUserDefaults standardUserDefaults] synchronize];
        return;
    }
    
    if ([value stringByReplacingOccurrencesOfString:@" " withString:@""].length == 0) {
        return;
    }
    
    if (data.count <= 0) {
        [data addObject:value];
    }else if (data.count >= 9){
        [data removeLastObject];
        [data insertObject:value atIndex:0];
    }else{
        [data insertObject:value atIndex:0];
    }
    
    [[NSUserDefaults standardUserDefaults] setObject:data forKey:@"historySearchArray"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

+ (NSArray *)getHistorySearchArray{
    return [[NSUserDefaults standardUserDefaults] objectForKey:@"historySearchArray"];
}

+(void)saveSearchHistoryArray:(NSArray *)data{
    [[NSUserDefaults standardUserDefaults] setObject:data forKey:@"historySearchArray"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

+ (void)removeSaveSearchHistoryValue:(NSString *)value{
    
    NSMutableArray *data = [NSMutableArray arrayWithArray:[self getHistorySearchArray]];
    if (data.count > 0) {
        [data removeObject:value];
    }
    [self saveSearchHistoryArray:data];
}

+ (void)removeSaveSearchHistoryAllValue{
    [self saveSearchHistoryArray:[NSArray new]];
}

+ (void)saveAdPictureUrl:(NSString *)adPictureUrl{
    [[NSUserDefaults standardUserDefaults] setObject:adPictureUrl forKey:@"adPictureUrl"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

+ (NSString *)getAdPicture{
    return [[NSUserDefaults standardUserDefaults] objectForKey:@"adPictureUrl"];
}

+ (void)saveUserNote:(NSString *)noteName userId:(NSString *)userId{
    [[NSUserDefaults standardUserDefaults] setObject:noteName forKey:userId];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

+(NSString *)getUserNoteName:(NSString *)userId{
    return [[NSUserDefaults standardUserDefaults] objectForKey:userId];
}

+(void)saveFoundDynamicSearchArray:(NSArray *)data{
    [[NSUserDefaults standardUserDefaults] setObject:data forKey:@"foundDynamicSearchArray"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

+(void)saveFoundDynamicSearchValue:(NSString *)value{
    NSMutableArray *data = [NSMutableArray arrayWithArray:[self getFoundDynamicSearchArray]];
    
    if ([data containsObject:value]) {
        [data removeObject:value];
        [data insertObject:value atIndex:0];
        [[NSUserDefaults standardUserDefaults] setObject:data forKey:@"foundDynamicSearchArray"];
        [[NSUserDefaults standardUserDefaults] synchronize];
        return;
    }
    
    if ([value stringByReplacingOccurrencesOfString:@" " withString:@""].length == 0) {
        return;
    }
    
    if (data.count <= 0) {
        [data addObject:value];
    }else if (data.count >= 9){
        [data removeLastObject];
        [data insertObject:value atIndex:0];
    }else{
        [data insertObject:value atIndex:0];
    }
    
    [[NSUserDefaults standardUserDefaults] setObject:data forKey:@"foundDynamicSearchArray"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

+ (NSArray *)getFoundDynamicSearchArray{
    return [[NSUserDefaults standardUserDefaults] objectForKey:@"foundDynamicSearchArray"];
}

+ (void)removeFoundDynamicSearchArrayValue:(NSString *)value{
    
    NSMutableArray *data = [NSMutableArray arrayWithArray:[self getFoundDynamicSearchArray]];
    if (data.count > 0) {
        [data removeObject:value];
    }
    [self saveFoundDynamicSearchArray:data];
}

+ (void)removeFoundDynamicSearchArrayAllValue{
    [self saveFoundDynamicSearchArray:[NSArray new]];
}


@end

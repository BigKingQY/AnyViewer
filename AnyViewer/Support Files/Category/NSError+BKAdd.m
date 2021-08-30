//
//  NSError+BKAdd.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import "NSError+BKAdd.h"

@implementation NSError (BKAdd)

+ (NSError *)errorWithCode:(NSInteger)code descript:(NSString *)descript {
    NSError *error = [NSError errorWithDomain:NSCocoaErrorDomain code:code userInfo:@{NSLocalizedDescriptionKey:descript}];
    return error;
}

@end

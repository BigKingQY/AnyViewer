//
//  NSError+BKAdd.h
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSError (BKAdd)

+ (NSError *)errorWithCode:(NSInteger)code descript:(NSString *)descript;

@end

NS_ASSUME_NONNULL_END

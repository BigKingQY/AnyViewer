//
//  BKIPRegionTool.h
//  AnyViewer
//
//  Created by BigKing on 2021/9/5.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^CompleteBlock)(NSString *region);

@interface BKIPRegionTool : NSObject

+ (BKIPRegionTool *)shared;

- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)new NS_UNAVAILABLE;

/// 根据IP获取区域
/// @param publicIP IP
/// @param complete 完成后的回调，返回区域内容
- (void)getRegionWithPublicIP:(NSString *)publicIP complete:(CompleteBlock)complete;

@end

NS_ASSUME_NONNULL_END

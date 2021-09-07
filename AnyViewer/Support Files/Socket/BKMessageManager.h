//
//  BKMessageManager.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/7.
//  主要用作代理消息

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN


@protocol BKMessageManagerDelegate <NSObject>


/// 注册后的回复，返回自己的设备ID
/// @param deviceId 设备ID
- (void)onRegistResponse:(const u_int64_t)deviceId;


/// 发送连接请求后的回调，返回两个状态码
/// @param status 状态码
/// @param otherStatus 其他状态码，可能没有
- (void)onConnectResponse:(const int)status otherStatus:(const int)otherStatus;

@end


@interface BKMessageManager : NSObject

+ (BKMessageManager *)shared;

- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)new NS_UNAVAILABLE;


@property (nonatomic, weak) id<BKMessageManagerDelegate> delegate;

@end

NS_ASSUME_NONNULL_END

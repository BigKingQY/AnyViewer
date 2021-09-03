//
//  AMSocketManager.h
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface AMSocketManager : NSObject

+ (AMSocketManager *)shared;

- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)new NS_UNAVAILABLE;


/// 连接远程控制服务器
- (BOOL)connectToRemoteServer;

@end

NS_ASSUME_NONNULL_END

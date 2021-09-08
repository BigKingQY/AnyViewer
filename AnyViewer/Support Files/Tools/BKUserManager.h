//
//  BKUserManager.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/8.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN


@interface BKUser : NSObject <NSSecureCoding, NSCopying>

///设备ID
@property (nonatomic, assign) u_int64_t deviceId;
///用户昵称
@property (nonatomic, copy) NSString *nickName;

@end

@interface BKUserManager : NSObject

///当前用户信息，包含设备ID和昵称
@property (nullable, nonatomic, strong, readonly) BKUser *user;

///获取当前用户的已连接历史记录
@property (nullable, nonatomic, strong, readonly) NSArray<BKUser *> *connectHistory;

///单例对象
+ (BKUserManager *)shared;

- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)new NS_UNAVAILABLE;


///更新用户信息
- (void)updateUser:(BKUser *)user;

/// 更新当前的user
- (void)update;

///退出，清空用户所有信息
- (void)logout;





/// 添加一个新的连接记录
/// @param user 用户对象
- (void)addConnectHistory:(BKUser *)user;

/// 删除某个连接记录
/// @param user 用户对象
- (void)removeConnectHistory:(BKUser *)user;


/// 通过设备ID来查询历史是否存在
/// @param deviceId 设备ID
- (BKUser *)findConnectHistory:(u_int64_t)deviceId;

/// 更新所有的连接记录
/// @param connectHistory 历史记录
- (void)updateConnectHistory:(NSArray<BKUser *> *)connectHistory;

///清空所有连接历史
- (void)clearHistory;


@end

NS_ASSUME_NONNULL_END

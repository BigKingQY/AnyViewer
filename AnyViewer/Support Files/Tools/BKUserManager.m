//
//  BKUserManager.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/8.
//

#import "BKUserManager.h"
#import <objc/runtime.h>

#define kUserPath [(NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).lastObject) stringByAppendingPathComponent:@"BKUser"]

@interface BKUser ()

///记录已连接设备的历史
@property (nullable, nonatomic, strong) NSMutableArray<BKUser *> *userConnectHistory;

@end

@implementation BKUser

- (instancetype)init {
    if (self = [super init]) {
        
        _deviceId = 0;
        _nickName = @"UnKnown Name";
    }
    return self;
}

- (NSMutableArray<BKUser *> *)userConnectHistory {
    if (!_userConnectHistory) {
        _userConnectHistory = [NSMutableArray array];
    }
    return _userConnectHistory;
}

- (void)encodeWithCoder:(NSCoder *)aCoder{
    unsigned int count = 0; //成员变量的个数
    Ivar *ivarList = class_copyIvarList(BKUser.class, &count);
    for (int i = 0; i < count; i++) {
        Ivar ivar = ivarList[i];
        NSString *key = [NSString stringWithUTF8String:ivar_getName(ivar)];
        [aCoder encodeObject:[self valueForKey:key] forKey:key];
    }
    free(ivarList);
}

- (nullable instancetype)initWithCoder:(NSCoder *)aDecoder{
    if (self == [super init]) {
        unsigned int count = 0; //成员变量的个数
        Ivar *ivarList = class_copyIvarList(BKUser.class, &count);
        for (int i = 0; i < count; i++) {
            Ivar ivar = ivarList[i];
            NSString *key = [NSString stringWithUTF8String:ivar_getName(ivar)];
            [self setValue:[aDecoder decodeObjectForKey:key] forKey:key];
        }
        free(ivarList);
    }
    return self;
}

- (NSString *)description {
    
    NSString *content = @"";
    unsigned int count = 0; //成员变量的个数
    Ivar *ivarList = class_copyIvarList(BKUser.class, &count);
    for (int i = 0; i < count; i++) {
        Ivar ivar = ivarList[i];
        NSString *key = [NSString stringWithUTF8String:ivar_getName(ivar)];
        
        content = [content stringByAppendingFormat:@"%@:%@\n", key, [self valueForKey:key.copy]];
    }
    free(ivarList);
    return content;
}

- (id)copyWithZone:(NSZone *)zone {
    return self;
}

- (id)mutableCopyWithZone:(NSZone *)zone {
    return self;
}

+ (BOOL)supportsSecureCoding {
    return YES;
}

@end

static BKUserManager *_manager;

@implementation BKUserManager

+ (BKUserManager *)shared {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        if (!_manager) {
            _manager = [[self alloc] init];
        }
    });
    return _manager;
}


/// 更新用户信息
/// @param user 用户对象
- (void)updateUser:(BKUser *)user
{
    
    if (@available(iOS 11.0, *)) {
        NSData *data = [NSKeyedArchiver archivedDataWithRootObject:user requiringSecureCoding:YES error:nil];
        
        BOOL ret = [data writeToFile:kUserPath atomically:YES];
        
        if (ret) {
            NSLog(@"写入成功");
        }else {
            NSLog(@"写入失败");
        }
    } else {
        // Fallback on earlier versions
        NSData *data = [NSKeyedArchiver archivedDataWithRootObject:user];
        [data writeToFile:kUserPath atomically:YES];
    }
}

/// 获取用户信息
- (BKUser *)user {
    
    NSData *data = [NSData dataWithContentsOfFile:kUserPath];
    if (!data) {
        return nil;
    }
    
    if (@available(iOS 11.0, *)) {
        return [NSKeyedUnarchiver unarchivedObjectOfClass:NSObject.class fromData:data error:nil];;
    } else {
        return [NSKeyedUnarchiver unarchiveObjectWithData:data];
    }
}

/// 删除所有数据
- (void)logout {
    
    if ([NSFileManager.defaultManager fileExistsAtPath:kUserPath]) {
        [NSFileManager.defaultManager removeItemAtPath:kUserPath error:nil];
    }
    
}


/// 更新当前的user
- (void)update {
    BKUser *user = BKUserManager.shared.user.copy;
    
    [BKUserManager.shared updateUser:user];
}


/// 获取当前用户的已连接历史记录
- (NSArray<BKUser *> *)connectHistory {
    return BKUserManager.shared.user.userConnectHistory.copy;
}

/// 添加一个新的连接记录
/// @param user 用户对象
- (void)addConnectHistory:(BKUser *)user {
    
    [BKUserManager.shared.user.userConnectHistory addObject:user];
    
    [BKUserManager.shared update];
}

/// 删除某个连接记录
/// @param user 用户对象
- (void)removeConnectHistory:(BKUser *)user {
    
    [BKUserManager.shared.user.userConnectHistory removeObject:user];
    
    [BKUserManager.shared update];
}

/// 通过设备ID来查询历史是否存在
/// @param deviceId 设备ID
- (BKUser *)findConnectHistory:(u_int64_t)deviceId {
    
    NSArray *array = BKUserManager.shared.connectHistory;
    BKUser *user = [BKUser new];
    for (BKUser *history in array) {
        if (deviceId == history.deviceId) {
            user = history;
            break;
        }
    }
    
    return user;
}

/// 更新所有的连接记录
/// @param connectHistory 历史记录
- (void)updateConnectHistory:(NSArray<BKUser *> *)connectHistory {
    
    BKUserManager.shared.user.userConnectHistory = [NSMutableArray arrayWithArray:connectHistory];
    
    [BKUserManager.shared update];
}

///清空所有连接历史
- (void)clearHistory {
    
    [BKUserManager.shared.user.userConnectHistory removeAllObjects];
    
    [BKUserManager.shared update];
}


@end

//
//  NetworkRequest.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/3.
//

#import <Foundation/Foundation.h>
#import <AFNetworking/AFHTTPSessionManager.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^BKSuccessBlock)(id _Nullable responseObject);
typedef void(^BKFailredBlock)(NSError *_Nullable error);

@interface NetworkRequest : NSObject

/// GET请求，默认JSON格式请求
/// @param urlString 网络链接
/// @param paramters 参数
/// @param success 成功的回调
/// @param failred 失败的回调
+ (void)GET:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
     failred:(nullable BKFailredBlock)failred;


/// GET请求
/// @param urlString 网络链接
/// @param paramters 参数
/// @param success 成功的回调
/// @param failred 失败的回调
/// @param json 是否是JSON格式
+ (void)GET:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
     failred:(nullable BKFailredBlock)failred
        json:(BOOL)json;


/// POST请求，默认JSON格式请求
/// @param urlString 网络链接
/// @param paramters 参数
/// @param success 成功的回调
/// @param failred 失败的回调
+ (void)POST:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
     failred:(nullable BKFailredBlock)failred;


/// POST请求
/// @param urlString 网络链接
/// @param paramters 参数
/// @param success 成功的回调
/// @param failred 失败的回调
/// @param json 是否是JSON格式
+ (void)POST:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
     failred:(nullable BKFailredBlock)failred
        json:(BOOL)json;


/// UPLOAD请求，默认JSON格式请求
/// @param urlString 网络链接
/// @param block 上传参数的回调
/// @param success 成功的回调
/// @param failred 失败的回调
+ (void)UPLOAD:(NSString *)urlString
constructingBlock:(void (^)(id<AFMultipartFormData> _Nonnull formData))block
       success:(nullable BKSuccessBlock)success
       failred:(nullable BKFailredBlock)failred;


/// UPLOAD请求
/// @param urlString 网络链接
/// @param block 上传参数的回调
/// @param success 成功的回调
/// @param failred 失败的回调
/// @param json 是否是JSON格式
+ (void)UPLOAD:(NSString *)urlString
constructingBlock:(void (^)(id<AFMultipartFormData> _Nonnull formData))block
       success:(nullable BKSuccessBlock)success
       failred:(nullable BKFailredBlock)failred
          json:(BOOL)json;


/// DELETE请求，默认JSON格式请求
/// @param urlString 网络链接
/// @param paramters 参数
/// @param success 成功的回调
/// @param failred 失败的回调
+ (void)DELETE:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
     failred:(nullable BKFailredBlock)failred;


/// DELETE请求
/// @param urlString 网络链接
/// @param paramters 参数
/// @param success 成功的回调
/// @param failred 失败的回调
/// @param json 是否是JSON格式
+ (void)DELETE:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
     failred:(nullable BKFailredBlock)failred
          json:(BOOL)json;


/// POST表单提交
/// @param urlString  网络链接
/// @param paramters 参数
/// @param success 成功的回调
/// @param failred 失败的回调
+ (void)POSTTheForm:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
            failred:(nullable BKFailredBlock)failred;

@end

NS_ASSUME_NONNULL_END

//
//  NetworkRequest.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/3.
//

#import "NetworkRequest.h"
#import "NSError+BKAdd.h"
#import "AMLoginViewController.h"

@implementation NetworkRequest

+ (void)GET:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
    failred:(nullable BKFailredBlock)failred {
    [self requestWithUrlString:urlString method:@"GET" paramters:paramters constructingBlock:NULL success:success failred:failred];
}

+ (void)GET:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
     failred:(nullable BKFailredBlock)failred
       json:(BOOL)json {
    [self requestWithUrlString:urlString method:@"GET" paramters:paramters constructingBlock:NULL success:success failred:failred isJson:json];

}

+ (void)POST:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
     failred:(nullable BKFailredBlock)failred {
    [self requestWithUrlString:urlString method:@"POST" paramters:paramters constructingBlock:NULL success:success failred:failred];
}

+ (void)POST:(NSString *)urlString paramters:(NSDictionary *)paramters success:(BKSuccessBlock)success failred:(BKFailredBlock)failred json:(BOOL)json {
    [self requestWithUrlString:urlString method:@"POST" paramters:paramters constructingBlock:NULL success:success failred:failred isJson:json];
}

+ (void)POSTTheForm:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
     failred:(nullable BKFailredBlock)failred {
    [self requestWithUrlString:urlString method:@"POSTTHEFORM" paramters:paramters constructingBlock:NULL success:success failred:failred];
}

+ (void)UPLOAD:(NSString *)urlString
constructingBlock:(void (^)(id<AFMultipartFormData> _Nonnull formData))block
       success:(nullable BKSuccessBlock)success
       failred:(nullable BKFailredBlock)failred {
    [self requestWithUrlString:urlString method:@"UPLOAD" paramters:nil constructingBlock:block success:success failred:failred];
}

+ (void)UPLOAD:(NSString *)urlString constructingBlock:(void (^)(id<AFMultipartFormData> _Nonnull))block success:(BKSuccessBlock)success failred:(BKFailredBlock)failred json:(BOOL)json {
    [self requestWithUrlString:urlString method:@"UPLOAD" paramters:nil constructingBlock:block success:success failred:failred isJson:json];
}

+ (void)DELETE:(NSString *)urlString
   paramters:(nullable NSDictionary *)paramters
     success:(nullable BKSuccessBlock)success
       failred:(nullable BKFailredBlock)failred {
    [self requestWithUrlString:urlString method:@"DELETE" paramters:paramters constructingBlock:nil success:success failred:failred];
}

+ (void)DELETE:(NSString *)urlString paramters:(NSDictionary *)paramters success:(BKSuccessBlock)success failred:(BKFailredBlock)failred json:(BOOL)json {
    [self requestWithUrlString:urlString method:@"DELETE" paramters:paramters constructingBlock:nil success:success failred:failred isJson:json];
}

+ (void)requestWithUrlString:(NSString *)urlString method:(NSString *)method paramters:(NSDictionary *)paramters constructingBlock:(nullable void (^)(id<AFMultipartFormData> _Nonnull))block success:(BKSuccessBlock)success failred:(BKFailredBlock)failred {
    
    [self requestWithUrlString:urlString method:method paramters:paramters constructingBlock:block success:success failred:failred isJson:YES];
}

+ (void)requestWithUrlString:(NSString *)urlString method:(NSString *)method paramters:(NSDictionary *)paramters constructingBlock:(nullable void (^)(id<AFMultipartFormData> _Nonnull))block success:(BKSuccessBlock)success failred:(BKFailredBlock)failred isJson:(BOOL)isJson {
    
    AFHTTPSessionManager *manager = [AFHTTPSessionManager manager];
    if (!isJson) {
        [manager setRequestSerializer:[AFHTTPRequestSerializer serializer]];
    }else {
        [manager setRequestSerializer:[AFJSONRequestSerializer serializer]];
    }
    [manager setResponseSerializer:[AFJSONResponseSerializer serializer]];
    manager.requestSerializer.timeoutInterval = 15.0;
    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"application/x-www-form-urlencoded;charset=UTF-8",@"application/x-www-form-urlencoded", @"text/plain",@"text/html", @"application/json", @"application/json;charset=UTF-8",@"text/json",@"text/javascript", nil];
    NSMutableDictionary *headers;
//    NSMutableDictionary *headers = @{@"Authorization":@"Basic bnM6bnNfc2tfMjAyMTA0MzA=",
//                                     @"Tenant-Id":@"000000"
//    }.mutableCopy;
//    if (JZMUserManager.shared.user) {
//        NSString *token = [NSString stringWithFormat:@"bearer %@", JZMUserManager.shared.user.token];
//        [headers setObject:token forKey:@"Blade-Auth"];
//    }
//
//    urlString = [NSString stringWithFormat:@"%@%@", JZMUserManager.shared.baseUrl, urlString];
    
    if ([method isEqualToString:@"GET"]) {
        [manager GET:urlString parameters:paramters headers:headers progress:nil success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
            [self handleSuccess:responseObject success:success failred:failred];
        } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
            [self handleError:error failred:failred];
        }];
    }else if ([method isEqualToString:@"POST"]) {
        [manager POST:urlString parameters:paramters headers:headers progress:nil success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
            [self handleSuccess:responseObject success:success failred:failred];
        } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
            [self handleError:error failred:failred];
        }];
    }else if ([method isEqualToString:@"UPLOAD"]) {
        [manager POST:urlString parameters:paramters headers:headers constructingBodyWithBlock:block progress:nil success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
            [self handleSuccess:responseObject success:success failred:failred];
        } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
            [self handleError:error failred:failred];
        }];
    }else if ([method isEqualToString:@"DELETE"]) {
        [manager DELETE:urlString parameters:paramters headers:headers success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
            [self handleSuccess:responseObject success:success failred:failred];
        } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
            [self handleError:error failred:failred];
        }];
    }else if ([method isEqualToString:@"POSTTHEFORM"]) {
        [manager POST:urlString parameters:paramters headers:headers constructingBodyWithBlock:^(id<AFMultipartFormData>  _Nonnull formData) {

                } progress:^(NSProgress * _Nonnull uploadProgress) {

                } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
                    [self handleSuccess:responseObject success:success failred:failred];
                } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
                    [self handleError:error failred:failred];
                }];
    }else {
        NSAssert(NO, @"请求方法类型错误，仅GET、POST、UPLOAD、DELETE");
    }
}

+ (void)handleSuccess:(id _Nullable)respoonseObject success:(BKSuccessBlock)success failred:(BKFailredBlock)failred {
    NSInteger code = [respoonseObject[@"code"] integerValue];
    if (code == 200) {
        if (success) {
            success(respoonseObject);
        }
    }else {
        NSError *error = [NSError errorWithCode:code descript:respoonseObject[@"msg"]];
        if (code == 401) {
            UIViewController *topController = UIApplication.sharedApplication.delegate.window.rootViewController;
            UIViewController *vc = [topController gk_visibleViewControllerIfExist];
            if (vc) {
//                [JZMUserManager.shared logout];
                run_main_after(1.2, ^{
                    //将我们的storyBoard实例化，“LoginViewController”为StoryBoard的名称
                    UIStoryboard *mainStoryBoard = [UIStoryboard storyboardWithName:@"LoginViewController" bundle:nil];
                    
                    //将第二个控制器实例化，"LoginViewController"为我们设置的控制器的ID
//                    LoginViewController *login = [mainStoryBoard instantiateViewControllerWithIdentifier:@"LoginViewController"];
//                    login.modalPresentationStyle = UIModalPresentationFullScreen;
//                    [vc presentViewController:login animated:YES completion:nil];
                });
            }
        }
        if (failred) {
            failred(error);
        }
    }
}

+ (void)handleError:(NSError *)error failred:(BKFailredBlock)failred {
    if ([error.localizedDescription containsString:@"401"]) {
//        UIViewController *topController = UIApplication.sharedApplication.delegate.window.rootViewController;
//        UIViewController *vc = [topController gk_visibleViewControllerIfExist];
//        if (vc && ![vc isKindOfClass:[LoginViewController class]]) {
//            [JZMUserManager.shared logout];
//            run_main_after(1.2, ^{
//                //将我们的storyBoard实例化，“LoginViewController”为StoryBoard的名称
//                UIStoryboard *mainStoryBoard = [UIStoryboard storyboardWithName:@"LoginViewController" bundle:nil];
//
//                //将第二个控制器实例化，"LoginViewController"为我们设置的控制器的ID
//                LoginViewController *login = [mainStoryBoard instantiateViewControllerWithIdentifier:@"LoginViewController"];
//                login.modalPresentationStyle = UIModalPresentationFullScreen;
//                [vc presentViewController:login animated:YES completion:nil];
//            });
//            return;
//        }
    }
    
    NSData *responseData = error.userInfo[AFNetworkingOperationFailingURLResponseDataErrorKey];
    NSString * receive = [[NSString alloc]initWithData:responseData encoding:NSUTF8StringEncoding];
    NSData *data = [receive dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableLeaves error:nil];
    
    NSError *err = nil;
    if (dict && dict[@"msg"]) {
        err = [NSError errorWithCode:error.code descript:dict[@"msg"]];
    }
    if (failred) {
        if (err) {
            failred(err);
        }else {
            failred(error);
        }
    }
}

@end

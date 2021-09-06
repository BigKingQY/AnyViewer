//
//  BKIPRegionTool.m
//  AnyViewer
//
//  Created by BigKing on 2021/9/5.
//

#import "BKIPRegionTool.h"
#import <WebKit/WebKit.h>

#define kPublicIPKey @"kPublicIPKey"

@interface BKIPRegionTool () <WKNavigationDelegate>

@property (nonatomic, copy) CompleteBlock complete;
@property (nonatomic, strong) WKWebView *webView;

@property (nonatomic, strong) NSString *publicIP;

@end

@implementation BKIPRegionTool

static BKIPRegionTool *_tool;
static dispatch_once_t onceToken;
+ (BKIPRegionTool *)shared {
    dispatch_once(&onceToken, ^{
        if (!_tool) {
            _tool = [[self alloc] init];
        }
    });
    return _tool;
}

+ (void)tearDown {
    onceToken = 0l;
    _tool = nil;
}

- (void)getRegionWithPublicIP:(NSString *)publicIP complete:(CompleteBlock)complete {
    
    //这里记录一个字典，用来保存公网地址对应的区域，格式@{@"110.10.2.12":@"成都市锦江区"}
    NSDictionary *dic = [NSUserDefaults.standardUserDefaults objectForKey:kPublicIPKey];
    if (dic && dic[publicIP]) {
        
        NSLog(@"使用上次的查询结果：%@:%@", publicIP, dic[publicIP]);
        complete(dic[publicIP]);

        [BKIPRegionTool tearDown];
        return;
    }
    
    self.publicIP = publicIP;
    
    NSString *url = [NSString stringWithFormat:@"https://www.ip138.com/iplookup.asp?ip=%@&action=2", publicIP];
    NSURLRequest *request = [[NSURLRequest alloc] initWithURL:[NSURL URLWithString:url]];
    
    run_main_queue(^{
        self.webView = [[WKWebView alloc] init];
        self.webView.navigationDelegate = self;
        
//        [UIApplication.sharedApplication.delegate.window addSubview:self.webView];
        [self.webView loadRequest:request];
    });
    
    
    self.complete = complete;
    
    [self performSelector:@selector(timeOut) afterDelay:5];
}

- (void)timeOut {
    if (self.complete) {
        self.complete(@"");
    }
    NSLog(@"获取IP地址区域失败：加载网页超时");

    [self clear];
}

- (void)webView:(WKWebView *)webView didFinishNavigation:(WKNavigation *)navigation {
    if (self.complete) {
        [webView evaluateJavaScript:@"ip_result" completionHandler:^(NSDictionary *dic, NSError * _Nullable error) {
            
            NSString *region = [dic[@"ASN归属地"] stringByReplacingOccurrencesOfString:@" " withString:@""];
            
            //保存本次的查询结果
            [NSUserDefaults.standardUserDefaults setObject:@{self.publicIP:region} forKey:kPublicIPKey];
            [NSUserDefaults.standardUserDefaults synchronize];
            
            self.complete(region);
                        
            [self clear];
        }];
    }
}

- (void)webView:(WKWebView *)webView didFailNavigation:(WKNavigation *)navigation withError:(NSError *)error {
    if (self.complete) {
        self.complete(@"");
    }
    NSLog(@"获取IP地址区域失败：%@", error.localizedDescription);

    [self clear];
}

- (void)clear {
    
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    
//    [self.webView removeFromSuperview];
    self.webView.navigationDelegate = nil;
    
    [BKIPRegionTool tearDown];
}


@end

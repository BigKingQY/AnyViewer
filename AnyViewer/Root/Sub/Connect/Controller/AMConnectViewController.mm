//
//  AMConnectViewController.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/23.
//

#import "AMConnectViewController.h"
#import "AMRemoteDeviceInputView.h"
#import "AMSafeCodeAlertView.h"
#import "AMDeviceControlViewController.h"
#import "BKMessageManager.h"

#include "LocalMessageBus.h"
#include "CNetTransactionEngine.h"
#include <string>

using namespace std;

@interface AMConnectViewController () <BKMessageManagerDelegate>

@property (nonatomic, strong) UILabel *titleLabel;
@property (nonatomic, strong) AMRemoteDeviceInputView *deviceView;
@property (nonatomic, strong) AMSafeCodeAlertView *codeView;


@end

@implementation AMConnectViewController

//MARK: LifeCycle - 生命周期

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self addDelegate];
    [self configUISet];
    [self loadData];
    [self addActions];
    
}


//MARK: Methods Override - 方法重写

//MARK: UISet - 界面布局

- (void)configUISet {
    [self.view addSubview:self.titleLabel];
    [self.view addSubview:self.deviceView];
}

//MARK: Layout - 布局设置

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    
    self.titleLabel.frame = CGRectMake(25, KSTATUS_BAR_H, self.view.width-50, KNAVIGATION_BAR_H);
    self.deviceView.frame = CGRectMake(0, 130, self.view.width, 200);
}

//MARK: Network Request - 网络请求

- (void)loadData {
    
}

//MARK: Delegate && DataSource - 代理

//BKMessageManagerDelegate

/// 注册后的回复，返回自己的设备ID
/// @param deviceId 设备ID
- (void)onRegistResponse:(const u_int64_t)deviceId {
    NSLog(@"我的设备ID：%llu", deviceId);
}


/// 发送连接请求后的回调，返回两个状态码
/// @param status 状态码
/// @param otherStatus 其他状态码，可能没有
- (void)onConnectResponse:(const int)status otherStatus:(const int)otherStatus {
    
    run_main_queue(^{
       
        switch (status)
        {
                
            case ES_SELECT_AUTH_METHOD: //选择认证方式
                
                break;
                
            case ES_NEED_PWD_AUTH:  //需要密码认证
                [self showPasswordAlert];
                break;
                
            case ES_NOT_FOUND_CUSTORM:  //没有找到客户信息
            case ES_CUSTORM_OFFLINE:    //客户不在线
                
                [MBProgressHUD showMessage:@"当前用户不存在或未在线" onView:self.view];
                break;
                
            case ES_PROHIBITION_CONTROL:    //设备禁止被控制
                
                [MBProgressHUD showMessage:@"目标设备禁止被控制，暂时无法连接" onView:self.view];
                break;
                
            case ES_REJECT_CTRL_REQUEST:    //拒绝控制请求
                
                [MBProgressHUD showMessage:@"对方拒绝了你的请求" onView:self.view];
                break;
                
            case ES_SRC_DEVICE_FROZEN:  //源设备被冻结
                
                [MBProgressHUD showMessage:@"当前设备已被冻结，暂时无法使用" onView:self.view];
                break;
                
            case ES_DST_DEVICE_FROZEN:  //目标设备被冻结
                
                [MBProgressHUD showMessage:@"目标设备已被冻结，暂时无法使用" onView:self.view];
                break;
                
            case ES_SUCCESS:    //连接成功
                
                
                [self handleConnectSuccess];
                break;
                
            default:    //其他错误处理
                
                [MBProgressHUD showMessage:@"连接失败，请稍后再试" onView:self.view];
                break;
        }
        
    });
        
}

- (void)handleConnectSuccess {
    
    [MBProgressHUD showSuccess:@"连接成功" onView:self.view];
    
    [self.codeView hide];
}

- (void)showPasswordAlert {
        
    run_main_queue(^{
        
        [MBProgressHUD hideForView:self.view];
        
        //弹出需要输入的安全码弹框
        [self.codeView showViewWithDeviceId:self.deviceView.deviceId nickName:@"KYO"];
        
        
    });
    
    //测试账号：941309969， 123qwe
}


//跳转控制页面
- (void)showControlViewController {
    
    AMDeviceControlViewController *controller = [AMDeviceControlViewController new];
    [self.navigationController pushViewController:controller animated:YES];
}


//MARK: EventResponse - 事件处理

- (void)addActions {
    
    //连接按钮点击事件
    [[self.deviceView.connButton rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
               

        if (self.deviceView.deviceId.length == 0) {
            [MBProgressHUD showMessage:@"请输入连接码" onView:self.view];
            return;
        }
        
        [MBProgressHUD showLoading:@"正在连接..." onView:self.view];
        
        U64 deviceId = [[self.deviceView.deviceId stringByReplacingOccurrencesOfString:@" " withString:@""] unsignedLongLongValue];
        
        GetTransactionInstance()->ConnectControlled(deviceId);

    }];
    
    
    //输入安全码确定点击事件
    [[self.codeView.doneButton rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
                    
        const U64 deviceId = [[self.deviceView.deviceId stringByReplacingOccurrencesOfString:@" " withString:@""] unsignedLongLongValue];
        
        NSString *code = self.codeView.textField.text;
        
        if (code.length == 0) {
            [MBProgressHUD showMessage:@"请输入安全码" onView:self.view];
            return;
        }
        
        GetTransactionInstance()->SendAuthenticationRequest(deviceId, code.UTF8String);
        
    }];
    
}

//MARK: Private Methods - 私有方法

//MARK: Public  Methods - 公用方法

//MARK: NotificationCenter - 通知中心


- (void)addDelegate {
    
    BKMessageManager.shared.delegate = self;
}





//MARK: Setter

//MARK: Getter

- (UILabel *)titleLabel {
    if (!_titleLabel) {
        _titleLabel = [UILabel new];
        _titleLabel.font = BKFont(24);
        _titleLabel.textColor = UIColor.whiteColor;
        _titleLabel.text = @"连接";
    }
    return _titleLabel;
}

- (AMRemoteDeviceInputView *)deviceView {
    if (!_deviceView) {
        _deviceView = [AMRemoteDeviceInputView new];
    }
    return _deviceView;
}

- (AMSafeCodeAlertView *)codeView {
    if (!_codeView) {
        _codeView = [[AMSafeCodeAlertView alloc] initWithFrame:CGRectMake(0, 0, kScreenWidth, kScreenHeight)];
    }
    return _codeView;
}

@end

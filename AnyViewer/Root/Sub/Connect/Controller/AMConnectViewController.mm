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


@interface AMConnectViewController () <BKMessageManagerDelegate>

@property (nonatomic, strong) UILabel *titleLabel;
@property (nonatomic, strong) AMRemoteDeviceInputView *deviceView;
@property (nonatomic, strong) AMSafeCodeAlertView *codeView;

/// 判断当前是否为手动认证
@property (nonatomic, assign) BOOL isManual;

@end

@implementation AMConnectViewController

//MARK: LifeCycle - 生命周期

- (void)viewDidLoad {
    [super viewDidLoad];
    
    
    [self configUISet];
    [self loadData];
    [self addActions];
    
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    //页面已经显示的时候设置代理
    [BKMessageManager.shared setDeletegateForTarget:self];
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

//MARK: BKMessageManagerDelegate

/// 注册后的回复，返回自己的设备ID
/// @param deviceId 设备ID
//- (void)onRegistResponse:(const u_int64_t)deviceId {
//    NSLog(@"我的设备ID：%llu", deviceId);
//
//    if (BKUserManager.shared.user) {
//
//        BKUserManager.shared.user.deviceId = deviceId;
//
//        [BKUserManager.shared update];
//
//    }else {
//
//        BKUser *user = [BKUser new];
//        user.deviceId = deviceId;
//
//        [BKUserManager.shared updateUser:user];
//    }
//}


/// 发送连接请求后的回调，返回两个状态码
/// @param status 状态码
/// @param otherStatus 其他状态码，可能没有
- (void)onConnectResponse:(const int)status otherStatus:(const int)otherStatus {
    
    run_main_queue(^{
       
        [MBProgressHUD hideForViewAndWindow];
        
        switch (status)
        {
            case ES_SUCCESS:    //连接成功
//
                [self handleConnectSuccess];
                break;
                
            case ES_SELECT_AUTH_METHOD: //选择认证方式
            {
                BOOL isExsit = [self checkUserPasswordExsit];
                if (!isExsit) {
                    [self handleAuthType];
                }
            }
                break;
                
            case ES_NEED_PWD_AUTH:  //需要密码认证
            {
                BOOL isExsit = [self checkUserPasswordExsit];
                if (!isExsit) {
                    [self showPasswordAlert];
                }
            }
                break;
                
            case ES_SRC_DEVICE_FROZEN:  //源设备被冻结
            
                [self handleSrcDeviceError:otherStatus];
                break;
                
            case ES_DST_DEVICE_FROZEN:  //目标设备被冻结
            
                [self handleDstDeviceError:otherStatus];
                break;
                                
            case ES_INVALID_PASSWD:     //密码错误
                
                [MBProgressHUD showMessageOnWindow:BKLocalizedString(@"IDS_INCORRECT_SECURITY_CODE")];
                break;
                
            case ES_NOT_FOUND_CUSTORM:  //没有找到客户信息
            case ES_CUSTORM_OFFLINE:    //客户不在线
                
                [MBProgressHUD showMessageOnWindow:BKLocalizedString(@"IDS_CUSTORM_OFFLINE")];
                break;
                
            case ES_PROHIBITION_CONTROL:    //设备禁止被控制
                
                [MBProgressHUD showMessageOnWindow:BKLocalizedString(@"IDS_PROHIBITION_CONTROL")];
                break;
                
            case ES_REJECT_CTRL_REQUEST:    //拒绝控制请求
                
                [MBProgressHUD showMessageOnWindow:BKLocalizedString(@"IDS_REJECT_CONTROL")];
                break;
                                
            case ES_TIME_OUT:   //超时
                
                [MBProgressHUD showMessageOnWindow:BKLocalizedString(@"IDS_CONNECT_TIME_OUT")];
                break;
                
            default:    //其他错误处理
                
                [MBProgressHUD showMessageOnWindow:BKLocalizedString(@"IDS_UNKNOWN_ERROR")];
                break;
        }
        
    });
        
}

///连接成功，跳转到远程设备的显示页面
- (void)handleConnectSuccess {
    
    //这里处理手动认证的结果，手动认证控制端不需要发起认证，直接处理返回的结果
    if (self.isManual) {
        
        NSLog(@"==== 手动认证成功 ====");
    }
    
}


///检查用户的密码是否存在，存在的话直接发起认证
- (BOOL)checkUserPasswordExsit {
    
    BKUser *user = [BKUserManager.shared findConnectHistory:[self getCurrentDeviceId]];
    
    if (user && user.password.length > 0) {
        
        [self sendAuthenticationRequest:user.password];
        
        return YES;
    }
    
    return NO;
}


///弹出选择认证方式的弹窗
- (void)handleAuthType {
        
    BKCellModel *manual = [BKCellModel initWithTitle:@"发送控制请求" icon:nil block:^(BKCellModel * _Nonnull model) {
        
        [self handleConnect:AM_MANUAL];
      
    }];
    
    BKCellModel *pwd = [BKCellModel initWithTitle:BKLocalizedString(@"PwdAuthRadio") icon:nil block:^(BKCellModel * _Nonnull model) {
        
        [self showPasswordAlert];
        
    }];
    
    BasePickerViewController *controller = [BasePickerViewController new];
    controller.dataSource = @[manual, pwd];
    
    [self bk_customDirectionalPresentViewController:controller animated:YES autoDismiss:YES];
}


///显示输入安全码的弹窗
- (void)showPasswordAlert {
        
    run_main_queue(^{
        
        BKUser *user = [BKUserManager.shared findConnectHistory:[self.deviceView.deviceId unsignedLongLongValue]];
        
        //弹出需要输入的安全码弹框
        [self.codeView showViewWithDeviceId:self.deviceView.deviceId nickName:user.nickName];
        
    });
    
    //测试账号：941309969， 123qwe
}



/// 处理源设备冻结的错误提示
/// @param otherStatus 解冻时长(秒)
- (void)handleSrcDeviceError:(const int)otherStatus {
        
    if (otherStatus > 60) {
        
        [MBProgressHUD showMessageOnWindow:[NSString stringWithFormat:BKLocalizedString(@"IDS_SRC_DEVICE_FORZEN_TIPS_MIN"), otherStatus / 60]];
    }else {
        
        [MBProgressHUD showMessageOnWindow:[NSString stringWithFormat:BKLocalizedString(@"IDS_SRC_DEVICE_FORZEN_TIPS_SEC"), otherStatus / 60]];
    }
}


/// 处理目标设备冻结的错误提示
/// @param otherStatus 解冻时长(秒)
- (void)handleDstDeviceError:(const int)otherStatus {
        
    if (otherStatus > 60) {
        
        [MBProgressHUD showMessageOnWindow:[NSString stringWithFormat:BKLocalizedString(@"IDS_DST_DEVICE_FORZEN_TIPS_MIN"), otherStatus / 60]];
    }else {
        
        [MBProgressHUD showMessageOnWindow:[NSString stringWithFormat:BKLocalizedString(@"IDS_DST_DEVICE_FORZEN_TIPS_SEC"), otherStatus / 60]];
    }
}

/// 发送认证后的回调，返回两个状态码
/// @param status 状态码1
/// @param otherStatus 被冻结时长，单位秒
- (void)onAuthenticatResponse:(const int)status otherStatus:(const int)otherStatus {
    
    NSLog(@"==== 收到认证后返回的消息: %d---%d ====", status, otherStatus);
    
    run_main_queue(^{
       
        [MBProgressHUD hideForViewAndWindow];
        
        //自动认证成功后在此处理，因为自动认证需要发起认证请求，收到回复
        if (status == ES_SUCCESS) {
            
            u_int64_t deviceId = [self getCurrentDeviceId];
            
            NSString *password = self.codeView.textField.text;
            
            run_global_queue(^{
               
                //有密码就保存起来
                if (password.length > 0) {
                    
                    //查询历史记录中是否存在该用户
                    BKUser *user = [BKUserManager.shared findConnectHistory:deviceId];
                    
                    //用户不存在时说明是第一次认证成功
                    if (!user) {
                        user = [BKUser new];
                    }
                    
                    //更新设备ID和密码
                    user.deviceId = deviceId;
                    user.password = password;
                    
                    [BKUserManager.shared addConnectHistory:user];
                }
                
            });
                        
            NSLog(@"==== 自动认证成功 ====");
            return;
            
        }else if (status == ES_INVALID_PASSWD) {
            
            //当返回密码错误时，需要判断当前界面是否展示了密码弹窗，如果没有展示需要重新展示出来
            //可能对方修改了密码，导致自动认证失败，还需要删除现有的密码，如果有保存过
            if (!self.codeView.isShow) {
                
                [MBProgressHUD showMessageOnWindow:BKLocalizedString(@"IDS_INCORRECT_SECURITY_CODE")];

                run_main_after(1.2, ^{
                    [self showPasswordAlert];
                });
            }
            
            //清除当前的用户，防止历史记录在最近连接列表数据不正常
            BKUser *user = [BKUserManager.shared findConnectHistory:[self getCurrentDeviceId]];
            
            if (user) {
                
                [BKUserManager.shared removeConnectHistory:user];
            }
            
        }
        
        
        //其他错误消息与连接时保持一致
        [self onConnectResponse:status otherStatus:otherStatus];
       
        NSLog(@"==== 自动认证失败 ====");
        
    });
    
}


/// 连接回调
/// @param success 是否成功
- (void)onVNCConnectResponse:(U32)sessionId success:(BOOL)success {
    
    run_main_queue(^{
       
        [MBProgressHUD hideForViewAndWindow];
        
        if (success) {
             
            [self showControlViewController:sessionId];
            
        }else {
            
            [MBProgressHUD showMessageOnWindow:BKLocalizedString(@"ConnectionIssueRadio")];
        }
        
    });

}


//跳转控制页面
- (void)showControlViewController:(U32)sessionId {
    
    AMDeviceControlViewController *controller = [AMDeviceControlViewController new];
    //保存会话ID
    controller.sessionId = sessionId;
    //设置代理
    [BKMessageManager.shared setDeletegateForTarget:controller];
    
    [self.navigationController pushViewController:controller animated:YES];
    
    //隐藏密码输入弹窗
    [self.codeView hide];
}


//MARK: EventResponse - 事件处理

- (void)addActions {
    
    //连接按钮点击事件
    [[self.deviceView.connButton rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
        
        [self.view endEditing:YES];
        [self handleConnect:AM_AUTO];

    }];
    
    
    //输入安全码确定点击事件
    [[self.codeView.doneButton rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
        
        [self.view endEditing:YES];
        [self sendAuthenticationRequest:self.codeView.textField.text];
    
    }];
    
}


- (void)sendAuthenticationRequest:(NSString *)code {
    
    [MBProgressHUD showLoadingOnWindow:BKLocalizedString(@"IDS_VERIFYING_TIPS")];

    const U64 deviceId = [self getCurrentDeviceId];
    
    GetTransactionInstance()->SendAuthenticationRequest(deviceId, code.UTF8String);
}



/// 选择连接方式，发送控制请求，默认是auto，手动传manual
/// @param controlMethod 控制方式
- (void)handleConnect:(AUTHENTICATION_METHOD)controlMethod {
    
    self.isManual = (controlMethod == AM_MANUAL);
    
    //当前设备还没注册成功
    if (BKUserManager.shared.user.deviceId == 0) {
        [MBProgressHUD showMessage:@"连接中，请稍后" onView:self.view];
        return;
    }
    
    U64 deviceId = [self getCurrentDeviceId];
    
    //判断输入的是否是自己的设备ID
    if (deviceId == BKUserManager.shared.user.deviceId) {
        [MBProgressHUD showMessage:BKLocalizedString(@"IDS_SAME_AS_LOCAL_ID_TIPS") onView:self.view];
        return;
    }
    
    [MBProgressHUD showLoading:BKLocalizedString(@"IDS_VERIFYING_TIPS") onView:self.view];
    
    GetTransactionInstance()->ConnectControlled(deviceId, controlMethod);
}


///返回当前输入的设备号，转成U64类型
- (U64)getCurrentDeviceId {
    
    U64 deviceId = [[self.deviceView.deviceId stringByReplacingOccurrencesOfString:@" " withString:@""] unsignedLongLongValue];
    
    return deviceId;
}

//MARK: Private Methods - 私有方法

//MARK: Public  Methods - 公用方法

//MARK: NotificationCenter - 通知中心

//MARK: Setter

//MARK: Getter

- (UILabel *)titleLabel {
    if (!_titleLabel) {
        _titleLabel = [UILabel new];
        _titleLabel.font = BKFont(24);
        _titleLabel.textColor = UIColor.whiteColor;
        _titleLabel.text = BKLocalizedString(@"ConnectButton");
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

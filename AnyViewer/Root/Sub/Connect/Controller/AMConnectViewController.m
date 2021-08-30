//
//  AMConnectViewController.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/23.
//

#import "AMConnectViewController.h"
#import "AMRemoteDeviceInputView.h"
#import "AMSafeCodeAlertViewController.h"
#import "AMDeviceControlViewController.h"


@interface AMConnectViewController ()

@property (nonatomic, strong) UILabel *titleLabel;
@property (nonatomic, strong) AMRemoteDeviceInputView *deviceView;

@end

@implementation AMConnectViewController

//MARK: LifeCycle - 生命周期

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self addNotification];
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

//MARK: EventResponse - 事件处理

- (void)addActions {
    
    [[self.deviceView.connButton rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
       
        
        
        
        
//        m_pVNCTCPServer = std::make_shared<CVNCDataTCPServer>(this, INADDR_ANY, USHORT(pPrjSettings->GetRFBPort()));
//
//        if (nullptr != m_pVNCTCPServer)
//        {
//            bool bResult = m_pVNCTCPServer->Initial();
//
//            if (bResult)
//            {
//                bResult = m_pVNCTCPServer->Start();
//            }
//
//            LOG_INFO("Start VNC TCP data server (port:%d) %s!", pPrjSettings->GetRFBPort() + 1, bResult ? "successfully" : "unsuccessfully");
//        }
//
        
        
        
        
        
        
        
        
        
        
        
        return;
        if (self.deviceView.deviceId.length == 0) {
            [MBProgressHUD showMessage:@"请输入连接码" onView:self.view];
            return;
        }
        
        //弹出需要输入的安全码弹框
        AMSafeCodeAlertViewController *controller = [AMSafeCodeAlertViewController new];
        [self bk_customFadedPresentViewController:controller cornerRadius:20 animated:YES autoDismiss:NO];
        controller.callback = ^(NSString * _Nonnull safeCode) {
            AMDeviceControlViewController *controller = [AMDeviceControlViewController new];
            [self.navigationController pushViewController:controller animated:YES];
        };
    }];
    
}

//MARK: Private Methods - 私有方法

//MARK: Public  Methods - 公用方法

//MARK: NotificationCenter - 通知中心

- (void)addNotification {}

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

@end

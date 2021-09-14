//
//  AMDeviceViewController.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/23.
//

#import "AMDeviceViewController.h"
#import "AMDeviceListTableViewCell.h"
#import "BKMessageManager.h"
#import "AMSafeCodeAlertView.h"
#import "AMDeviceControlViewController.h"

#include "LocalMessageBus.h"
#include "CNetTransactionEngine.h"


@interface AMDeviceViewController () <BKMessageManagerDelegate>

@property (nonatomic, strong) UILabel *titleLabel;

@property (nonatomic, strong) NSMutableArray<BKUser *> *dataSource;
@property (nonatomic, strong) AMSafeCodeAlertView *codeView;

@end

@implementation AMDeviceViewController

//MARK: LifeCycle - 生命周期

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self addNotification];
    [self configUISet];
    [self startRefresh];
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
    
    self.gk_navigationBar.hidden = YES;
    
    [self.view addSubview:self.titleLabel];
    
    [self.tableView registerClass:[AMDeviceListTableViewCell class] forCellReuseIdentifier:kAMDeviceListTableViewCellId];
    self.tableView.rowHeight = 140;
    
    [self addRefreshHeader];
    [self addEmptyDelegator];
  
}

- (void)refreshData {
    
    [self loadData];
}

//MARK: Layout - 布局设置

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    
    self.titleLabel.frame = CGRectMake(25, KSTATUS_BAR_H, self.view.width-50, KNAVIGATION_BAR_H);
    self.tableView.frame = CGRectMake(0, KSTATUS_NAVIGATION_H, self.view.width, self.view.height-KSTATUS_NAVIGATION_H);
}

//MARK: Network Request - 网络请求

- (void)loadData {
    
    self.dataSource = BKUserManager.shared.connectHistory.mutableCopy;
    
    if (self.dataSource.count == 0) {
        [self emptyDelegatorSubTitle:@"暂无连接记录"];
    }
    
    [self.tableView reloadData];
    
    [self endRefreshing];
}

//MARK: Delegate && DataSource - 代理

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.dataSource.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    AMDeviceListTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:kAMDeviceListTableViewCellId forIndexPath:indexPath];
    cell.user = self.dataSource[indexPath.row];
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
    //当前设备还没注册成功
    if (BKUserManager.shared.user.deviceId == 0) {
        [MBProgressHUD showMessage:@"连接中，请稍后" onView:self.view];
        return;
    }
    
    U64 deviceId = self.dataSource[indexPath.row].deviceId;

    [MBProgressHUD showLoading:BKLocalizedString(@"IDS_VERIFYING_TIPS") onView:self.view];
    
    GetTransactionInstance()->ConnectControlled(deviceId);
}

//MARK: BKMessageManagerDelegate

/// 注册后的回复，返回自己的设备ID
/// @param deviceId 设备ID
//- (void)onRegistResponse:(const u_int64_t)deviceId {
//    NSLog(@"我的设备ID：%llu", deviceId);
//
//    run_main_queue(^{
//        [MBProgressHUD hideForView:self.view];
//    });
//
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
//        
//}


/// 发送连接请求后的回调，返回两个状态码
/// @param status 状态码
/// @param otherStatus 其他状态码，可能没有
- (void)onConnectResponse:(const int)status otherStatus:(const int)otherStatus {
    
    run_main_queue(^{
       
        switch (status)
        {
            case ES_SUCCESS:    //连接成功
                
                [self handleConnectSuccess];
                break;
                
            case ES_SELECT_AUTH_METHOD: //选择认证方式
                
                [self handleAuthType];
                break;
                
            case ES_NEED_PWD_AUTH:  //需要密码认证
                [self showPasswordAlert];
                break;
                
            case ES_SRC_DEVICE_FROZEN:  //源设备被冻结
            
                [self handleSrcDeviceError:otherStatus];
                break;
                
            case ES_DST_DEVICE_FROZEN:  //目标设备被冻结
            
                [self handleDstDeviceError:otherStatus];
                break;
                                
            case ES_INVALID_PASSWD:
                
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
                                
            case ES_TIME_OUT:
                
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
    
    [self.codeView hide];
    
    
}



///弹出选择认证方式的弹窗
- (void)handleAuthType {
    
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
    
    //这里理论上跟连接返回的是一样的错误，只是分开了消息的发送
    NSLog(@"==== 收到认证后返回的消息 ====");
    [self onConnectResponse:status otherStatus:otherStatus];
   
}



- (void)showPasswordAlert {
        
    run_main_queue(^{
        
        [MBProgressHUD hideForView:self.view];
        
        //弹出需要输入的安全码弹框
//        [self.codeView showViewWithDeviceId:self.deviceView.deviceId nickName:@"KYO"];
        
    });
    
    //测试账号：941309969， 123qwe
}


//跳转控制页面
- (void)showControlViewController {
    
//    AMDeviceControlViewController *controller = [AMDeviceControlViewController new];
//    [self.navigationController pushViewController:controller animated:YES];
}

//MARK: EventResponse - 事件处理

- (void)addActions {}

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
        _titleLabel.text = @"最近连接";
    }
    return _titleLabel;
}

- (NSMutableArray<BKUser *> *)dataSource {
    if (!_dataSource) {
        _dataSource = [NSMutableArray array];
    }
    return _dataSource;
}

- (AMSafeCodeAlertView *)codeView {
    if (!_codeView) {
        _codeView = [[AMSafeCodeAlertView alloc] initWithFrame:CGRectMake(0, 0, kScreenWidth, kScreenHeight)];
    }
    return _codeView;
}

@end

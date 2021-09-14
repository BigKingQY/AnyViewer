//
//  AMDeviceControlViewController.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/25.
//

#import "AMDeviceControlViewController.h"
#import "AMDeviceControlView.h"
#import "BKMessageManager.h"
#include "CNetTransactionEngine.h"

@interface AMDeviceControlViewController ()

@property (nonatomic, strong) AMDeviceControlView *controlView;

@property (nonatomic, strong) UIButton *closeButton;


@property (nonatomic, assign) U32 sessionId;
@property (nonatomic, assign) U64 deviceId;


@end

@implementation AMDeviceControlViewController

//MARK: LifeCycle - 生命周期

- (instancetype)initWithDeviceId:(U64)deviceId sessionId:(U32)sessionId {
    if (self = [super init]) {
        self.deviceId = deviceId;
        self.sessionId = sessionId;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self addNotification];
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
    
    self.gk_navigationBar.hidden = YES;
    
    [self.view addSubview:self.controlView];
    
    [self.view addSubview:self.closeButton];
    
    GetTransactionInstance()->SetNeedUpdateFrameBuffer();
}

//MARK: Layout - 布局设置

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    
    self.controlView.frame = CGRectMake(0, KSTATUS_BAR_H, self.view.width, self.view.height-KHOME_INDICATOR_H-KSTATUS_BAR_H);
    
    self.closeButton.frame = CGRectMake(self.view.width-80, KSTATUS_NAVIGATION_H, 65, 40);
}

//MARK: Network Request - 网络请求

- (void)loadData {
    
}

//MARK: Delegate && DataSource - 代理


/// 视图大小改变时的回调
/// @param nWidth 宽度
/// @param nHeight 高度
- (void)onFrameBufferSizeChange:(const unsigned int)nWidth height:(const unsigned int)nHeight {
   
    [self.controlView updateFrameSize:nWidth height:nHeight];
}


/// 视图更新时的回调
/// @param image CGImage
- (void)onFrameBufferUpdate:(UIImage *)image {
    
    [self.controlView updateFrameWithImage:image];
}


//MARK: EventResponse - 事件处理

- (void)addActions {
    
    [[self.closeButton rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
       
        GetTransactionInstance()->CloseConnect(self.sessionId);
        
        [self.navigationController popViewControllerAnimated:YES];
        
    }];
    
}

//MARK: Private Methods - 私有方法

//MARK: Public  Methods - 公用方法

//MARK: NotificationCenter - 通知中心

- (void)addNotification {}

//MARK: Setter

//MARK: Getter

- (AMDeviceControlView *)controlView {
    if (!_controlView) {
        _controlView = [AMDeviceControlView new];
    }
    return _controlView;
}

- (UIButton *)closeButton {
    if (!_closeButton) {
        _closeButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [_closeButton setTitle:@"关闭连接" forState:UIControlStateNormal];
        _closeButton.titleLabel.font = BKFont(14);
    }
    return _closeButton;
}

@end

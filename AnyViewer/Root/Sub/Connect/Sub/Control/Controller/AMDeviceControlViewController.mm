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

@end

@implementation AMDeviceControlViewController

//MARK: LifeCycle - 生命周期

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


//图片处理，仅供参考
- (void)HandleImage:(UIImage *)img complite:(void(^)(UIImage *img))complite
{
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        CGImageRef imgref = img.CGImage;
        size_t width = CGImageGetWidth(imgref);
        size_t height = CGImageGetHeight(imgref);
        size_t bitsPerComponent = CGImageGetBitsPerComponent(imgref);
        size_t bitsPerPixel = CGImageGetBitsPerPixel(imgref);
        size_t bytesPerRow = CGImageGetBytesPerRow(imgref);
        
        CGColorSpaceRef colorSpace = CGImageGetColorSpace(imgref);
        CGBitmapInfo bitmapInfo = CGImageGetBitmapInfo(imgref);
        
        bool shouldInterpolate = CGImageGetShouldInterpolate(imgref);
        
        CGColorRenderingIntent intent = CGImageGetRenderingIntent(imgref);
        
        CGDataProviderRef dataProvider = CGImageGetDataProvider(imgref);
        
        CFDataRef data = CGDataProviderCopyData(dataProvider);
        
        UInt8 *buffer = (UInt8*)CFDataGetBytePtr(data);//Returns a read-only pointer to the bytes of a CFData object.// 首地址
        NSUInteger  x, y;
        // 像素矩阵遍历，改变成自己需要的值
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                UInt8 *tmp;
                tmp = buffer + y * bytesPerRow + x * 4;
                UInt8 alpha;
                alpha = *(tmp + 3);
                if (alpha) {// 透明不处理 其他变成红色
                    int r = arc4random() % 256;
                    int g = arc4random() % 256;
                    int b = arc4random() % 256;
                    *tmp = r;//red
                    *(tmp + 1) = g;//green
                    *(tmp + 2) = b;// Blue
                }
            }
        }
        
        CFDataRef effectedData = CFDataCreate(NULL, buffer, CFDataGetLength(data));
        
        CGDataProviderRef effectedDataProvider = CGDataProviderCreateWithCFData(effectedData);
        // 生成一张新的位图
        CGImageRef effectedCgImage = CGImageCreate(
                                                   width, height,
                                                   bitsPerComponent, bitsPerPixel, bytesPerRow,
                                                   colorSpace, bitmapInfo, effectedDataProvider,
                                                   NULL, shouldInterpolate, intent);
        
        UIImage *effectedImage = [[UIImage alloc] initWithCGImage:effectedCgImage];
        
        CGImageRelease(effectedCgImage);
        
        CFRelease(effectedDataProvider);
        
        CFRelease(effectedData);
        
        CFRelease(data);
        dispatch_async(dispatch_get_main_queue(), ^{
            if (complite) {
                complite(effectedImage);
            }
        });
    });
}

//MARK: Methods Override - 方法重写

//MARK: UISet - 界面布局

- (void)configUISet {
    
    self.gk_navigationBar.hidden = YES;
    
    [self.view addSubview:self.controlView];
    
    [self.view addSubview:self.closeButton];
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


/// 视图更新时的回调
/// @param image CGImage
- (void)onFrameBufferUpdate:(UIImage *)image {
    
    run_main_queue(^{
        self.controlView.contentImageView.image = image;
    });
    
    
    
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
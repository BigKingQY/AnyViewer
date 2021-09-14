//
//  AMDeviceControlView.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/25.
//

#import "AMDeviceControlView.h"
#import "BKFingerWaveView.h"

///设置最短的双击时间，两次点击的时间差在该范围内，就认为是双击
static const NSTimeInterval kMinDoubleClickTime = 0.3;

@interface AMDeviceControlView() <UIScrollViewDelegate>

@property (nonatomic, strong) UIScrollView *scrollView;

@property (nonatomic, strong) NSDate *lastTapDate;

@end

@implementation AMDeviceControlView

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        
        _scrollView = [UIScrollView new];
        _scrollView.delegate = self;
        _scrollView.scrollEnabled = YES;
        _scrollView.minimumZoomScale = 1;
        _scrollView.maximumZoomScale = 5;
        _scrollView.showsVerticalScrollIndicator = NO;
        _scrollView.showsHorizontalScrollIndicator = NO;
        [self addSubview:_scrollView];
        
        //初始化的时候需要将显示的图片大小设置好
        _contentImageView = [UIImageView new];
        _contentImageView.frame = CGRectMake(0,(kScreenHeight-KSTATUS_BAR_H-KHOME_INDICATOR_H-210)/2.f, kScreenWidth, 210);
        _contentImageView.contentMode = UIViewContentModeScaleAspectFit;
//        _contentImageView.image = BKImage(@"test");
        _contentImageView.userInteractionEnabled = YES;
        _contentImageView.backgroundColor = UIColor.redColor;
        [_scrollView addSubview:_contentImageView];
        
        [self addTapGestureRecognizer];
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    self.scrollView.frame = CGRectMake(0, 0, self.width, self.height);
}


- (void)updateFrameWithImage:(UIImage *)image {
    
    run_main_queue(^{
        
        self.contentImageView.image = image;
    });
}

- (void)updateFrameSize:(const unsigned int)width height:(const unsigned int)height {
    
    run_main_queue(^{
        
        if (width == 0 || height == 0) {
            return;
        }
        
        CGFloat sheight = kScreenWidth / width * height;
        
        [UIView animateWithDuration:0.35 animations:^{
                
            self.contentImageView.frame = CGRectMake(0,(kScreenHeight-KSTATUS_BAR_H-KHOME_INDICATOR_H-sheight)/2.f, kScreenWidth, sheight);
        }];
        
    });
}

//MARK: Action

- (void)addTapGestureRecognizer {
    [self addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithActionBlock:^(UITapGestureRecognizer *sender) {
        
        [self handleTapGesture:sender];

    }]];
    
    [self addGestureRecognizer:[[UILongPressGestureRecognizer alloc] initWithActionBlock:^(UILongPressGestureRecognizer *sender) {
        
        NSLog(@"长按了屏幕");
        
        [self handleLongGesture:sender];
        
    }]];
}

- (void)handleTapGesture:(UITapGestureRecognizer *)sender {
    
    CGPoint center = [sender locationInView:sender.view];
            
    if (CGRectContainsPoint(self.contentImageView.frame, center)) {

        [BKFingerWaveView showInView:self center:center];
        
        CGPoint p = [self.contentImageView convertPoint:center fromView:self];
        NSLog(@"imageView的坐标：%@", NSStringFromCGPoint(p));
        
        NSDate *date = [NSDate date];
        NSTimeInterval timeInterval = [date timeIntervalSinceDate:self.lastTapDate];

        if (self.lastTapDate
            && timeInterval < kMinDoubleClickTime) {
            NSLog(@"双击了");
        }else {
            NSLog(@"单击了");
        }
        
        //记录上一次的点击时间
        self.lastTapDate = date;
    }
}

- (void)handleLongGesture:(UILongPressGestureRecognizer *)sender {
    
}


//MARK: UIScrollViewDelegate

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView {
    
    return self.contentImageView;
}

- (void)scrollViewDidZoom:(UIScrollView *)scrollView {
    
    CGRect frame = self.contentImageView.frame;
    
    frame.origin.y = (self.scrollView.height - self.contentImageView.height) > 0 ? (self.scrollView.height - self.contentImageView.height) * 0.5 : 0;
    frame.origin.x = (self.scrollView.width - self.contentImageView.width) > 0 ? (self.scrollView.width - self.contentImageView.width) * 0.5 : 0;
    self.contentImageView.frame = frame;
    
    self.scrollView.contentSize = CGSizeMake(self.contentImageView.width, self.contentImageView.height);
}

@end

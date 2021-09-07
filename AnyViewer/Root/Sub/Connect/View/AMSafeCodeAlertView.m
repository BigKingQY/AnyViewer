//
//  AMSafeCodeAlertView.m
//  AnyViewer
//
//  Created by Aomei on 2021/9/7.
//

#import "AMSafeCodeAlertView.h"

@interface AMSafeCodeAlertView ()

@property (nonatomic, strong) BaseButton *backButton;
@property (nonatomic, strong) UILabel *titleLabel;

@property (nonatomic, strong) BaseView *bgView;
@property (nonatomic, strong) UIImageView *iconImageView;
@property (nonatomic, strong) UILabel *nameLabel;
@property (nonatomic, strong) UILabel *codeLabel;
@property (nonatomic, strong) UILabel *tipsLabel;

@end

@implementation AMSafeCodeAlertView

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        
        _backButton = [BaseButton new];
        [_backButton setImage:BKImage(@"") forState:UIControlStateNormal];
        [self addSubview:_backButton];
        
        _titleLabel = [UILabel new];
        _titleLabel.font = BKFont(18);
        _titleLabel.text = @"连接";
        _titleLabel.textColor = UIColor.whiteColor;
        _titleLabel.textAlignment = NSTextAlignmentCenter;
        [self addSubview:_titleLabel];
        
        _bgView = [BaseView new];
        _bgView.backgroundColor = BKColor(0x414C59);
        _bgView.bk_cornerRadius = 20;
        [self addSubview:_bgView];
        
        _iconImageView = [UIImageView new];
        _iconImageView.contentMode = UIViewContentModeScaleAspectFit;
        _iconImageView.image = [UIImage imageNamed:@"icon_computer"];
        [_bgView addSubview:_iconImageView];
        
        _nameLabel = [UILabel new];
        _nameLabel.font = BKFont(18);
        _nameLabel.textColor = UIColor.whiteColor;
        _nameLabel.textAlignment = NSTextAlignmentCenter;
        [_bgView addSubview:_nameLabel];
        
        _codeLabel = [UILabel new];
        _codeLabel.font = BKFont(14);
        _codeLabel.textColor = UIColor.whiteColor;
        _codeLabel.textAlignment = NSTextAlignmentCenter;
        [_bgView addSubview:_codeLabel];
        
        _tipsLabel = [UILabel new];
        _tipsLabel.font = BKFont(16);
        _tipsLabel.textColor = UIColor.whiteColor;
        _tipsLabel.textAlignment = NSTextAlignmentCenter;
        _tipsLabel.text = @"请输入设备安全码";
        [_bgView addSubview:_tipsLabel];
        
        _textField = [BaseTextField new];
        _textField.font = BKFont(16);
        _textField.textColor = UIColor.whiteColor;
        _textField.placeholder = @"输入安全码";
        _textField.backgroundColor = BKColor(0x2e3439);
        _textField.tintColor = UIColor.whiteColor;
        _textField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
        _textField.secureTextEntry = YES;
        _textField.bk_inset = UIEdgeInsetsMake(0, 10, 0, 10);
        _textField.bk_cornerRadius = 8;
        [_bgView addSubview:_textField];
        
        _doneButton = [BaseButton new];
        [_doneButton setTitle:@"确定" forState:UIControlStateNormal];
        _doneButton.titleLabel.font = BKFont(16);
        _doneButton.backgroundColor = BKColor(0x0085ff);
        _doneButton.bk_cornerRadius = 8;
        [_bgView addSubview:_doneButton];
        
        self.backgroundColor = BKColor(0x1e2124);
        
        [self addActions];
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    self.backButton.frame = CGRectMake(10, KSTATUS_BAR_H, 44, 44);
    self.titleLabel.frame = CGRectMake(self.backButton.right, self.backButton.top, self.width-108, 44);
    self.bgView.frame = CGRectMake(20, self.height/2.f-250, self.width-40, 500);
    self.iconImageView.frame = CGRectMake(self.bgView.width/2.f-50.f, 90, 100, 100);
    self.nameLabel.frame = CGRectMake(0, self.iconImageView.bottom+20, self.bgView.width, 21);
    self.codeLabel.frame = CGRectMake(0, self.nameLabel.bottom+10, self.nameLabel.width, 16);
    self.tipsLabel.frame = CGRectMake(0, self.codeLabel.bottom+58, self.nameLabel.width, 18);
    self.textField.frame = CGRectMake(25, self.tipsLabel.bottom+22, self.bgView.width-50, 50);
    self.doneButton.frame = CGRectMake(25, self.textField.bottom+20, self.textField.width, 50);
}


- (void)addActions {
    
    [[self.backButton rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
       
        [self hide];
    }];
}


- (void)showViewWithDeviceId:(NSString *)deviceId nickName:(NSString *)nickName {
    
    self.alpha = 0;
    self.nameLabel.text = nickName;
    self.codeLabel.text = deviceId;
    
    self.textField.text = @"";
    
    UIWindow *window = UIApplication.sharedApplication.delegate.window;
    
    if (![window.subviews containsObject:self]) {
        [window addSubview:self];
    }
    
    [UIView animateWithDuration:0.35 animations:^{
        
        self.alpha = 1;
    }];
    
}

- (void)hide {
    
    [UIView animateWithDuration:0.35 animations:^{
        self.alpha = 0;
    } completion:^(BOOL finished) {
        
    }];
}


@end

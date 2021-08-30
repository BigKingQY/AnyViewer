//
//  AMSafeCodeAlertViewController.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/24.
//

#import "AMSafeCodeAlertViewController.h"

@interface AMSafeCodeAlertViewController ()

@property (nonatomic, strong) UIImageView *iconImageView;
@property (nonatomic, strong) UILabel *nameLabel;
@property (nonatomic, strong) UILabel *codeLabel;
@property (nonatomic, strong) UILabel *tipsLabel;
@property (nonatomic, strong) BaseTextField *textField;
@property (nonatomic, strong) BaseButton *doneButton;

@property (nonatomic, strong) CADisplayLink *displayLink;
@property (nonatomic, strong) NSThread *thread;

@end

@implementation AMSafeCodeAlertViewController

//MARK: LifeCycle - 生命周期

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self addNotification];
    [self configUISet];
    [self loadData];
    [self addActions];
    
    
}




//MARK: Methods Override - 方法重写

- (CGSize)preferredContentSize {
    return CGSizeMake(kScreenWidth-40, 500);
}

//MARK: UISet - 界面布局

- (void)configUISet {
    [self.view addSubview:self.iconImageView];
    [self.view addSubview:self.nameLabel];
    [self.view addSubview:self.codeLabel];
    [self.view addSubview:self.tipsLabel];
    [self.view addSubview:self.textField];
    [self.view addSubview:self.doneButton];
}

//MARK: Layout - 布局设置

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    
    self.iconImageView.frame = CGRectMake(self.view.width/2.f-50.f, 90, 100, 100);
    self.nameLabel.frame = CGRectMake(0, self.iconImageView.bottom+20, self.view.width, 21);
    self.codeLabel.frame = CGRectMake(0, self.nameLabel.bottom+10, self.nameLabel.width, 16);
    self.tipsLabel.frame = CGRectMake(0, self.codeLabel.bottom+58, self.nameLabel.width, 18);
    self.textField.frame = CGRectMake(25, self.tipsLabel.bottom+22, self.view.width-50, 50);
    self.doneButton.frame = CGRectMake(25, self.textField.bottom+20, self.textField.width, 50);
}

//MARK: Network Request - 网络请求

- (void)loadData {
    
}

//MARK: Delegate && DataSource - 代理

//MARK: EventResponse - 事件处理

- (void)addActions {
    
    [[self.doneButton rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
            
        if (self.textField.text.length == 0) {
            [MBProgressHUD showMessage:@"请输入设备安全码" onView:self.view];
            return;
        }
        
        [self dismissViewControllerAnimated:YES completion:^{
            if (self.callback) {
                self.callback(self.textField.text);
            }            
        }];
        
    }];
}

//MARK: Private Methods - 私有方法

//MARK: Public  Methods - 公用方法

//MARK: NotificationCenter - 通知中心

- (void)addNotification {}

//MARK: Setter

//MARK: Getter

- (UIImageView *)iconImageView {
    if (!_iconImageView) {
        _iconImageView = [UIImageView new];
        _iconImageView.contentMode = UIViewContentModeScaleAspectFit;
        _iconImageView.image = [UIImage imageNamed:@"icon_computer"];
    }
    return _iconImageView;
}

- (UILabel *)nameLabel {
    if (!_nameLabel) {
        _nameLabel = [UILabel new];
        _nameLabel.font = BKFont(18);
        _nameLabel.textColor = UIColor.whiteColor;
        _nameLabel.textAlignment = NSTextAlignmentCenter;
        _nameLabel.text = @"KYO";
    }
    return _nameLabel;
}

- (UILabel *)codeLabel {
    if (!_codeLabel) {
        _codeLabel = [UILabel new];
        _codeLabel.font = BKFont(14);
        _codeLabel.textColor = UIColor.whiteColor;
        _codeLabel.textAlignment = NSTextAlignmentCenter;
        _codeLabel.text = @"123 123 123";
    }
    return _codeLabel;
}

- (UILabel *)tipsLabel {
    if (!_tipsLabel) {
        _tipsLabel = [UILabel new];
        _tipsLabel.font = BKFont(16);
        _tipsLabel.textColor = UIColor.whiteColor;
        _tipsLabel.textAlignment = NSTextAlignmentCenter;
        _tipsLabel.text = @"请输入设备安全码";
    }
    return _tipsLabel;
}

- (BaseTextField *)textField {
    if (!_textField) {
        _textField = [BaseTextField new];
        _textField.font = BKFont(16);
        _textField.textColor = UIColor.whiteColor;
        _textField.placeholder = @"输入安全码";
        _textField.backgroundColor = BKColor(0x2e3439);
        _textField.tintColor = UIColor.whiteColor;
        _textField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
        _textField.bk_inset = UIEdgeInsetsMake(0, 10, 0, 10);
        _textField.bk_cornerRadius = 8;
    }
    return _textField;
}

- (BaseButton *)doneButton {
    if (!_doneButton) {
        _doneButton = [BaseButton new];
        [_doneButton setTitle:@"确定" forState:UIControlStateNormal];
        _doneButton.titleLabel.font = BKFont(16);
        _doneButton.backgroundColor = BKColor(0x0085ff);
        _doneButton.bk_cornerRadius = 8;
    }
    return _doneButton;
}


@end

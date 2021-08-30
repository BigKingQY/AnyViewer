//
//  BasePickerViewController.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/14.
//

#import "BasePickerViewController.h"

@interface BasePickerViewController () <UIPickerViewDelegate, UIPickerViewDataSource>

@property (nonatomic, strong) UIPickerView *pickerView;
@property (nonatomic, strong) UILabel *titleLabel;
@property (nonatomic, strong) BaseButton *leftButton;
@property (nonatomic, strong) BaseButton *rightButton;

@property (nonatomic, strong) BKCellModel *currentModel;

@end

@implementation BasePickerViewController

//MARK: LifeCycle - 生命周期

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self addNotification];
    [self configUISet];
    [self addActions];
    [self loadData];
}

//MARK: UISet - 界面布局

- (void)configUISet {
    
    self.gk_statusBarHidden = YES;
    self.gk_navigationBar.hidden = YES;
    
    [self.view addSubview:self.pickerView];
    [self.view addSubview:self.leftButton];
    [self.view addSubview:self.titleLabel];
    [self.view addSubview:self.rightButton];
}

//MARK: Methods Override - 方法重写

- (CGSize)preferredContentSize {
    CGFloat height = 0;
    if (self.dataSource.count < 5) {
        height = 220;
    }else {
        height = 400;
    }
    return CGSizeMake(kScreenWidth, height+KHOME_INDICATOR_H);
}

//MARK: EventResponse - 事件处理

- (void)addActions {
    
    [[self.leftButton rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
        [self dismissViewControllerAnimated:YES completion:nil];
    }];
    
    [[self.rightButton rac_signalForControlEvents:UIControlEventTouchUpInside] subscribeNext:^(__kindof UIControl * _Nullable x) {
        [self dismissViewControllerAnimated:YES completion:^{
            BKCellModel *model = nil;
            if (self.currentModel) {
                model = self.currentModel;
            }else {
                model = self.dataSource.firstObject;
            }
            if (self.selectedBlock) {
                self.selectedBlock(model);
            }
        }];
    }];
    
}

//MARK: Private Methods - 私有方法

//MARK: Public  Methods - 公用方法

//MARK: Network Request - 网络请求

- (void)loadData {
    
}

- (void)setDataSource:(NSArray<BKCellModel *> *)dataSource {
    _dataSource = dataSource;
    
    [self.pickerView reloadAllComponents];
}

//MARK: Delegate && DataSource - 代理

- (CGFloat)pickerView:(UIPickerView *)pickerView rowHeightForComponent:(NSInteger)component {
    return 50;
}

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    return self.dataSource.count;
}

- (UIView *)pickerView:(UIPickerView *)pickerView viewForRow:(NSInteger)row forComponent:(NSInteger)component reusingView:(UIView *)view {
    UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, kScreenWidth-24, 50)];
    label.text = self.dataSource[row].title;
    label.textColor = UIColor.whiteColor;
    label.textAlignment = NSTextAlignmentCenter;
    return label;
}

//- (nullable NSAttributedString *)pickerView:(UIPickerView *)pickerView attributedTitleForRow:(NSInteger)row forComponent:(NSInteger)component {
//    return [[NSAttributedString alloc] initWithString:self.dataSource[row].title attributes:[NSDictionary regularFontSize:15 color:0xffffff]];
//}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component {
    self.currentModel = self.dataSource[row];
}


//MARK: Layout - 布局设置

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    
    self.leftButton.frame = CGRectMake(0, 0, 80, 44);
    self.titleLabel.frame = CGRectMake(self.leftButton.right, 0, self.view.width-160, 44);
    self.rightButton.frame = CGRectMake(self.view.width-80, 0, 80, 44);
    self.pickerView.frame = CGRectMake(0, self.leftButton.bottom, self.view.width, self.view.height-self.leftButton.bottom-KHOME_INDICATOR_H);
}

//MARK: NotificationCenter - 通知中心

- (void)addNotification {}

//MARK: Setter

- (void)setNaviTitle:(NSString *)naviTitle {
    _naviTitle = naviTitle;
    
    self.titleLabel.text = naviTitle;
}

//MARK: Getter

- (UIPickerView *)pickerView {
    if (!_pickerView) {
        _pickerView = [[UIPickerView alloc] init];
        _pickerView.delegate = self;
        _pickerView.dataSource = self;
    }
    return _pickerView;
}

- (UILabel *)titleLabel {
    if (!_titleLabel) {
        _titleLabel = [UILabel new];
        _titleLabel.font = BKFont(15);
        _titleLabel.textColor = UIColor.whiteColor;
        _titleLabel.textAlignment = NSTextAlignmentCenter;
        _titleLabel.text = @"请选择";
    }
    return _titleLabel;
}

- (BaseButton *)leftButton {
    if (!_leftButton) {
        _leftButton = [BaseButton buttonWithType:UIButtonTypeCustom];
        [_leftButton setTitle:@"取消" forState:UIControlStateNormal];
        _leftButton.titleLabel.font = BKFont(14);
        _leftButton.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
        _leftButton.contentEdgeInsets = UIEdgeInsetsMake(0, 12, 0, 0);
    }
    return _leftButton;
}

- (BaseButton *)rightButton {
    if (!_rightButton) {
        _rightButton = [BaseButton buttonWithType:UIButtonTypeCustom];
        [_rightButton setTitle:@"确定" forState:UIControlStateNormal];
        _rightButton.titleLabel.font = BKFont(14);
        _rightButton.contentHorizontalAlignment = UIControlContentHorizontalAlignmentRight;
        _rightButton.contentEdgeInsets = UIEdgeInsetsMake(0, 0, 0, 12);
    }
    return _rightButton;
}

@end

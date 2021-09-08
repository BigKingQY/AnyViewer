//
//  AMRemoteDeviceInputView.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/24.
//

#import "AMRemoteDeviceInputView.h"

@interface AMRemoteDeviceInputView () <UITextFieldDelegate>

@property (nonatomic, strong) BaseView *bgView;
@property (nonatomic, strong) UILabel *titleLabel;
@property (nonatomic, strong) BaseTextField *textField;

@property (nonatomic, assign) NSInteger textCount;

@property (nonatomic, strong) NSString *previousTextFieldContent;
@property (nonatomic, strong) UITextRange *previousSelection;

@end

@implementation AMRemoteDeviceInputView

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        
        _bgView = [BaseView new];
        _bgView.backgroundColor = BKColor(0x414C59);
        _bgView.bk_cornerRadius = 20;
        [self addSubview:_bgView];
        
        _titleLabel = [UILabel new];
        _titleLabel.font = BKFont(16);
        _titleLabel.textColor = UIColor.whiteColor;
        _titleLabel.text = @"控制远程设备";
        [_bgView addSubview:_titleLabel];
        
        _textField = [BaseTextField new];
        _textField.delegate = self;
        _textField.font = BKFont(16);
        _textField.textColor = UIColor.whiteColor;
        _textField.placeholder = @"输入伙伴的设备ID";
        _textField.backgroundColor = BKColor(0x2e3439);
        _textField.keyboardType = UIKeyboardTypeNumberPad;
        _textField.tintColor = UIColor.whiteColor;
        _textField.bk_inset = UIEdgeInsetsMake(0, 10, 0, 10);
        _textField.bk_cornerRadius = 8;
        [_bgView addSubview:_textField];
        
        _connButton = [BaseButton new];
        [_connButton setTitle:BKLocalizedString(@"ConnectButton") forState:UIControlStateNormal];
        _connButton.titleLabel.font = BKFont(16);
        [_connButton setBackgroundImage:[UIImage imageWithColor:BKColor(0x0085ff)] forState:UIControlStateNormal];
        [_connButton setBackgroundImage:[UIImage imageWithColor:BKColor(0x666666)] forState:UIControlStateDisabled];

        _connButton.bk_cornerRadius = 8;
        [_bgView addSubview:_connButton];
        
        _textCount = 0;
        
        [self addAction];
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    self.bgView.frame = CGRectMake(20, 0, self.width-40, 200);
    self.titleLabel.frame = CGRectMake(25, 15, self.bgView.width-50, 18);
    self.textField.frame = CGRectMake(25, self.titleLabel.bottom+22, self.titleLabel.width, 50);
    self.connButton.frame = CGRectMake(25, self.textField.bottom+20, self.textField.width, 50);
}

- (void)addAction {
    
    RAC(self.connButton, enabled) =  [self.textField.rac_textSignal merge:RACObserve(self.textField, text)];

    [self.textField.rac_textSignal subscribeNext:^(NSString * _Nullable x) {
        
        [self formatDeviceId:x];
        
    }];
}

//格式化当前的字符串，每次文本内容发生变更时修改
- (void)formatDeviceId:(NSString *)deviceId {
    
    //获取当前的光标位置
    NSUInteger pos = [self.textField offsetFromPosition:self.textField.beginningOfDocument toPosition:self.textField.selectedTextRange.start];
    
    if (deviceId.length > 11) {
        deviceId = [deviceId substringToIndex:deviceId.length-1];
        self.textField.text = deviceId;
        pos = 11;
    }
    
    //判断是编辑还是删除
    BOOL editFlag = YES;
    if (self.previousTextFieldContent.length >= deviceId.length) {
        editFlag = NO;
    }
    
    //将当前文本框的内容和上一次的内容去掉空格
    deviceId = [deviceId stringByReplacingOccurrencesOfString:@" " withString:@""];
    self.previousTextFieldContent = [self.previousTextFieldContent stringByReplacingOccurrencesOfString:@" " withString:@""];
    
    //用于保存最后的处理结果
    NSString *result = @"";
    for (int i = 0; i < deviceId.length; i++) {
        NSString *tmpStr = [deviceId substringWithRange:NSMakeRange(i, 1)];
        if (i == 3 || i == 6) {
            result = [result stringByAppendingFormat:@" %@", tmpStr];
        }else {
            result = [result stringByAppendingString:tmpStr];
        }
    }
    
    //赋值并记录本次的结果
    self.textField.text = result;
    self.previousTextFieldContent = self.textField.text;
    
    //判断光标位置在4和8的时候，编辑状态就自增1，删除自减1
    if (pos == 4 || pos == 8) {
        if (editFlag) {
            pos += 1;
        }else {
            pos -= 1;
        }
    }
    
    //改变光标位置
    UITextPosition *targetPosition = [self.textField positionFromPosition:[self.textField beginningOfDocument] offset:pos];
    [self.textField setSelectedTextRange:[self.textField textRangeFromPosition:targetPosition toPosition :targetPosition]];
}

- (NSString *)deviceId {
    if (self.textField.text.length == 0) {
        return @"";
    }
    return self.textField.text;
}




@end

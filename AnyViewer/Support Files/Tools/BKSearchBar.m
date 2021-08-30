//
//  BKSearchBar.m
//  sz
//
//  Created by BigKing on 2020/12/9.
//  Copyright © 2020 hqht. All rights reserved.
//

#import "BKSearchBar.h"
#import "NSDictionary+BKAdd.h"

@interface BKSearchBar () <UITextFieldDelegate>

@property (nonatomic, strong) UIImageView *iconImageView;
@property (nonatomic, strong) UITextField *textField;
@property (nonatomic, strong) UIButton *clearBtn;

@end

@implementation BKSearchBar

@synthesize text = _text;

- (instancetype)init {
    if (self = [super init]) {
        [self initContentUI];
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        [self initContentUI];
    }
    return self;
}

- (void)initContentUI {
    _iconImageView = [UIImageView new];
    _iconImageView.contentMode = UIViewContentModeScaleAspectFit;
    _iconImageView.image = [UIImage imageNamed:@"icon_search"];
    [self addSubview:_iconImageView];
    
    _textField = [UITextField new];
    _textField.font = [UIFont systemFontOfSize:14];
    _textField.textColor = UIColor.whiteColor;
//    _textField.clearButtonMode = UITextFieldViewModeAlways;
    _textField.delegate = self;
    _textField.returnKeyType = UIReturnKeySearch;
    _textField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
    _textField.tintColor = UIColor.whiteColor;
    [_textField addTarget:self action:@selector(textChanged:) forControlEvents:UIControlEventEditingChanged];
    [self addSubview:_textField];
    
//    _clearBtn = [UIButton buttonWithType:UIButtonTypeCustom];
//    [_clearBtn setImage:[UIImage imageNamed:@"icon_text_clear"] forState:UIControlStateNormal];
//    [_clearBtn addTarget:self action:@selector(clearBtnAction) forControlEvents:UIControlEventTouchUpInside];
//    [self addSubview:_clearBtn];
    
    self.backgroundColor = BKColor(0x2D2D33);
    

}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    self.iconImageView.frame = CGRectMake(12, self.height/2.f-8, 16, 16);
    
    if (self.hasClearBtn) {
        self.textField.frame = CGRectMake(self.iconImageView.right+12, 0, self.width-self.iconImageView.right-44, self.height);
        self.clearBtn.frame = CGRectMake(self.textField.right, self.height/2.f-16, 32, 32);
    }else {
        self.textField.frame = CGRectMake(self.iconImageView.right+12, 0, self.width-self.iconImageView.right-12, self.height);
    }
    
}

- (void)clearBtnAction {
    self.textField.text = @"";
}

- (void)resignFirstResponder {
    [self.textField resignFirstResponder];
}

- (void)becomeFirstResponder {
    [self.textField becomeFirstResponder];
}

//MARK: - UITextFieldDelegate

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    
    if ([self.delegate respondsToSelector:@selector(searchBarShouldReturn:)]) {
        [self.delegate searchBarShouldReturn:self];
    }
    
    return YES;
}

- (void)textChanged:(UITextField *)textField {
    if (self.maxTextCount > 0 && textField.text.length > self.maxTextCount) {
        textField.text = [textField.text substringToIndex:self.maxTextCount];
    }
    
    if ([self.delegate respondsToSelector:@selector(searchBar:textDidChange:)]) {
        [self.delegate searchBar:self textDidChange:textField.text];
    }
}


//MARK: - setter

- (void)setFont:(UIFont *)font {
    _font = font;
    self.textField.font = font;
}

- (void)setTextColor:(UIColor *)textColor {
    _textColor = textColor;
    self.textField.textColor = textColor;
}

- (void)setText:(NSString *)text {
    _text = text;
    self.textField.text = text;
}

- (void)setPlaceHolder:(NSString *)placeHolder {
    _placeHolder = placeHolder;
    self.textField.attributedPlaceholder = [[NSAttributedString alloc] initWithString:placeHolder attributes:[NSDictionary regularFontSize:13 color:0x747474]];
}

- (void)setReturnKeyType:(UIReturnKeyType)returnKeyType {
    _returnKeyType = returnKeyType;
    self.textField.returnKeyType = returnKeyType;
}

- (void)setKeyboardType:(UIKeyboardType)keyboardType {
    _keyboardType = keyboardType;
    self.textField.keyboardType = keyboardType;
}

- (void)setImageName:(NSString *)imageName {
    _imageName = imageName;
    self.iconImageView.image = [UIImage imageNamed:imageName];
}

- (void)setMaxTextCount:(NSInteger)maxTextCount {
    _maxTextCount = maxTextCount;
}

- (void)setHasClearBtn:(BOOL)hasClearBtn {
    _hasClearBtn = hasClearBtn;
    
    self.clearBtn.hidden = !hasClearBtn;
    
    [self layoutSubviews];
    [self layoutIfNeeded];
}

//MARK: - getter

- (NSString *)text {
    return self.textField.text;
}

@end

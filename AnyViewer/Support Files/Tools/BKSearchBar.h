//
//  BKSearchBar.h
//  sz
//
//  Created by BigKing on 2020/12/9.
//  Copyright © 2020 hqht. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class BKSearchBar;

@protocol BKSearchBarDelegate <NSObject>

@optional

- (void)searchBar:(BKSearchBar *)searchBar textDidChange:(NSString *)searchText;

- (void)searchBarShouldReturn:(BKSearchBar *)searchBar;

@end

@interface BKSearchBar : BaseView

@property (nonatomic, strong) UIFont *font;
@property (nonatomic, strong) UIColor *textColor;
@property (nonatomic, strong) NSString *text;
@property (nonatomic, strong) NSString *placeHolder;
@property (nonatomic, strong) NSString *imageName;
@property (nonatomic, assign) UIReturnKeyType returnKeyType;
@property (nonatomic, assign) UIKeyboardType keyboardType;
@property (nonatomic, assign) NSInteger maxTextCount;
@property (nonatomic, weak) id<BKSearchBarDelegate> delegate;
//是否有清除按钮，默认YES
@property (nonatomic, assign) BOOL hasClearBtn;

- (void)becomeFirstResponder;
- (void)resignFirstResponder;

@end

NS_ASSUME_NONNULL_END

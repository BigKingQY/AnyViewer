//
//  UIScrollView+BKAdd.h
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIScrollView (BKAdd)

- (void)startRefresh;

- (void)addHeaderWithTarget:(nullable id)target action:(nullable SEL)action;
- (void)addFooterWithTarget:(nullable id)target action:(nullable SEL)action;

- (void)removeHeader;
- (void)removeFooter;

- (void)endRefreshing;

- (void)updateFooterWithEnd:(BOOL)isEnd;

- (void)updateHeaderWithEnd:(BOOL)isEnd;

@end

NS_ASSUME_NONNULL_END

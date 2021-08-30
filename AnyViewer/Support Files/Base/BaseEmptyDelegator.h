//
//  BaseEmptyDelegator.h
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import <Foundation/Foundation.h>
#import <UIScrollView+EmptyDataSet.h>

NS_ASSUME_NONNULL_BEGIN

@protocol BaseEmptyDelegatorDelegate <NSObject>

@optional

- (void)emptyDataSet:(UIScrollView *)scrollView didTapView:(UIView *)view;

- (void)emptyDataSet:(UIScrollView *)scrollView didTapButton:(UIButton *)button;

@end

@interface BaseEmptyDelegator : NSObject <DZNEmptyDataSetSource, DZNEmptyDataSetDelegate>

@property (nonatomic) CGFloat offset;

@property (nullable, nonatomic, copy) UIImage *image;

@property (nullable, nonatomic, copy) NSAttributedString *title;
@property (nullable, nonatomic, copy) NSAttributedString *subtitle;
@property (nullable, nonatomic, copy) UIImage *buttonBackImage;
@property (nullable, nonatomic, copy) NSAttributedString *buttonTitle;

@property (nonatomic, weak) id<BaseEmptyDelegatorDelegate> delegate;

- (void)reset;

@end

NS_ASSUME_NONNULL_END

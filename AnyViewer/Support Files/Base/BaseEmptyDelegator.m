//
//  BaseEmptyDelegator.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import "BaseEmptyDelegator.h"

@implementation BaseEmptyDelegator

- (instancetype)init{
    self = [super init];
    if (self) {
        _offset = 0.f;
    }
    return self;
}

- (NSAttributedString *)titleForEmptyDataSet:(UIScrollView *)scrollView {
    return self.title;
}

- (NSAttributedString *)descriptionForEmptyDataSet:(UIScrollView *)scrollView {
    return self.subtitle;
}

- (UIImage *)imageForEmptyDataSet:(UIScrollView *)scrollView {
    return self.image;
}


- (NSAttributedString *)buttonTitleForEmptyDataSet:(UIScrollView *)scrollView forState:(UIControlState)state {
    return self.buttonTitle;
}

- (UIImage *)buttonImageForEmptyDataSet:(UIScrollView *)scrollView forState:(UIControlState)state {
    return nil;
}

- (UIImage *)buttonBackgroundImageForEmptyDataSet:(UIScrollView *)scrollView forState:(UIControlState)state {
    return self.buttonBackImage;
}

- (UIColor *)backgroundColorForEmptyDataSet:(UIScrollView *)scrollView {
    return nil;
}

- (CGFloat)verticalOffsetForEmptyDataSet:(UIScrollView *)scrollView {
    return self.offset;
}

- (CGFloat)spaceHeightForEmptyDataSet:(UIScrollView *)scrollView {
    return 20.f;
}

//MARK: - delegate

- (BOOL)emptyDataSetShouldFadeIn:(UIScrollView *)scrollView {
    return YES;
}

- (BOOL)emptyDataSetShouldBeForcedToDisplay:(UIScrollView *)scrollView {
    return NO;
}

- (BOOL)emptyDataSetShouldDisplay:(UIScrollView *)scrollView {
    return YES;
}

- (BOOL)emptyDataSetShouldAllowTouch:(UIScrollView *)scrollView {
    return YES;
}


- (BOOL)emptyDataSetShouldAllowScroll:(UIScrollView *)scrollView {
    return YES;
}

- (BOOL)emptyDataSetShouldAnimateImageView:(UIScrollView *)scrollView {
    return YES;
}



- (void)emptyDataSet:(UIScrollView *)scrollView didTapView:(UIView *)view {
    if ([self.delegate respondsToSelector:@selector(emptyDataSet:didTapView:)]) {
        [self.delegate emptyDataSet:scrollView didTapView:view];
    }
}


- (void)emptyDataSet:(UIScrollView *)scrollView didTapButton:(UIButton *)button {
    if ([self.delegate respondsToSelector:@selector(emptyDataSet:didTapButton:)]) {
        [self.delegate emptyDataSet:scrollView didTapButton:button];
    }
}

- (void)reset {
    self.image = nil;
    self.title = nil;
    self.subtitle = nil;
    self.buttonBackImage = nil;
    self.buttonTitle = nil;
}

@end

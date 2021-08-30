//
//  BasePageViewController.h
//  JZMerchant
//
//  Created by BigKing on 2021/5/10.
//

#import "BaseViewController.h"

NS_ASSUME_NONNULL_BEGIN

@interface BasePageViewController : BaseViewController <JXCategoryViewDelegate, JXCategoryListContainerViewDelegate>

@property (nonatomic, strong) JXCategoryListContainerView *listContainerView;
@property (nonatomic, strong) JXCategoryTitleView *categoryView;
@property (nonatomic, strong) NSArray<__kindof BaseViewController *> *controllers;
@property (nonatomic, assign) NSInteger currentIdx;

@end

NS_ASSUME_NONNULL_END

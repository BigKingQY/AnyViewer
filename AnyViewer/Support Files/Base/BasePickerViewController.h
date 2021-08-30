//
//  BasePickerViewController.h
//  JZMerchant
//
//  Created by BigKing on 2021/5/14.
//

#import "BaseViewController.h"

NS_ASSUME_NONNULL_BEGIN

@interface BasePickerViewController : BaseViewController

@property (nonatomic, strong) NSArray<BKCellModel *> *dataSource;

@property (nonatomic, copy) void(^selectedBlock)(BKCellModel *model);

@property (nonatomic, strong) NSString *naviTitle;

@end

NS_ASSUME_NONNULL_END

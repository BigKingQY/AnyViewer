//
//  AMSettingListTableViewCell.h
//  AnyViewer
//
//  Created by BigKing on 2021/8/25.
//

#import "BaseTableViewCell.h"

NS_ASSUME_NONNULL_BEGIN

UIKIT_EXTERN NSString *const kAMSettingListTableViewCellId;

@interface AMSettingListTableViewCell : BaseTableViewCell

@property (nonatomic, strong) BKCellModel *model;

@end

NS_ASSUME_NONNULL_END

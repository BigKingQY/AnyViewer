//
//  AMDeviceListTableViewCell.h
//  AnyViewer
//
//  Created by BigKing on 2021/8/25.
//

#import "BaseTableViewCell.h"
#import "AMDevice.h"

NS_ASSUME_NONNULL_BEGIN

UIKIT_EXTERN NSString *const kAMDeviceListTableViewCellId;

@interface AMDeviceListTableViewCell : BaseTableViewCell

@property (nonatomic, strong) AMDevice *device;

@end

NS_ASSUME_NONNULL_END

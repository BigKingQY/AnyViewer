//
//  AMDeviceControlViewController.h
//  AnyViewer
//
//  Created by BigKing on 2021/8/25.
//

#import "BaseViewController.h"
#include "Typedef.h"

NS_ASSUME_NONNULL_BEGIN

@interface AMDeviceControlViewController : BaseViewController


- (instancetype)initWithDeviceId:(U64)deviceId sessionId:(U32)sessionId;

- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)new NS_UNAVAILABLE;


@end

NS_ASSUME_NONNULL_END

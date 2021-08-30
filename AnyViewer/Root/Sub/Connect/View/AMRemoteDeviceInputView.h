//
//  AMRemoteDeviceInputView.h
//  AnyViewer
//
//  Created by BigKing on 2021/8/24.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface AMRemoteDeviceInputView : UIView

///获取当前的设备ID，包含了空格符
@property (nonatomic, strong, readonly) NSString *deviceId;

///连接按钮
@property (nonatomic, strong, readonly) BaseButton *connButton;

@end

NS_ASSUME_NONNULL_END

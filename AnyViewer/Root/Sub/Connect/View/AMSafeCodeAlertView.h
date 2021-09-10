//
//  AMSafeCodeAlertView.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/7.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface AMSafeCodeAlertView : UIView

@property (nonatomic, strong, readonly) BaseTextField *textField;

@property (nonatomic, strong, readonly) BaseButton *doneButton;


- (void)showViewWithDeviceId:(NSString *)deviceId nickName:(NSString *)nickName;

- (void)hide;

- (BOOL)isShow;

- (void)startLoading;


@end

NS_ASSUME_NONNULL_END

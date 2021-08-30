//
//  AMSafeCodeAlertViewController.h
//  AnyViewer
//
//  Created by BigKing on 2021/8/24.
//

#import "BaseViewController.h"

NS_ASSUME_NONNULL_BEGIN

typedef void(^AMSafeCodeAlertCallBack)(NSString *safeCode);

@interface AMSafeCodeAlertViewController : BaseViewController

@property (nonatomic, copy) AMSafeCodeAlertCallBack callback;

@end

NS_ASSUME_NONNULL_END

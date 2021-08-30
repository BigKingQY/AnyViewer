//
//  MBProgressHUD+BKAdd.h
//  BKStandProject
//
//  Created by BigKing on 2021/3/3.
//

#import <MBProgressHUD/MBProgressHUD.h>

NS_ASSUME_NONNULL_BEGIN

@interface MBProgressHUD (BKAdd)

+ (void)showLoading:(NSString *)text onView:(UIView *)view;

+ (void)showSuccess:(NSString *)text onView:(UIView *)view;

+ (void)showMessage:(NSString *)text onView:(UIView *)view;

+ (void)hideForView:(UIView *)view;


+ (void)showLoadingOnWindow:(NSString *)text;

+ (void)showSuccessOnWindow:(NSString *)text;

+ (void)showMessageOnWindow:(NSString *)text;

+ (void)hideForWindow;


+ (void)hideForViewAndWindow;

@end

NS_ASSUME_NONNULL_END

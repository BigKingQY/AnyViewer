//
//  BKFingerWaveView.h
//  AnyViewer
//
//  Created by BigKing on 2021/8/25.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface BKFingerWaveView : UIView

///根据center给view添加一个波纹效果
+ (instancetype)showInView:(UIView *)view center:(CGPoint)center;

@end

NS_ASSUME_NONNULL_END

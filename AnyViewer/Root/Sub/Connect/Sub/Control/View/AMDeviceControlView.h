//
//  AMDeviceControlView.h
//  AnyViewer
//
//  Created by BigKing on 2021/8/25.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface AMDeviceControlView : UIView

@property (nonatomic, strong, readonly) UIImageView *contentImageView;

- (void)updateFrameWithImage:(UIImage *)image;

- (void)updateFrameSize:(const unsigned int)width height:(const unsigned int)height;

@end

NS_ASSUME_NONNULL_END

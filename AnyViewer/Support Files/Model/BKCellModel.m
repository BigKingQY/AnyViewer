//
//  BKCellModel.m
//  sz
//
//  Created by BigKing on 2020/11/14.
//  Copyright © 2020 hqht. All rights reserved.
//

#import "BKCellModel.h"

@implementation BKCellModel

- (instancetype)init {
    if (self = [super init]) {
        _canEdit = YES;
        _keyboardType = UIKeyboardTypeDefault;
    }
    return self;
}

+ (BKCellModel *)initWithTitle:(nullable NSString *)title icon:(nullable NSString *)icon block:(nullable BKCellModelBlock)block {
    BKCellModel *model = [BKCellModel new];
    model.title = title;
    model.icon = icon;
    model.block = block;
    
    return model;
}

@end


@implementation BKGroupCellModel

- (instancetype)init {
    if (self = [super init]) {
        _headerHeight = CGFLOAT_MIN;
        _footerHeight = CGFLOAT_MIN;
    }
    return self;
}

@end

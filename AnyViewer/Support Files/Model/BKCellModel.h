//
//  BKCellModel.h
//  sz
//
//  Created by BigKing on 2020/11/14.
//  Copyright © 2020 hqht. All rights reserved.
//  可通用的模型

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, BKCellModelType) {
    BKCellModelTypeNone = 0,
    BKCellModelType1,
    BKCellModelType2,
    BKCellModelType3,
    BKCellModelType4,
};

@class BKCellModel;

typedef void(^BKCellModelBlock)(BKCellModel *model);

@interface BKCellModel : NSObject

@property (nullable, nonatomic, copy) NSString *Id;
@property (nullable, nonatomic, copy) NSString *itemId;
@property (nullable, nonatomic, copy) NSString *title;
@property (nullable, nonatomic, copy) NSAttributedString *attributedString;
@property (nullable, nonatomic, copy) NSString *subTitle;
@property (copy,nonatomic)NSAttributedString *subAttributedString;
@property (nullable, nonatomic, copy) NSString *icon;

@property (nullable, nonatomic, copy) NSString *placeHolder;
@property (nullable, nonatomic, copy) NSString *cellClass;

@property (nullable, nonatomic, copy) NSString *address;
@property (nullable, nonatomic, copy) NSString *time;

@property (nullable, nonatomic, copy) NSString *key;
@property (nullable, nonatomic, strong) id value;

@property (nullable, nonatomic, strong) UIImage *image;

@property (nonatomic, assign) CGFloat itemSpace;

@property (nonatomic, assign) CGFloat lat;
@property (nonatomic, assign) CGFloat lon;

@property (nullable, nonatomic, strong) UIFont *font;
@property (nullable, nonatomic, strong) UIColor *textColor;

@property (nullable, nonatomic, strong) UIFont *subFont;
@property (nullable, nonatomic, strong) UIColor *subTextColor;
@property (nonatomic, assign) NSTextAlignment subAlignment;

@property (nullable, nonatomic, strong) UIColor *color;
@property (nonatomic, assign) CGSize size;

@property (nonatomic, assign) CGFloat rowHeight;
@property (nonatomic, assign) NSInteger row;
@property (nonatomic, assign) NSInteger count;

@property (nonatomic, assign) BOOL hasSwitch;
@property (nonatomic, assign) BOOL isOn;
@property (nonatomic, assign) BOOL isSelected;
@property (nonatomic, assign) BOOL canEdit;

@property (nonatomic, strong) id data;

@property (nonatomic, assign) UIKeyboardType keyboardType;

@property (nonatomic, assign) BKCellModelType type;

@property (nonatomic, copy) BKCellModelBlock block;

@property (nonatomic, strong) NSArray<BKCellModel *> *models;

+ (BKCellModel *)initWithTitle:(nullable NSString *)title icon:(nullable NSString *)icon block:(nullable BKCellModelBlock)block;

@end

@interface BKGroupCellModel : NSObject

@property (nonatomic, strong) NSArray<BKCellModel *> *models;

@property (nonatomic, assign) CGFloat headerHeight;
@property (nonatomic, assign) CGFloat footerHeight;

@property (nonatomic, copy) NSString *headerTitle;
@property (nonatomic, copy) NSString *footerTitle;

@property (nullable, nonatomic, copy) NSString *cellClass;
@property (nonatomic, assign) CGSize size;
@property (nonatomic, assign) UIEdgeInsets insets;
@property (nonatomic, assign) CGFloat lineSpace;
@property (nonatomic, assign) CGFloat itemSpace;

@end

NS_ASSUME_NONNULL_END

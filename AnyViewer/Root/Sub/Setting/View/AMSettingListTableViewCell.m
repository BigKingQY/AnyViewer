//
//  AMSettingListTableViewCell.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/25.
//

#import "AMSettingListTableViewCell.h"

NSString *const kAMSettingListTableViewCellId = @"kAMSettingListTableViewCellId";

@interface AMSettingListTableViewCell ()

@property (nonatomic, strong) UIImageView *iconImageView;
@property (nonatomic, strong) UILabel *titleLabel;
@property (nonatomic, strong) UIImageView *arrowImageView;

@end

@implementation AMSettingListTableViewCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        
        _iconImageView = [UIImageView new];
        _iconImageView.contentMode = UIViewContentModeScaleAspectFit;
        _iconImageView.layer.cornerRadius = 15;
        [self.contentView addSubview:_iconImageView];
        
        _titleLabel = [UILabel new];
        _titleLabel.font = BKFont(16);
        _titleLabel.textColor = UIColor.whiteColor;
        [self.contentView addSubview:_titleLabel];
        
        _arrowImageView = [UIImageView new];
        _arrowImageView.contentMode = UIViewContentModeScaleAspectFit;
        _arrowImageView.image = BKImage(@"icon_arrow_right");
        [self.contentView addSubview:_arrowImageView];
        
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    self.iconImageView.frame = CGRectMake(30, self.contentView.height/2.f-15.f, 30, 30);
    self.titleLabel.frame = CGRectMake(self.iconImageView.right+15, 0, self.contentView.width-self.iconImageView.right-120, self.contentView.height);
    self.arrowImageView.frame = CGRectMake(self.contentView.width-44, self.contentView.height/2.f-6.f, 14, 14);
}

- (void)setModel:(BKCellModel *)model {
    _model = model;
    
    self.iconImageView.image = BKImage(model.icon);
    self.titleLabel.text = model.title;
    
    self.iconImageView.backgroundColor = UIColor.blueColor;
    
}

@end

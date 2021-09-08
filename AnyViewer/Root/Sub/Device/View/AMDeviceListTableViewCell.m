//
//  AMDeviceListTableViewCell.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/25.
//

#import "AMDeviceListTableViewCell.h"

NSString *const kAMDeviceListTableViewCellId = @"kAMDeviceListTableViewCellId";

@interface AMDeviceListTableViewCell ()

@property (nonatomic, strong) BaseView *bgView;
@property (nonatomic, strong) UIImageView *bgImageView;
@property (nonatomic, strong) UILabel *nameLabel;
@property (nonatomic, strong) UILabel *codeLabel;

@end

@implementation AMDeviceListTableViewCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        
        _bgView = [BaseView new];
        _bgView.bk_cornerRadius = 20;
        [self.contentView addSubview:_bgView];
        
        _bgImageView = [UIImageView new];
        _bgImageView.image = [UIImage imageNamed:@""];
        _bgImageView.backgroundColor = UIColor.blueColor;
        [_bgView addSubview:_bgImageView];
        
        _nameLabel = [UILabel new];
        _nameLabel.font = BKFont(18);
        _nameLabel.textColor = UIColor.whiteColor;
        [_bgImageView addSubview:_nameLabel];
        
        _codeLabel = [UILabel new];
        _codeLabel.font = BKFont(14);
        _codeLabel.textColor = UIColor.whiteColor;
        [_bgImageView addSubview:_codeLabel];

    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    self.bgView.frame = CGRectMake(25, 10, self.contentView.width-50, self.contentView.height-20);
    self.bgImageView.frame = self.bgView.bounds;
    self.nameLabel.frame = CGRectMake(20, 25, self.bgImageView.width-40, 20);
    self.codeLabel.frame = CGRectMake(20, self.nameLabel.bottom+10, self.nameLabel.width, 16);
}

- (void)setUser:(BKUser *)user {
    _user = user;
    
    self.nameLabel.text = user.nickName;
    self.codeLabel.text = [NSString stringWithFormat:@"%llu", user.deviceId];
}


@end

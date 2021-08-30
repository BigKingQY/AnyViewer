//
//  AMSettingViewController.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/23.
//

#import "AMSettingViewController.h"
#import "AMSettingListTableViewCell.h"

@interface AMSettingViewController ()

@property (nonatomic, strong) UILabel *titleLabel;

@property (nonatomic, strong) NSArray<BKCellModel *> *dataSource;

@end

@implementation AMSettingViewController

//MARK: LifeCycle - 生命周期

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self addNotification];
    [self configUISet];
    [self loadData];
    [self addActions];
}

//MARK: Methods Override - 方法重写

//MARK: UISet - 界面布局

- (void)configUISet {
    
    self.gk_navigationBar.hidden = YES;
    [self.view addSubview:self.titleLabel];
    
    
    [self.tableView registerClass:[AMSettingListTableViewCell class] forCellReuseIdentifier:kAMSettingListTableViewCellId];
    self.tableView.rowHeight = 60;
}

//MARK: Layout - 布局设置

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    
    self.titleLabel.frame = CGRectMake(25, KSTATUS_BAR_H, self.view.width-50, KNAVIGATION_BAR_H);
    self.tableView.frame = CGRectMake(0, KSTATUS_NAVIGATION_H, self.view.width, self.view.height-KSTATUS_NAVIGATION_H);
}

//MARK: Network Request - 网络请求

- (void)loadData {

    BKCellModel *setting = [BKCellModel initWithTitle:@"设置" icon:@"" block:^(BKCellModel * _Nonnull model) {
        
    }];
    
    BKCellModel *operat = [BKCellModel initWithTitle:@"操作指南" icon:@"" block:^(BKCellModel * _Nonnull model) {
        
    }];
    
    BKCellModel *newbe = [BKCellModel initWithTitle:@"新手教程" icon:@"" block:^(BKCellModel * _Nonnull model) {
        
    }];
    
    BKCellModel *invit = [BKCellModel initWithTitle:@"邀请伙伴" icon:@"" block:^(BKCellModel * _Nonnull model) {
        
    }];
    
    BKCellModel *feedback = [BKCellModel initWithTitle:@"问题反馈" icon:@"" block:^(BKCellModel * _Nonnull model) {
        
    }];
    
    BKCellModel *comment = [BKCellModel initWithTitle:@"给我们好评" icon:@"" block:^(BKCellModel * _Nonnull model) {
        
    }];
    
    self.dataSource = @[setting, operat, newbe, invit, feedback, comment];
    
    [self.tableView reloadData];
}

//MARK: Delegate && DataSource - 代理

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.dataSource.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    AMSettingListTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:kAMSettingListTableViewCellId forIndexPath:indexPath];
    cell.model = self.dataSource[indexPath.row];
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    BKCellModel *model = self.dataSource[indexPath.row];
    if (model.block) {
        model.block(model);
    }
}

//MARK: EventResponse - 事件处理

- (void)addActions {}

//MARK: Private Methods - 私有方法

//MARK: Public  Methods - 公用方法

//MARK: NotificationCenter - 通知中心

- (void)addNotification {}

//MARK: Setter

//MARK: Getter

- (UILabel *)titleLabel {
    if (!_titleLabel) {
        _titleLabel = [UILabel new];
        _titleLabel.font = BKFont(24);
        _titleLabel.textColor = UIColor.whiteColor;
        _titleLabel.text = @"设置";
    }
    return _titleLabel;
}

@end

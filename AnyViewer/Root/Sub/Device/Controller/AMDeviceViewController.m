//
//  AMDeviceViewController.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/23.
//

#import "AMDeviceViewController.h"
#import "AMDeviceListTableViewCell.h"

@interface AMDeviceViewController ()

@property (nonatomic, strong) UILabel *titleLabel;

@property (nonatomic, strong) NSMutableArray<AMDevice *> *dataSource;

@end

@implementation AMDeviceViewController

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
    
    [self.tableView registerClass:[AMDeviceListTableViewCell class] forCellReuseIdentifier:kAMDeviceListTableViewCellId];
    self.tableView.rowHeight = 140;
}

//MARK: Layout - 布局设置

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    
    self.titleLabel.frame = CGRectMake(25, KSTATUS_BAR_H, self.view.width-50, KNAVIGATION_BAR_H);
    self.tableView.frame = CGRectMake(0, KSTATUS_NAVIGATION_H, self.view.width, self.view.height-KSTATUS_NAVIGATION_H);
}

//MARK: Network Request - 网络请求

- (void)loadData {
    
    for (int i = 0; i < 10; i++) {
        AMDevice *device = [AMDevice new];
        device.name = @"KYO";
        device.code = @"123 345 456";
        
        [self.dataSource addObject:device];
    }
    
    [self.tableView reloadData];
}

//MARK: Delegate && DataSource - 代理

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.dataSource.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    AMDeviceListTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:kAMDeviceListTableViewCellId forIndexPath:indexPath];
    cell.device = self.dataSource[indexPath.row];
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
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
        _titleLabel.text = @"最近连接";
    }
    return _titleLabel;
}

- (NSMutableArray<AMDevice *> *)dataSource {
    if (!_dataSource) {
        _dataSource = [NSMutableArray array];
    }
    return _dataSource;
}

@end

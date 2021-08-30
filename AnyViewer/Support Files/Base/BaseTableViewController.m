//
//  BaseTableViewController.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import "BaseTableViewController.h"

@implementation BaseTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self.view addSubview:self.tableView];
}

//MARK: Delegate && DataSource - 代理

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section {
    return CGFLOAT_MIN;
}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section {
    return CGFLOAT_MIN;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"cell"];
    if (!cell) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"cell"];
    }
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {}

//MARK: Public Methods

- (void)startRefresh {
    self.pageNum = 1;
    self->_isEnd = NO;
    [self.tableView startRefresh];
}

- (void)endRefreshing {
    [self.tableView endRefreshing];
}

- (BOOL)isLoadMore {
    return self.pageNum > 1;
}

- (void)resetEnd:(BOOL)isEnd {
    self->_isEnd = isEnd;
}

- (void)addRefreshHeader {
    [self.tableView addHeaderWithTarget:self action:@selector(refreshData)];
}

- (void)removeHeader {
    [self.tableView removeHeader];
}

- (void)addMoreFooter {
    [self.tableView addFooterWithTarget:self action:@selector(loadMoreData)];
}

- (void)removeFooter {
    [self.tableView removeFooter];
}

- (void)addEmptyDelegator {
    self.emptyDelegator = [BaseEmptyDelegator new];
    self.tableView.emptyDataSetDelegate = self.emptyDelegator;
    self.tableView.emptyDataSetSource = self.emptyDelegator;
}

- (void)refreshData {}

- (void)loadMoreData {}

//MARK: Public EmptyDelegator

- (void)emptyDelegatorSubTitle:(NSString *)subTitle {
    [self.emptyDelegator reset];
    self.emptyDelegator.subtitle = [[NSAttributedString alloc] initWithString:subTitle attributes:@{NSFontAttributeName:[UIFont systemFontOfSize:20], NSForegroundColorAttributeName:[UIColor colorWithHexString:@"666666"]}];
}

- (void)emptyDelegatorImage:(NSString *)imageName {
    [self.emptyDelegator reset];
    self.emptyDelegator.image = [UIImage imageNamed:imageName];
}

- (void)emptyDelegatorImage:(NSString *)imageName subTitle:(NSString *)subTitle {
    [self.emptyDelegator reset];
    self.emptyDelegator.image = [UIImage imageNamed:imageName];
    self.emptyDelegator.subtitle = [[NSAttributedString alloc] initWithString:subTitle attributes:@{NSFontAttributeName:[UIFont systemFontOfSize:16], NSForegroundColorAttributeName:[UIColor colorWithHexString:@"666666"]}];
}

- (void)emptyDelegatorOffset:(CGFloat)offset {
    self.emptyDelegator.offset = offset;
}

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    
    if (self.navigationController) {
        self.tableView.frame = CGRectMake(0, KSTATUS_NAVIGATION_H, self.view.width, self.view.height-KHOME_INDICATOR_H-KSTATUS_NAVIGATION_H);
    }else {
        self.tableView.frame = CGRectMake(0, KSTATUS_BAR_H, self.view.width, self.view.height-KHOME_INDICATOR_H-KSTATUS_BAR_H);
    }
}

- (UITableView *)tableView {
    if (!_tableView) {
        _tableView = [[UITableView alloc] initWithFrame:CGRectZero style:UITableViewStyleGrouped];
        _tableView.delegate = self;
        _tableView.dataSource = self;
        _tableView.estimatedRowHeight = 0;
        _tableView.estimatedSectionFooterHeight = 0;
        _tableView.estimatedSectionHeaderHeight = 0;
        _tableView.delaysContentTouches = NO;
        _tableView.canCancelContentTouches = YES;
        _tableView.showsHorizontalScrollIndicator = NO;
        _tableView.backgroundColor = UIColor.clearColor;
        _tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
        _tableView.keyboardDismissMode = UIScrollViewKeyboardDismissModeOnDrag;
        _tableView.tableHeaderView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, CGFLOAT_MIN, CGFLOAT_MIN)];
        _tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, CGFLOAT_MIN, CGFLOAT_MIN)];
        if (@available(iOS 11.0, *)) {
            _tableView.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
        }
    }
    return _tableView;
}

@end

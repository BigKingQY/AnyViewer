//
//  BaseTableViewController.h
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import "BaseViewController.h"
#import "BaseEmptyDelegator.h"
#import "UIScrollView+BKAdd.h"

NS_ASSUME_NONNULL_BEGIN

@interface BaseTableViewController : BaseViewController <UITableViewDelegate, UITableViewDataSource>

@property (nonatomic, assign, readonly) BOOL isEnd;
@property (nonatomic, assign) NSInteger pageNum;
@property (nonatomic, assign) NSInteger totalPage;
@property (nonatomic, strong) UITableView *tableView;

@property (nullable, nonatomic, strong) BaseEmptyDelegator *emptyDelegator;

/// 开始刷新
- (void)startRefresh;
/// 结束刷新
- (void)endRefreshing;

/// 下拉刷新，需要重写此方法
- (void)refreshData;
/// 上拉加载，需要重写此方法
- (void)loadMoreData;

/// 是否是加载更多
- (BOOL)isLoadMore;

/// 重置end状态
- (void)resetEnd:(BOOL)isEnd;

/// 添加刷新Header
- (void)addRefreshHeader;
/// 移除刷新Header
- (void)removeHeader;

/// 添加上拉刷新Footer
- (void)addMoreFooter;
/// 移除上拉刷新Footer
- (void)removeFooter;

/// 添加EmptyDelegator
- (void)addEmptyDelegator;

/// 只设置副标题
- (void)emptyDelegatorSubTitle:(NSString *)subTitle;
/// 只设置图片
- (void)emptyDelegatorImage:( NSString *)imageName;
/// 同时设置图片及副标题
- (void)emptyDelegatorImage:(NSString *)imageName subTitle:(NSString *)subTitle;
/// 设置偏移量
- (void)emptyDelegatorOffset:(CGFloat)offset;

@end

NS_ASSUME_NONNULL_END
